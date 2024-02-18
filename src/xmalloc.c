#include <stdio.h>
#include <stdlib.h>

#include "xmalloc.h"

void *xmalloc(size_t len) {
    void *ptr = malloc(len);
    if (ptr == NULL) {
        perror("malloc");
        exit(1);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t len) {
    void *new_ptr = ptr ? realloc(ptr, len) : xmalloc(len);
    if (new_ptr == NULL) {
        perror("realloc");
        exit(1);
    }
    return new_ptr;
}