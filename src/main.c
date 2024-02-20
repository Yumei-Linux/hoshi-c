#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "args.h"
#include "string_utils.h"
#include "logging.h"

#include "core/help.h"
#include "core/grab.h"
#include "core/build.h"
#include "core/scaffold.h"

static inline bool am_i_root(void) {
    return geteuid() == 0;
}

int main(int argc, char *argv[]) {
    struct Argument *action = parse_arguments(argc, argv);

    if (streql(action->action, "help"))
        help(action);

    if (!am_i_root() && !streql(action->action, "help"))
        error("please, note that %shoshi%s requires elevated privileges to work!", COLOR_GREEN, COLOR_RESET);

    if (streql(action->action, "grab"))
        grab(action);

    if (streql(action->action, "build"))
        build(action);

    if (streql(action->action, "scaffold"))
        scaffold(action);

    free_argument(action);

    return 0;
}
