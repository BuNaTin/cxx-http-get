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

    if (!args.has(ARG_CONFIG_FILE)) {
        std::cout << "Should have " ARG_CONFIG_FILE " argument"
                  << std::endl;
        std::cerr << args.defaultHelp() << std::endl;
        return 1;
    }

    auto application = my_project::Application::Builder()
                               .setConfig(args.get(ARG_CONFIG_FILE))
                               .setArgs(args)
                               .build();
    if (!application) {
        return 2;
    }

    if (!application->start()) {
        std::cerr << "Something goes wrong during app work"
                  << std::endl;
        return 3;
    }

    std::cout << "Work done, shutdown application" << std::endl;
    return 0;
}
