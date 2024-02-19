#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "builder.h"
#include "ui.h"

#include "../fs.h"
#include "../logging.h"
#include "../secsystem.h"

#include "../core/hoshi_formulas.h"

static void perform_downloads(const char *pkgname, const char *metadata) {
    info("Checking for pending downloads...");

    cJSON *json = cJSON_ParseWithLength(metadata, strlen(metadata));

    if (json == NULL) {
        error("cannot parse metadata for package %s%s%s", COLOR_GREEN, pkgname, COLOR_RESET);
    }

    const cJSON *downloads = cJSON_GetObjectItemCaseSensitive(json, "downloads");

    if (!cJSON_IsArray(downloads)) {
        error("Invalid downloads field in metadata for package %s%s%s", COLOR_GREEN, pkgname, COLOR_RESET);
    }

    if (cJSON_GetArraySize(downloads) == 0) {
        success("Okay");
        cJSON_Delete(json);
    }

    success("Done, performing pending downloads...\n");

    const cJSON *download;

    // attempting to keep it clean
    silent_secsystem("rm -rf %s", FS_HOSHI_PREFIX"/hoshi-formulas/downloads/*");

    cJSON_ArrayForEach(download, downloads) {
        if (cJSON_IsString(download) && (download->valuestring != NULL)) {
            secsystem("wget %s -P %s", download->valuestring, FS_HOSHI_PREFIX"/hoshi-formulas/downloads/");
        }
    }

    cJSON_Delete(json);

    success("Okay");
}

void build_from_source(const char *pkgname) {
    char *metadata_path = get_pkg_metadata(pkgname);

    if (file_exists(metadata_path) == false) {
        error("Invalid attempt to grab package `%s`: No package has been found", pkgname);
    }

    char *metadata_content = read_file(metadata_path);

    present_metadata(metadata_path, metadata_content);

    if (!ask("Would you like to build it? [Y/n] ")) {
        warning("Sure, giving up...");
        free(metadata_path);
        free(metadata_content);
        exit(0);
    }

    printf("\n");

    perform_downloads(pkgname, metadata_content);

    info("Starting build process");
    formulas_builder(pkgname);

    free(metadata_path);
    free(metadata_content);

    success("Done, package has been built successfully");
}
