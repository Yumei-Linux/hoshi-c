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

static void present_metadata(const char *path, const char *metadata) {
    cJSON *json = cJSON_ParseWithLength(metadata, strlen(metadata));

    if (json == NULL) {
        error("Unable to parse metadata at %s", path);
    }

    const cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    const cJSON *description = cJSON_GetObjectItemCaseSensitive(json, "description");

    if (!cJSON_IsString(name) || (name->valuestring == NULL)) {
        error("invalid name found in metadata for json at %s", path);
    }

    if (!cJSON_IsString(description) || (description->valuestring == NULL)) {
        error("invalid description found in metadata for json at %s", path);
    }

    printf("%sShowing package information:%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s* %sName:%s %s\n", COLOR_GREEN, COLOR_YELLOW, COLOR_RESET, name->valuestring);
    printf("%s* %sDescription:%s %s\n", COLOR_GREEN, COLOR_YELLOW, COLOR_RESET, description->valuestring);

    if (ask("Are you up to build & merge it right now? [Y/n] ") == false) {
        info("sure, giving up...");
        cJSON_Delete(json);
        exit(0);
    }

    printf("\n");

    cJSON_Delete(json);
}

static void perform_downloads(const char *pkgname, const char *metadata) {
    info("Checking for pending downloads...");
    cJSON *json = cJSON_ParseWithLength(metadata, strlen(metadata));

    const cJSON *downloads = cJSON_GetObjectItemCaseSensitive(json, "downloads");

    if (!cJSON_IsArray(downloads)) {
        error("invalid downloads field in the metadata for the package %s%s%s", COLOR_RED, pkgname, COLOR_RESET);
    }

    if (cJSON_GetArraySize(downloads) == 0) {
        success("Okay");
        cJSON_Delete(json);
    }

    success("Done, performing pending downloads\n");

    const cJSON *download;

    // attempting to keep it clean every time.
    secsystem("rm -rf %s", FS_HOSHI_PREFIX"/hoshi-formulas/downloads/*");

    cJSON_ArrayForEach(download, downloads) {
        if (cJSON_IsString(download) && (download->valuestring != NULL)) {
            secsystem("wget %s -P %s", download->valuestring, FS_HOSHI_PREFIX"/hoshi-formulas/downloads/");
        }
    }

    cJSON_Delete(json);

    success("Okay");
}

static void build_package(const char *pkgname) {
    info("Starting build process");
    formulas_builder(pkgname);
}

static void merge_package(char *pkgname) {
    char *hoshi_filename;
    int i = 0;

    for (char *token = strtok(pkgname, "/"); token != NULL; token = strtok(NULL, "/"), ++i) {
        if (i == 1) {
            size_t len = strlen(token) + strlen(".hoshi") + 1;
            hoshi_filename = xmalloc(len);
            snprintf(hoshi_filename, len, "%s.hoshi", token);
        }
    }

    if (hoshi_filename == NULL) {
        error("cannot get .hoshi filename? (allocation problem maybe?)");
    }

    // perform actual merging
    const char *prefix = FS_HOSHI_PREFIX"/hoshi-formulas/dist";
    char *formulas_command = xmalloc(strlen(prefix) + strlen(hoshi_filename) + 1);
    sprintf(formulas_command, "%s/%s", prefix, hoshi_filename);
    formulas_merger(formulas_command);
    free(formulas_command);
}

static void build_from_source(const char *pkgname) {
    char *metadata_path = get_pkg_metadata(pkgname);

    if (file_exists(metadata_path) == false) {
        error("Invalid attempt to grab package `%s`: No package has been found by using that query", pkgname);
    }

    char *metadata_content = read_file(metadata_path);

    present_metadata(metadata_path, metadata_content);

    perform_downloads(pkgname, metadata_content);
    build_package(pkgname);
    merge_package((char*) pkgname);

    success("Done, package has been installed successfully");

    free(metadata_path);
    free(metadata_content);
}

void grab(struct Argument *arg) {
    struct Data *data = recollect_data(arg);

    if (data->is_binary == false) {
        build_from_source(data->pkgname);
    }
    
    free_data(data);
}
