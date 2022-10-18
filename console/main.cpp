#include <Application/Application.h>

#include <iostream>

#include <Arguments.h>
bool processArgs(Args &args, int argc, char *argv[]) noexcept {
    try {
        buildArgs(args);
        args.process(argc, argv);
    } catch (Args::WrongArgument &err) {
        std::cerr << err.what() << std::endl;
        return false;
    } catch (Args::ErrorChecking &err) {
        std::cerr << err.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unhandeled exception while parsing args"
                  << std::endl;
        return false;
    }

    return true;
}

http_get::Application::Builder createFromArgs(const Args &args) {
    http_get::Application::Builder builder;
    if (args.has(ARG_PORT)) {
        builder.setPort(std::stoi(args.get(ARG_PORT)));
    }
    if (args.has(ARG_FOLDER_NAME)) {
        builder.setFolderName(args.get(ARG_FOLDER_NAME));
    }
    if (args.has(ARG_SIGINT)) {
        builder.setSigint(std::stoi(args.get(ARG_SIGINT)));
    }
    return builder;
}

int main(int argc, char *argv[]) {
    Args args;
    if (!processArgs(args, argc, argv)) {
        std::cerr << args.defaultHelp() << std::endl;
        return 1;
    }

    if (args.has(ARG_HELP)) {
        std::cout << args.defaultHelp() << std::endl;
        return 0;
    }

    auto server = createFromArgs(args).build();

    if (!server) {
        std::cerr << "Failed create server, exit" << std::endl;
        return 2;
    }

    if (!server->start()) {
        std::cerr << "Something goes wrong during server work"
                  << std::endl;
        return 3;
    }

    std::cout << "Work done, shutdown server" << std::endl;
    return 0;
}
