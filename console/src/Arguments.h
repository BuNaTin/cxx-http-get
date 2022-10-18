#include <Application/Args.h>

#define ARG_HELP "help"
#define ARG_HELP_SHORT "h"

#define ARG_PORT "port"
#define ARG_PORT_SHORT "p"

#define ARG_FOLDER_NAME "folder"
#define ARG_FOLDER_NAME_SHORT "f"

#define ARG_SIGINT "sigint_cnt"
#define ARG_SIGINT_SHORT "sig"

#define ARG_BUFFER "buffer"
#define ARG_BUFFER_SHORT "b"

void buildArgs(Args &args) {
    args.addPattern("./http_get -p 9090 -f / - run server shared "
                    "folder '/' at 9090 port");

    args.add({ARG_HELP, ARG_HELP_SHORT}, "Print help message");

    args.add({ARG_PORT, ARG_PORT_SHORT}, "Bind port, default is 8080");

    args.add({ARG_FOLDER_NAME, ARG_FOLDER_NAME_SHORT},
             "Sharing folder, default is ./",
             Param::not_empty());

    auto is_int = [](const std::string &option) -> bool {
        try {
            std::ignore = std::stoi(option);
        } catch (...) {
            return false;
        }
        return true;
    };

    args.add({ARG_SIGINT, ARG_SIGINT_SHORT},
             "Sigint handling count before abort",
             is_int);
}
