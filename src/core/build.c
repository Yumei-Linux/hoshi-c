#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "build.h"

#include "../args.h"
#include "../xmalloc.h"
#include "../string_utils.h"
#include "../logging.h"
#include "../secsystem.h"
#include "../fs.h"

#include "../internal/builder.h"

struct Data {
    char *pkgname;
};

static struct Data *recollect_data(struct Argument *args) {
    struct Data *data = (struct Data*) xmalloc(sizeof(struct Data));

    for (unsigned int i = 0; i < args->flags->len; ++i) {
        struct Flag *flag = args->flags->value[i];

        if (flag == NULL) {
            error("%s[internal]%s cannot get a %sflag%s????",
                COLOR_YELLOW, COLOR_RESET, COLOR_RED, COLOR_RESET);
        }

        if (flag->kind == FLAG_POSITIONAL && data->pkgname == NULL) {
            data->pkgname = xmalloc(strsize(flag->value) + 1);
            strcpy(data->pkgname, flag->value);
        }
    }

    if (data->pkgname == NULL) {
        error("required positional argument: %s<pkgname>%s",
            COLOR_GREEN, COLOR_RESET);
    }

    return data;
}

static inline void free_data(struct Data *data) {
    free(data->pkgname);
    free(data);
}

void build(struct Argument *args) {
    struct Data *data = recollect_data(args);

    build_from_source(data->pkgname);

    char *built_hoshi_filename = xmalloc(10);
    char *built_hoshi_path = xmalloc(10);
    unsigned int i = 0;

    for (char *token = strtok(data->pkgname, "/"); token != NULL; token = strtok(NULL, "/"), ++i) {
        if (i == 1) {
            built_hoshi_filename = xrealloc(built_hoshi_filename, strlen(token) + strlen(".hoshi") + 1);
            strcpy(built_hoshi_filename, token);
            strcat(built_hoshi_filename, ".hoshi");
            size_t path_len = strlen(FS_HOSHI_PREFIX) + strlen("/hoshi-formulas/dist/") + strlen(token) + strlen(".hoshi") + 1;
            built_hoshi_path = xrealloc(built_hoshi_path, path_len);
            snprintf(built_hoshi_path, path_len, "%s/hoshi-formulas/dist/%s.hoshi", FS_HOSHI_PREFIX, token);
        }
    }

    silent_secsystem("bash -c 'test -d dist || mkdir dist ; mv %s ./dist'", built_hoshi_path);

    warning("Package has been saved at %s./dist/%s%s", COLOR_GREEN, built_hoshi_filename, COLOR_RESET);

    free(built_hoshi_filename);
    free(built_hoshi_path);
    free_data(data);
}
