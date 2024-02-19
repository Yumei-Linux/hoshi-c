#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "grab.h"
#include "hoshi_formulas.h"

#include "../xmalloc.h"
#include "../args.h"
#include "../string_utils.h"
#include "../logging.h"
#include "../fs.h"
#include "../secsystem.h"

#include "../internal/ui.h"
#include "../internal/builder.h"
#include "../internal/merger.h"
#include "../internal/query.h"

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
        error("required positional argument: %s<pkgname>%s", COLOR_GREEN, COLOR_RESET);

    return data;
}

static inline void free_data(struct Data *data) {
    free(data->pkgname);
    free(data);
}

// TODO: Check if the package has been already installed
// (and skip if so)
static void build_and_merge(char *pkgname, bool interactive) {
    if (!interactive && is_package_installed(pkgname)) {
        warning("Skipping %s%s%s: %sPackage already installed%s", COLOR_GREEN,
            pkgname, COLOR_RESET, COLOR_MAGENTA, COLOR_RESET);

        return;
    }

    char *metadata_path = get_pkg_metadata(pkgname);

    if (!file_exists(metadata_path))
        error("Cannot stat metadata for package %s%s%s", COLOR_GREEN, pkgname, COLOR_RESET);

    char *metadata_content = read_file(metadata_path);

    if (interactive) {
        present_metadata(metadata_path, metadata_content);
        if (!ask("Would you like to built it? [Y/n] ")) {
            warning("Sure, giving up...");
            free(metadata_path);
            free(metadata_content);
            exit(0);
        }
    }

    cJSON *json = cJSON_ParseWithLength(metadata_content, strlen(metadata_content));

    if (json == NULL) {
        error("cannot read metadata for package %s%s%s",
            COLOR_GREEN, pkgname, COLOR_RESET);
    }

    const cJSON *dependencies = cJSON_GetObjectItemCaseSensitive(json, "depends");

    if (!cJSON_IsArray(dependencies)) {
        error("Invalid dependencies field for metadata of package %s%s%s",
            COLOR_GREEN, pkgname, COLOR_RESET);
    }

    const cJSON *dependency;

    // build dependencies recursively
    cJSON_ArrayForEach(dependency, dependencies) {
        if (cJSON_IsString(dependency) && (dependency->valuestring != NULL)) {
            build_and_merge((char*) dependency->valuestring, false);
        }
    }

    cJSON_Delete(json);

    info("Building package %s%s%s %s%s",
        COLOR_GREEN, pkgname, COLOR_MAGENTA,
        interactive == true ? "" : "(dependency)",
        COLOR_RESET);

    perform_build_downloads(pkgname, metadata_content);
    formulas_builder(pkgname);
    merge_by_pkgname(pkgname);

    free(metadata_path);
    free(metadata_content);
}

static inline void cleanup(void) {
    silent_secsystem("rm -rf %s/*", FS_HOSHI_PREFIX"/hoshi-formulas/dist");
}

void grab(struct Argument *arg) {
    struct Data *data = recollect_data(arg);

    if (data->is_binary == false) {
        build_and_merge(data->pkgname, true);
        cleanup();
    }
    
    free_data(data);
}
