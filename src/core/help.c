#include <stdio.h>

#include "help.h"
#include "../args.h"

void help(struct Argument *argument) {
    puts("usage: hoshi help/grab/build/scaffold");
    puts("usage: hoshi grab <pkgname> [--bin=on|off,-b=on|off]");
    puts("usage: hoshi build <pkgname>");
    puts("usage: hoshi scaffold [-f=on|off,--forced=on|off]");
}
