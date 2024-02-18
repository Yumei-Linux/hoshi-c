#pragma once

#define COLOR_RED     "\033[31m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

void success(const char *format, ...);
void info(const char *format, ...);
void warning(const char *format, ...);
void error(const char *format, ...);
