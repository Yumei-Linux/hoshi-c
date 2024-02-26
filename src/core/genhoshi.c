#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

#include "genhoshi.h"
#include "hoshi_formulas.h"

#include "../args.h"
#include "../xmalloc.h"
#include "../string_utils.h"
#include "../logging.h"
#include "../secsystem.h"
#include "../fs.h"

#include "../internal/packages.h"
#include "../internal/builder.h"
#include "../internal/ui.h"

struct Data {
    struct Packages *packages;
};

static struct Data *collect_data(struct Argument *args) {
    struct Data *data = xmalloc(sizeof(struct Data));

    data->packages = initialise_packages();

    for (unsigned int i = 0; i < args->flags->len; ++i) {
        struct Flag *flag = args->flags->value[i];

        if (flag == NULL) {
            error("%s[internals]%s cannot get a %sflag%s????",
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

static inline char *get_metadata_path(const char *dirname) {
    char *metadata_path = xmalloc(strlen(dirname) + strlen("metadata.json") + 1);
    sprintf(metadata_path, "%s/metadata.json", dirname);
    return metadata_path;
}

static void show_package_data(const char *dirname) {
    char *metadata_path = get_metadata_path(dirname);

    if (!file_exists(metadata_path)) {
        error("metadata file for package %s%s%s does not exist!",
            COLOR_GREEN, dirname, COLOR_RESET);
    }

    char *metadata_content = read_file(metadata_path);

    present_metadata(metadata_path, metadata_content);

    free(metadata_path);
    free(metadata_content);
}

static inline void show_confirmation(void) {
    if (!ask("Would you like to generate the .hoshi file for these packages? [Y/n] ")) {
        warning("Sure, giving up...");
        exit(0);
    }
}

struct ParsedPackage {
    char *category;
    char *name;
    char *fullname;
};

struct ParsedPackage parse_package_path(char *input) {
    char *tokens[100];
    char *token = strtok(input, "/");
    int i = 0;

    while(token) {
        tokens[i++] = token;
        token = strtok(NULL, "/");
    }

    struct ParsedPackage pkg;

    pkg.category = tokens[i - 2];
    pkg.name = tokens[i - 1];

    pkg.fullname = xmalloc(strlen(pkg.category) + strlen(pkg.name) + 3);
    sprintf(pkg.fullname, "%s/%s", pkg.category, pkg.name);

    return pkg;
}

static void genhoshi_file(const char *dirname) {
    char buf[PATH_MAX];
    realpath(dirname, buf);

    char pkgname_buf[1024];
    strcpy(pkgname_buf, dirname);

    struct ParsedPackage package = parse_package_path(pkgname_buf);

    char *built_filename = xmalloc(strlen(FS_HOSHI_PREFIX"/hoshi-formulas/dist/") + strlen(package.name) + strlen(".hoshi") + 1);
    sprintf(built_filename, FS_HOSHI_PREFIX"/hoshi-formulas/dist/%s.hoshi", package.name);

    formulas_register(buf, "");
    build_from_source(package.fullname, false);
    formulas_register(buf, "unregister");

    silent_secsystem("bash -c 'test -d dist || mkdir -p dist' ; mv -v %s ./dist", built_filename);

    success("Package has been saved successfully at ./dist/%s.hoshi",
        package.name);

    free(package.fullname);
    free(built_filename);
}

void genhoshi(struct Argument *args) {
    struct Data *data = collect_data(args);

    for (unsigned int i = 0; i < data->packages->len; ++i) {
        show_package_data((const char*) data->packages->value[i]);
        puts("");
    }

    show_confirmation();

    for (unsigned int i = 0; i < data->packages->len; ++i) {
        genhoshi_file(data->packages->value[i]);
    }

    free_data(data);
}
