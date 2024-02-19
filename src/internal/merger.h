#pragma once

void merge_hoshi_bundle(const char *filepath);

// merges the package by using the package id/name as
// reference, e.g: misc/cutefetch
// Note: this requires to be called after a build package
// process, such as build_from_source(const char *pkgname);
// > refer to `builder.h`
void merge_by_pkgname(char *pkgname);
