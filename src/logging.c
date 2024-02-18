#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "logging.h"

void print_message(const char* prefix, const char *color, const char* format, va_list argList) {
    printf("%s%s%s: ", color, prefix, COLOR_RESET);
    vprintf(format, argList);
    printf("\n");
}

void error(const char* format, ...) {
    va_list argList;
    va_start(argList, format);
    print_message("error", COLOR_RED, format, argList);
    va_end(argList);
    exit(EXIT_FAILURE);
}

void success(const char* format, ...) {
    va_list argList;
    va_start(argList, format);
    print_message("success", COLOR_GREEN, format, argList);
    va_end(argList);
}

void info(const char* format, ...) {
    va_list argList;
    va_start(argList, format);
    print_message("info", COLOR_BLUE, format, argList);
    va_end(argList);
}

void warning(const char* format, ...) {
    va_list argList;
    va_start(argList, format);
    print_message("warning", COLOR_RESET, format, argList);
    va_end(argList);
}
