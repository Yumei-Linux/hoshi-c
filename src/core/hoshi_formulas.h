#pragma once

#include "../fs.h"

#define BUILDER_PATH FS_HOSHI_PREFIX"/hoshi-formulas/builder.sh"
#define MERGER_PATH FS_HOSHI_PREFIX"/hoshi-formulas/merger.sh"

void formulas_builder(const char *command);
void formulas_merger(const char *command);
