#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include "scaffold.h"

#include "../args.h"
#include "../fs.h"
#include "../logging.h"
#include "../secsystem.h"
#include "../string_utils.h"

static void proceed_scaffolding(void) {
    info("Starting hoshi scaffolding...");

    secsystem("mkdir -p /var/lib/hoshi");

    secsystem("git clone https://github.com/yumei-linux/hoshi-formulas %s",
        "/var/lib/hoshi/hoshi-formulas");

    secsystem("bash -c 'mkdir -p %s{%s,%s}'",
        "/var/lib/hoshi/",
        "debug-rootfs",
        "packages");

    success("Scaffolding has been done properly");
}

void scaffold(struct Argument *args) {
    bool forced = false;

    for (unsigned int i = 0; i < args->flags->len; ++i) {
        struct Flag *flag;

        if (!(flag = args->flags->value[i])) {
            error("[internal] cannot obtain a flag??");
        }

        if ((streql(flag->name, "-f") || streql(flag->name, "--forced")) &&
                flag->kind == FLAG_BOOLEAN) {
            forced = streql(flag->value, "on");
        }
    }

    if (forced) {
        warning("removing previous base files in order to generate new ones");
        silent_secsystem("rm -rf /var/lib/hoshi");
    }

    DIR *hoshi_dir = opendir(FS_HOSHI_PREFIX);

    if (hoshi_dir) {
        closedir(hoshi_dir);
        warning("hoshi has been already scaffolded previously at %s%s%s",
            COLOR_GREEN, FS_HOSHI_PREFIX, COLOR_RESET);
    } else if (ENOENT == errno) {
        proceed_scaffolding();
    } else {
        error("cannot open hoshi cache dir for some unexpected reason...");
    }
}
