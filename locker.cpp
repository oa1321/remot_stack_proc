// implemant lockking using fcntl
#include "locker.hpp"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

#include "dyalloc.hpp"

struct sock_lock* create_a_sock() {
    struct sock_lock* sock = (struct sock_lock*)dynmic_alloc(sizeof(struct sock_lock));
    // buffer to hold the temporary file name
    char nameBuff[32];
    int filedes = -1;
    // memset the buffers to 0
    memset(nameBuff, 0, sizeof(nameBuff));

    // Copy the relevant information in the buffers
    strncpy(nameBuff, "/tmp/locker-XXXXXX", 18);

    errno = 0;
    // Create the temporary file, this function will replace the 'X's
    filedes = mkostemp(nameBuff, O_RDWR | O_CREAT);
    fchmod(filedes, 0666);

    // Call unlink so that whenever the file is closed or the program exits
    // the temporary file is deleted
    unlink(nameBuff);

    if (filedes < 1) {
        printf("\n Creation of temp file failed with error [%s]\n", strerror(errno));
        return NULL;
    }
    sock->fd = filedes;
    // sock->fd = open("/tmp/locker", O_RDWR | O_CREAT, 0666);
    sock->lock.l_type = 0;
    sock->lock.l_whence = SEEK_SET;
    sock->lock.l_start = 0;
    sock->lock.l_len = 0;
    return sock;
}
int get_if_locked(struct sock_lock* sock) {
    fcntl(sock->fd, F_GETLK, &sock->lock);
    return sock->lock.l_type;
}
int lock_sock(struct sock_lock* sock) {
    while (get_if_locked(sock) == F_WRLCK) {
    }
    sock->pid = getppid();
    sock->lock.l_type = F_WRLCK;
    return fcntl(sock->fd, F_SETLKW, &sock->lock);
}
int unlock_sock(struct sock_lock* sock) {
    sock->lock.l_type = F_UNLCK;
    return fcntl(sock->fd, F_SETLKW, &sock->lock);
}
