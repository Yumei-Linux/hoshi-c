#pragma once

#include <stddef.h>

enum FlagKind {
    FLAG_POSITIONAL,
    FLAG_BOOLEAN,
    FLAG_COMMON
};

struct Flag {
    char *name;
    char *value;
    enum FlagKind kind;
};

struct Flags {
    unsigned int cap;
    unsigned int len;
    struct Flag **value;
};

struct Argument {
    char *action;
    struct Flags *flags;
};

struct Argument *parse_arguments(int argc, char **argv);
const char *format_argkind(enum FlagKind kind);
char *debug_flagstr(struct Flag *flag);
void print_debugflagstr(char *flagstr);
void free_argument(struct Argument *argument);
