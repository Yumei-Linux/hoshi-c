#pragma once

#include <stddef.h>

void *xmalloc(size_t len);
void *xrealloc(void *ptr, size_t len);