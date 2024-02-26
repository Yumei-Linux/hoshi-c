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
#include "../internal/ui.h"
#include "../internal/packages.h"

struct Data {
    struct Packages *packages;
};

static struct Data *collect_data(struct Argument *args) {
    struct Data *data = (struct Data*) xmalloc(sizeof(struct Data));

    data->packages = initialise_packages();

    for (unsigned int i = 0; i < args->flags->len; ++i) {
        struct Flag *flag = args->flags->value[i];

        if (flag == NULL) {
            error("%s[internal]%s cannot get a %sflag%s????",
                COLOR_YELLOW, COLOR_RESET, COLOR_RED, COLOR_RESET);
        }

        if (flag->kind == FLAG_POSITIONAL) {
            push_package(data->packages, strreserve(flag->value));
        }
    }

    if (data->packages->len == 0) {
        error("required positional argument: %s<pkgname>%s",
            COLOR_GREEN, COLOR_RESET);
    }

    return data;
}

static inline void free_data(struct Data *data) {
    free_packages(data->packages);
    free(data);
}

static inline void present_metadatas(struct Packages *packages) {
    info("Showing information about the packages to build\n");

    for (unsigned int i = 0; i < packages->len; ++i) {
        present_pkg_metadata(packages->value[i]);
        puts("");
    }

    if (!ask("Would you like to build these packages? [Y/n] ")) {
        warning("Sure, giving up...");
        exit(0);
    }
}

static void build_single_package(char *packagename, bool interactive) {
    char *original_packagename = strreserve(packagename);

    build_from_source(original_packagename, interactive);

    char *built_hoshi_filename = xmalloc(10);
    char *built_hoshi_path = xmalloc(10);

    unsigned int i = 0;

    for (char *token = strtok(packagename, "/"); token != NULL; token = strtok(NULL, "/"), ++i) {
        if (i == 1) {
            built_hoshi_filename = xrealloc(built_hoshi_filename, strlen(token) + strlen(".hoshi") + 1);
            strcpy(built_hoshi_filename, token);
            strcat(built_hoshi_filename, ".hoshi");
            size_t path_len = strlen(FS_HOSHI_PREFIX) + strlen("/hoshi-formulas/dist/") + strlen(token) + strlen(".hoshi") + 1;
            built_hoshi_path = xrealloc(built_hoshi_path, path_len);
            snprintf(built_hoshi_path, path_len, "%s/hoshi-formulas/dist/%s.hoshi", FS_HOSHI_PREFIX, token);
        }
    }

    silent_secsystem("bash -c 'test -d dist || mkdir dist ; mv %s ./dist'",
        built_hoshi_path);

    warning("Package %s%s%s has been saved at %s./dist/%s%s",
        COLOR_GREEN, original_packagename, COLOR_RESET,
        COLOR_GREEN, built_hoshi_filename, COLOR_RESET);

    free(built_hoshi_filename);
    free(built_hoshi_path);
    free(original_packagename);
}

void build(struct Argument *args) {
    struct Data *data = collect_data(args);

    // build single package interactively
    if (data->packages->len == 1) {
        build_single_package(data->packages->value[0], true);
    }

    // build multiple packages interactively
    if (data->packages->len > 1) {
        present_metadatas(data->packages);

        for (unsigned int i = 0; i < data->packages->len; ++i) {
            build_single_package(data->packages->value[i], false);
        }
    }

    // free the data as how it should be always ;)
    free_data(data);
}
