#pragma once

#include <stddef.h>

struct Packages {
    size_t cap;
    size_t len;
    char **value;
};

struct Packages *initialise_packages(void);
void push_package(struct Packages *packages, char *new_package);
void remove_package(struct Packages *packages, char *package);
void free_packages(struct Packages *packages);
