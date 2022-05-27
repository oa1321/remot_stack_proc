/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "stack.hpp"
#include "dyalloc.hpp"
#include "locker.hpp"
#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold
// the stack
struct stack *stack_server;

// First, a pointer-size-related definition, in case
// this code is being compiled in 32-bit rather than 
// 64-bit mode; if you know the code is always 64-bit
// you can just use the "l"

struct flock lock;


void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	errno = saved_errno;
}

int do_stack_command(char *buffer, struct stack *s, struct node *n, char *data, int new_fd,struct sock_lock * lock)
{
	lock_sock(lock);
	printf("data: %s\n", buffer);
	if (!strcmp(buffer, "POP\n"))
	{
		// send debug message
		pop(s, data);
		unlock_sock(lock);
		send(new_fd, data, strlen(data), 0);
		return 0;
	}
	else if (!strcmp(buffer, "TOP\n"))
	{
		// send output message
		top(s, data);
		char temp[2048];
		strcpy(temp, "OUTPUT: ");
		strcat(temp, data);
		unlock_sock(lock);
		send(new_fd, temp, strlen(temp), 0);
		return 0;
	}
	if (strlen(buffer) < 6)
	{
		// send error message
		strcpy(data, "ERROR: INVALID COMMAND");
		unlock_sock(lock);
		send(new_fd, data, strlen(data), 0);
		return 0;
	}

	char *token = strtok(buffer, " ");
	printf("token: %s\n", token);
	strcpy(data, (buffer+5));
	data[strlen(data)-1] = '\0';
	if (!strcmp(token, "PUSH"))
	{
		push(s, n);
		// send debug message
		unlock_sock(lock);
		send(new_fd, data, strlen(data), 0);
		return 1;
	}
	else
	{
		// send error message
		strcpy(data, "ERROR: INVALID COMMAND");
		unlock_sock(lock);
		send(new_fd, data, strlen(data), 0);
		return 0;
	}
	unlock_sock(lock);
	return 0;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		printf("ERROR: getaddrinfo\n");
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			printf("ERROR: server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1)
		{
			printf("ERROR: setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			printf("ERROR: server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		printf("DEBUG: server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		printf("ERROR: listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		printf("ERROR: sigaction");
		exit(1);
	}

	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;

	printf("server: waiting for connections...\n");
	// create a stack
	stack_server = (struct stack *)create_stack("SERVER STACK");
	int* my_i = (int*)dynmic_alloc(sizeof(int));
	*my_i = 0;
	struct sock_lock * my_sock = create_a_sock();
	while (1)
	{ // main accept() loop
		print_stack(stack_server);
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			printf("ERROR: accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
				  get_in_addr((struct sockaddr *)&their_addr),
				  s, sizeof s);
		printf("DEBUG: server: got connection from %s\n", s);
		struct node *new_node = (struct node *)dynmic_alloc(sizeof(struct node));
		char *data = (char *)dynmic_alloc(MAX_DATA*(sizeof(char)));
		new_node->data = data;
		if (!fork())
		{ // this is the child process
			char buffer[MAX_DATA];
			close(sockfd); // child doesn't need the listener
			int n = read(new_fd, buffer, MAX_DATA);
			if (n <= 0)
				perror("ERROR reading from socket");
			buffer[n] = '\0';
			printf("DEBUG: server: received %s\n", buffer);
			if (!do_stack_command(buffer, stack_server, new_node, data, new_fd, my_sock))
			{
				dynmic_free(new_node->data);
				dynmic_free(new_node);
				close(new_fd);
			}
			
			(*my_i)++;
			printf("\nsize %d\n", *my_i);
			exit(0);
		}
		close(new_fd); // parent doesn't need this
	}

	return 0;
}
/*
|------------------------------------|
|======= notes interface  =======    |
|------------------------------------|
*/