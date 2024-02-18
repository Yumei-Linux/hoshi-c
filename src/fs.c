#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "fs.h"
#include "xmalloc.h"
#include "logging.h"

bool file_exists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    bool is_file = false;
    if (fp != NULL) {
        is_file = true;
        fclose(fp);
    }

    return is_file;
}

char *read_file(const char *filename) {
    if (!file_exists(filename)) {
        error("cannot stat file %s: No such file or directory", filename);
        exit(1);
    }

    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        perror("fopen:");
        exit(1);
    }

    size_t cap = 1024;
    char *content = xmalloc(cap);
    char line[100];

    while (fgets(line, sizeof(line), fp)) {
        size_t actual_content_len = strlen(content);

        if (actual_content_len >= cap) {
            cap += 1024;
            content = xrealloc(content, cap);
        }

        strcat(content, line);
    }

    fclose(fp);
    explicit_bzero(&line, sizeof(line));

    return content;
}

char *get_pkg_metadata(const char *pkgname) {
    size_t len = strlen(FS_HOSHI_PREFIX) + strlen("/hoshi-formulas/") + strlen(pkgname) + strlen("/metadata.json") + 1;
    char *result = xmalloc(len);
    snprintf(result, len, "%s/hoshi-formulas/%s/metadata.json", FS_HOSHI_PREFIX, pkgname);
    return result;
}
