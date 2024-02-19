#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "grab.h"

#include "../xmalloc.h"
#include "../args.h"
#include "../string_utils.h"
#include "../logging.h"
#include "../fs.h"
#include "../secsystem.h"
#include "../internal/merger.h"
#include "../internal/builder.h"

struct Data {
    char *pkgname;
    bool is_binary;
};

static struct Data *recollect_data(struct Argument *arg) {
    struct Data *data = xmalloc(sizeof(struct Data));

    for (unsigned int i = 0; i < arg->flags->len; ++i) {
        struct Flag *flag = arg->flags->value[i];

        if (flag == NULL)
            error("%s[internal]%s cannot get a %sflag%s????", COLOR_YELLOW, COLOR_RESET, COLOR_BLUE, COLOR_RESET);

        if (flag->kind == FLAG_POSITIONAL && data->pkgname == NULL) {
            data->pkgname = xmalloc(strsize(flag->value) + 1);
            strcpy(data->pkgname, flag->value);
        }

        int is_flag_binary = (
            streql(flag->name, "-b") ||
            streql(flag->name, "--bin")
        );

        if (flag->kind == FLAG_BOOLEAN && is_flag_binary) {
            data->is_binary = (bool) streql(flag->value, "on");
        }
    }

    if (data->pkgname == NULL)
        error("required positional argument: %s<pkgname>%s", COLOR_BLUE, COLOR_RESET);

    return data;
}

static inline void free_data(struct Data *data) {
    free(data->pkgname);
    free(data);
}

void grab(struct Argument *arg) {
    struct Data *data = recollect_data(arg);

    if (data->is_binary == false) {
        build_from_source(data->pkgname);
        merge_by_pkgname((char*) data->pkgname);

        // keeping dist files clean, note that this shouldn't
        // be performed when doing a simple build, this time we're
        // doing both build & merge, so no need to keep these
        silent_secsystem("rm -rf %s/*", FS_HOSHI_PREFIX"/hoshi-formulas/dist");
    }
    
    free_data(data);
}
