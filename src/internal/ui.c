#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "ui.h"

#include "../fs.h"
#include "../logging.h"
#include "../xmalloc.h"

void present_metadata(const char *path, const char *metadata) {
    cJSON *json = cJSON_ParseWithLength(metadata, strlen(metadata));

    if (json == NULL) {
        error("Unable to parse metadata for '%s'", path);
    }

    const cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    const cJSON *description = cJSON_GetObjectItemCaseSensitive(json, "description");

    if (!cJSON_IsString(name) || (name->valuestring == NULL)) {
        error("Invalid `name` found in metadata at '%s'", path);
    }

    if (!cJSON_IsString(description) || (description->valuestring == NULL)) {
        error("Invalid `description` found in metadata at '%s'", path);
    }

    printf("%sShowing package information:%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s* %sName:%s %s\n", COLOR_GREEN, COLOR_YELLOW, COLOR_RESET, name->valuestring);
    printf("%s* %sDescription:%s %s\n", COLOR_GREEN, COLOR_YELLOW, COLOR_RESET, description->valuestring);

    const cJSON *dependencies = cJSON_GetObjectItemCaseSensitive(json, "depends");

    if (!cJSON_IsArray(dependencies)) {
        error("Invalid `dependencies` field found at metadata (%s%s%s)",
            COLOR_GREEN, path, COLOR_RESET);
    }

    if (cJSON_GetArraySize(dependencies) == 0) {
        cJSON_Delete(json);
        return;
    }

    const cJSON *dependency;

    size_t dependencies_list_cap = 10;
    size_t dependencies_list_length = 0;
    char **dependencies_list = xmalloc(sizeof(char*) * dependencies_list_cap);

    cJSON_ArrayForEach(dependency, dependencies) {
        if (cJSON_IsString(dependency) && (dependency->valuestring != NULL)) {
            dependencies_list[dependencies_list_length] = xmalloc(strlen(dependency->valuestring) + 1);
            strcpy(dependencies_list[dependencies_list_length++], dependency->valuestring);
            if (dependencies_list_length >= dependencies_list_cap) {
                dependencies_list_cap += 10;
                dependencies_list = xrealloc(dependencies_list, sizeof(char*) * dependencies_list_cap);
            }
        }
    }

    printf("%s* %sDependencies:%s\n", COLOR_GREEN, COLOR_YELLOW, COLOR_RESET);

    for (size_t i = 0; i < dependencies_list_length; ++i) {
        printf("  %s->%s %s\n", COLOR_MAGENTA, COLOR_RESET, dependencies_list[i]);
    }

    cJSON_Delete(json);
}

void present_pkg_metadata(const char *packagename) {
    char *path = get_pkg_metadata(packagename);
    char *content = read_file(path);

    present_metadata(path, content);

    free(path);
    free(content);
}
