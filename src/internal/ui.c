#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "ui.h"

#include "../logging.h"

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

    cJSON_Delete(json);
}
