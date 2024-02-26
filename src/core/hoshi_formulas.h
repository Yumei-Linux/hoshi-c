#pragma once

#include "../fs.h"

#define BUILDER_PATH FS_HOSHI_PREFIX"/hoshi-formulas/builder.sh"
#define REGISTER_PATH FS_HOSHI_PREFIX"/hoshi-formulas/register.sh"

void formulas_builder(const char *command);
void formulas_register(const char *dirname, const char *flag);
