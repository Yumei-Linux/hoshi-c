#pragma once

#include <stdbool.h>

#define FS_HOSHI_PREFIX "/var/lib/hoshi"

bool file_exists(const char *filename);
char *read_file(const char *filename);
char *get_pkg_metadata(const char *pkgname);
