#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "xmalloc.h"
#include "string_utils.h"

static void append_flag(struct Flags *flags, struct Flag *flag) {
    flags->value[flags->len++] = flag;

    if (flags->len >= flags->cap) {
        flags->cap += 10;
        flags->value = xrealloc(flags->value, sizeof(struct Flag*) * flags->cap);
    }
}

static void determine_flagkind(struct Flag *flag) {
    int is_boolval = flag->name[0] == '-' && (
        streql(flag->value, "<empty>") ||
        streql(flag->value, "off") ||
        streql(flag->value, "on")
    );

    if (is_boolval) {
        flag->kind = FLAG_BOOLEAN;
        return;
    }

    if (flag->name[0] != '-' && streql(flag->value, "<empty>")) {
        flag->kind = FLAG_POSITIONAL;
    } else {
        flag->kind = FLAG_COMMON; // has value
    }
}

struct Argument *parse_arguments(int argc, char **argv) {
    struct Argument *args = xmalloc(sizeof(struct Argument));

    args->action = strreserve("help");

    args->flags = xmalloc(sizeof(struct Flags));
    args->flags->cap = 5;
    args->flags->len = 0;

    if (argc == 1) {
        return args;
    }

    args->flags->value = xmalloc(sizeof(struct Flag*) * args->flags->cap);

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-' && i == 1) {
            free(args->action);
            args->action = strreserve(argv[i]);
            continue;
        }

        struct Flag *flag = xmalloc(sizeof(struct Flag));

        char *str = argv[i];
        int i = 0;

        for (char *token = strtok(str, "="); token != NULL; token = strtok(NULL, "=")) {
            if (i == 0) flag->name = strreserve(token);
            if (i == 1) flag->value = strreserve(token);
            i++;
        }

        flag->name = flag->name ? flag->name : strreserve("<empty>");
        flag->value = flag->value ? flag->value : strreserve("<empty>");

        determine_flagkind(flag);

        if (flag->kind == FLAG_POSITIONAL) {
            free(flag->value);
            flag->value = strreserve(flag->name);
        }

        if (flag->kind == FLAG_BOOLEAN && streql(flag->value, "<empty>")) {
            free(flag->value);
            flag->value = strreserve("on");
        }

        append_flag(args->flags, flag);
    }

    return args;
}

void free_argument(struct Argument *argument) {
    free(argument->action);

    if (argument->flags == NULL) {
        free(argument);
        return;
    }

    for (unsigned int i = 0; i < argument->flags->len; ++i) {
        free(argument->flags->value[i]->name);
        free(argument->flags->value[i]->value);
        free(argument->flags->value[i]);
    }

    free(argument->flags->value);
    free(argument->flags);
    free(argument);
}

const char *format_argkind(enum FlagKind kind) {
    switch (kind) {
        case FLAG_POSITIONAL:
            return "positional";
        case FLAG_COMMON:
            return "normal";
        case FLAG_BOOLEAN:
            return "boolean";
    }
}

char *debug_flagstr(struct Flag *flag) {
    size_t len = strlen(flag->name) + strlen(flag->value)
        + strlen(format_argkind(flag->kind)) + 10;

    char *result = xmalloc(len);

    sprintf(result, "=> [%s] %s",
        format_argkind(flag->kind),
        flag->name);

    if (flag->kind != FLAG_POSITIONAL)
        sprintf(result, "%s => %s", result, flag->value);

    return result;
}

void print_debugflagstr(char *flagstr) {
    puts(flagstr);
    free(flagstr);
}
