//implemant lockking using fcntl
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "dyalloc.hpp"

struct sock_lock {
    int pid;
    int fd;
    struct flock lock;
};

struct sock_lock * create_a_sock();
int lock_sock(struct sock_lock * sock);
int unlock_sock(struct sock_lock * sock);

