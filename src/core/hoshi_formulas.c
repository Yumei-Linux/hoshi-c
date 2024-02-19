#include <stdlib.h>
#include <string.h>

#include "hoshi_formulas.h"

#include "../secsystem.h"
#include "../xmalloc.h"
#include "../string_utils.h"

void formulas_builder(const char *command) {
    char *cmd = xmalloc(strsize(command) + strsize(BUILDER_PATH) + 3);
    strcpy(cmd, BUILDER_PATH);
    strcat(cmd, " ");
    strcat(cmd, command);
    secsystem(cmd);
    free(cmd);
}

