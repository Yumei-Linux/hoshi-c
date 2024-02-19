#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "secsystem.h"
#include "logging.h"

void secsystem(const char *format, ...) {
    char command[1024];

    va_list arg_list;
    va_start(arg_list, format);
    vsprintf(command, format, arg_list);
    va_end(arg_list);

    cmd("%s", command);

    if (system(command) != 0) {
        error("cannot complete command, assertion: `statuscode == 0` failed");
    }
}

void silent_secsystem(const char *format, ...) {
    char command[4906];

    va_list arg_list;
    va_start(arg_list, format);
    vsprintf(command, format, arg_list);
    va_end(arg_list);

    if (system(command) != 0) {
        error("cannot complete command, assertion: `statuscode == 0` failed");
    }
}
