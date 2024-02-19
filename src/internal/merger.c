#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "merger.h"

#include "../xmalloc.h"
#include "../logging.h"
#include "../secsystem.h"
#include "../config.h"

static void install(const char *file, const char *dest) {
    size_t len = strlen(ROOTFS_PATH) + strlen(dest) + 2;
    char buf[len];
    snprintf(buf, len, "%s/%s", ROOTFS_PATH, dest);
    printf("%s  ++%s %s -> %s\n", COLOR_GREEN, COLOR_RESET, file, buf);
    silent_secsystem("bash -c 'install -Dm755 %s %s >/dev/null 2>&1 || true'", file, buf);
}

static void install_from_files(const char *pathname) {
    size_t len = strlen(pathname) + strlen("files.txt") + 2;
    char buf[len];
    snprintf(buf, len, "%s/%s", pathname, "files.txt");

    FILE *fp = fopen(buf, "r");

    if (fp == NULL) {
        perror("fopen:");
        exit(1);
    }

    char line[1024];

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '\n') continue;
        line[strcspn(line, "\n")] = '\0';
        len = strlen(pathname) + strlen(line) + 2;
        char filebuf[len];
        snprintf(filebuf, len, "%s/%s", pathname, line);
        install(filebuf, line);
    }

    fclose(fp);
}

void merge_hoshi_bundle(const char *filename) {
    char *extracted_path = xmalloc(strlen(filename) + 1);
    strcpy(extracted_path, filename);

    // remove extension
    extracted_path[strcspn(extracted_path, ".")] = '\0';

    info("registering %s as installed", filename);
    secsystem("cp %s %s", filename, FS_HOSHI_PREFIX"/packages");
    info("reading %s", filename);
    mkdir(extracted_path, 0777);
    secsystem("tar xpf %s --strip-components=1 -C %s", filename, extracted_path);
    info("merging files into `%s`", ROOTFS_PATH);
    install_from_files(extracted_path);
    info("okay, cleaning up");
    secsystem("rm -rf %s", extracted_path);
    free(extracted_path);
    success("Okay, the package has been merged successfully");
}

