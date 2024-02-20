#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "logging.h"
#include "string_utils.h"

void print_message(const char* prefix, const char *color, const char* format, va_list arg_list) {
    printf("%s%s%s ", color, prefix, COLOR_RESET);
    vprintf(format, arg_list);
    printf("\n");
}

void error(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    print_message("error:", COLOR_RED, format, arg_list);
    va_end(arg_list);
    exit(EXIT_FAILURE);
}

void cmd(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    print_message("$", COLOR_MAGENTA, format, arg_list);
    va_end(arg_list);
}

void success(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    print_message("success:", COLOR_GREEN, format, arg_list);
    va_end(arg_list);
}

void info(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    print_message("info:", COLOR_BLUE, format, arg_list);
    va_end(arg_list);
}

void warning(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    print_message("warning:", COLOR_YELLOW, format, arg_list);
    va_end(arg_list);
}

bool ask(const char *format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    printf("%s[?]%s ", COLOR_GREEN, COLOR_RESET);
    vprintf(format, arg_list);

    char input[100];
    char *r = fgets(input, sizeof(input), stdin);

    if (!r) {
        perror("fgets:");
        exit(1);
    }

    // sanitisation
    input[1] = '\0';
    strlower(input);

    while (!streql(input, "y") && !streql(input, "n") && !streql(input, "\n")) {
        printf("%s[??]%s ", COLOR_RED, COLOR_RESET);
        vprintf(format, arg_list);
        bzero(&input, sizeof(input));
        char *r = fgets(input, sizeof(input), stdin);
        if (!r) {
            perror("fgets:");
            exit(1);
        }
        input[1] = '\0';
        strlower(input);
    }

    va_end(arg_list);

    return streql(input, "y") || streql(input, "\n");
}

