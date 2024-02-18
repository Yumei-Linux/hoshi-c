#include "args.h"
#include "string_utils.h"

#include "core/help.h"
#include "core/grab.h"

int main(int argc, char *argv[]) {
    struct Argument *action = parse_arguments(argc, argv);

    if (streql(action->action, "help"))
        help(action);

    if (streql(action->action, "grab")) {
        grab(action);
    }

    free_argument(action);

    return 0;
}
