#pragma once

#include <stdbool.h>

#define COLOR_RED     "\033[31m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

void success(const char *format, ...);
void info(const char *format, ...);
void cmd(const char *format, ...);
void warning(const char *format, ...);
void error(const char *format, ...);

bool ask(const char *format, ...);
