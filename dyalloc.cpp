#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/mman.h>

void *dynmic_alloc(size_t size) {
    void *p = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    return p;
}


void dynmic_free(void *ptr) {
    munmap(ptr, sizeof(ptr));
}
