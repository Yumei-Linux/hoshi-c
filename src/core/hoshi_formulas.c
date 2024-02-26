#include <stdio.h>
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

void formulas_register(const char *dirname, const char *flag) {
    char *cmd = xmalloc(strlen(REGISTER_PATH) + strlen(dirname) + strlen(flag) + 4);
    sprintf(cmd, "%s %s %s", REGISTER_PATH, dirname, flag);
    secsystem(cmd);
    free(cmd);
}

