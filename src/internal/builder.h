#pragma once

#include <stdbool.h>

void perform_build_downloads(const char *pkgname, const char *metadata);

// builds a package using the formulas builder
// by a given package name.
void build_from_source(const char *pkgname, bool interactive);
