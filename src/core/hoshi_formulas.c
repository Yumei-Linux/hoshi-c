#include <stdlib.h>
#include <string.h>

#include "hoshi_formulas.h"

#include "../secsystem.h"
#include "../xmalloc.h"
#include "../string_utils.h"

#define DEFINE_CALL(name, path) \
    void (name)(const char *command) { \
        char *cmd = xmalloc(strsize(command) + strsize((path)) + 3); \
        strcpy(cmd, (path)); \
        strcat(cmd, " "); \
        strcat(cmd, command); \
        secsystem(cmd); \
        free(cmd); \
    }

DEFINE_CALL(formulas_builder, BUILDER_PATH);
DEFINE_CALL(formulas_merger, MERGER_PATH);

