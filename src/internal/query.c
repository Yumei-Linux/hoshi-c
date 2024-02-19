#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "query.h"

#include "../xmalloc.h"
#include "../fs.h"
#include "../string_utils.h"

bool is_package_installed(char *pkgname) {
    char *pkgname_copy = strreserve(pkgname);

    char *name;
    unsigned int i = 0;

    for (char *token = strtok(pkgname_copy, "/"); token != NULL; token = strtok(NULL, "/"), ++i) {
        if (i == 1) {
            name = xmalloc(strlen(token) + 1);
            strcpy(name, token);
        }
    }

    size_t len = strlen(FS_HOSHI_PREFIX"/packages/") + strlen(name) + strlen(".hoshi") + 1;
    char possible_path[len];
    snprintf(possible_path, len, FS_HOSHI_PREFIX"/packages/%s.hoshi", name);

    free(name);
    free(pkgname_copy);

    return file_exists(possible_path);
}
