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
#include <pthread.h>
#include <assert.h>
#include "stack.hpp"
#include "dyalloc.hpp"
#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
pthread_mutex_t mutex;

struct stack *stack_server;

struct stack_listen_args {
    char *arg1;
	pthread_mutex_t *arg2;
	int sockfd;
};


void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



//test the stack_command function
void test_free_stack() {
    //create a stack
	printf("test_free_stack\n");
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    assert(free_stack(stack_server) == 0);
}
void test_push_stack() {
    //create a stack
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
	struct node *new_node = (struct node *)dynmic_alloc(sizeof(struct node));
    assert(push(stack_server, new_node) == 0);
}
void test_pop_stack() {
    //create a stack
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
	struct node *new_node = (struct node *)dynmic_alloc(sizeof(struct node));
	new_node->data = data;
    assert(pop(stack_server, data) == -1);
    assert(push(stack_server, new_node) == 0);
    assert(pop(stack_server, data) == 0);
}

void test_top_stack() {
    //create a stack
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
	struct node *new_node = (struct node *)dynmic_alloc(sizeof(struct node));
	new_node->data = data;
	strcpy(data, "test_data: nir");
    assert(top(stack_server, data) == -1);
    assert(push(stack_server, new_node) == 0);
    assert(top(stack_server, data) == 0);
}
void test_stack_command() {

    //create a stack
    char *stack_name = "test_stack2";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //try to push a value
	struct node *new_node = (struct node *)dynmic_alloc(sizeof(struct node));
	new_node->data = "1";
	struct node *new_node2 = (struct node *)dynmic_alloc(sizeof(struct node));
	new_node2->data = "2";
	struct node *new_node3 = (struct node *)dynmic_alloc(sizeof(struct node));
	new_node3->data = "3";

    push(stack_server, new_node);
    //try to push a value
    push(stack_server, new_node2);
    //try to push a value
    push(stack_server, new_node3);
    //check if values are in the stack with top
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
    top(stack_server, data);
    //check if data is 3
    printf("%s\n", data);
    assert(strcmp(data, "3") == 0);
    //pop the value
    pop(stack_server, data);
    top(stack_server, data);
    //check if data is 3
    printf("%s\n", data);
    assert(strcmp(data, "2") == 0);
    //pop the value
    pop(stack_server, data);
    top(stack_server, data);
    //check if data is 3
    printf("%s\n", data);
    assert(strcmp(data, "1") == 0);
}
int main(){
    test_stack_command();
    test_free_stack();
    test_push_stack();
    test_pop_stack();
    test_top_stack();
    return 0;
}