#include <stdlib.h>

#include "packages.h"
#include "../string_utils.h"
#include "../xmalloc.h"

// Note: Packages.value is a list of strings which has to be
// previously allocated, free() will be called on them.

struct Packages *initialise_packages(void) {
    struct Packages *packages = xmalloc(sizeof(struct Packages));

    packages->len = 0;
    packages->cap = 10;
    packages->value = xmalloc(sizeof(char*) * packages->cap);

    return packages;
}

void push_package(struct Packages *packages, char *package) {
    packages->value[packages->len++] = package;

    if (packages->len >= packages->cap) {
        packages->cap += 10;
        packages->value = xrealloc(packages->value, sizeof(char*) * packages->cap);
    }
}

void remove_package(struct Packages *packages, char *package) {
    for (size_t i = 0; i < packages->len; ++i) {
        if (streql(packages->value[i], package)) {
            free(packages->value[i]);
            packages->value[i] = packages->value[--packages->len];
            break;
        }
    }
}

void free_packages(struct Packages *packages) {
    for (size_t i = 0; i < packages->len; ++i) {
        free(packages->value[i]);
    }

    free(packages->value);
    free(packages);
}
