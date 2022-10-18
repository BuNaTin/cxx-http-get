#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

class Param {
    // interface
public:
    using Options = std::string;
    using check_t = std::function<bool(const Options &)>;

    static check_t empty();
    static check_t not_empty();
    static check_t all_true();

public:
    Param(const std::string &name, const std::string &shrt);

    bool operator==(const std::string &name) const noexcept;

    // constructors
public:
    Param() = delete;
    Param(const Param &other) = default;
    Param(Param &&other) = default;
    ~Param() = default;

    // data
public:
    std::string m_name;
    std::string m_short;
    std::string m_description;
    check_t m_check;
};

class Args {
public:
    class Name;

    class WrongArgument;
    class ErrorChecking;

    // interface
public:
    Args &&add(Param &&param) noexcept;
    Args &&add(Param &&param, const std::string &description) noexcept;
    Args &&add(Param &&param,
               const std::string &description,
               Param::check_t check) noexcept;
    Args &&addPattern(const std::string &pattern) noexcept;

    // my throw error
    void process(int argc, char *argv[]);

    bool has(const std::string &param_name) const noexcept;
    Param::Options get(const std::string &param_name) const noexcept;

    std::string defaultHelp() const noexcept;

    // data
private:
    std::vector<Param> m_data;

    using Args_t = std::pair<Param, Param::Options>;
    std::vector<Args_t> m_args;

    // for default help
    std::string m_pattern;
};

class Args::WrongArgument final : public std::runtime_error {
public:
    WrongArgument(const std::string &name) noexcept
            : runtime_error("Had wrong argument: " + name) {}

    ~WrongArgument() = default;
};

class Args::ErrorChecking final : public std::runtime_error {
public:
    ErrorChecking(const std::string &name) noexcept
            : runtime_error("Error checking argument '" + name +
                            "' params") {}

    ~ErrorChecking() = default;
};

class Args::Name {
public:
    static const std::string NAMESPACE_DEFAULT;

    static const std::string CATCH_SIGINT_COUNT;
    static const std::string CATCH_SIGINT_COUNT_SHORT;
};

