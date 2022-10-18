#include <Application/Args.h>

#define ARG_LONG_HELP "help"
#define ARG_HELP "h"

#define ARG_CONFIG_FILE "config"
#define ARG_CONFIG_FILE_SHORT "c"

void buildArgs(Args &args) {
    args.add({ARG_LONG_HELP, ARG_HELP}, "Print help message");

    args.add({ARG_CONFIG_FILE, ARG_CONFIG_FILE_SHORT},
             "Application configuration file",
             Param::not_empty());
}
