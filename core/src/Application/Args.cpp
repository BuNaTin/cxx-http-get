#include <Application/Args.h> 

#include <algorithm>

namespace {

bool isArg(char *word) { return word[0] == '-'; }

} // namespace

const std::string Args::Name::NAMESPACE_DEFAULT = "DEFAULT";

const std::string Args::Name::CATCH_SIGINT_COUNT = "catch_sigint_cnt";
const std::string Args::Name::CATCH_SIGINT_COUNT_SHORT = "sig_cnt";

Param::Param(const std::string &name, const std::string &short_name)
        : m_name(name), m_short(short_name) {
    ;
}

bool Param::operator==(const std::string &name) const noexcept {
    if (("--" + m_name) == name) {
        return true;
    }
    if (("-" + m_name) == name) {
        return true;
    }
    if (m_name == name) {
        return true;
    }
    if (("--" + m_short) == name) {
        return true;
    }
    if (("-" + m_short) == name) {
        return true;
    }
    if (m_short == name) {
        return true;
    }
    return false;
}

Param::check_t Param::empty() {
    return [](auto &&params) -> bool {
        return params.empty();
    };
}
Param::check_t Param::not_empty() {
    return [](auto &&params) -> bool {
        return !params.empty();
    };
}
Param::check_t Param::all_true() {
    return [](auto &&params) -> bool {
        return true;
    };
}

Args &&Args::add(Param &&param) noexcept {
    m_data.push_back(std::move(param));
    param.m_check = Param::empty();
    return std::move(*this);
}
Args &&Args::add(Param &&param, const std::string &description) noexcept {
    param.m_description = description;
    param.m_check = Param::empty();
    m_data.push_back(std::move(param));
    return std::move(*this);
}
Args &&Args::add(Param &&param, const std::string &description, Param::check_t check) noexcept {
    param.m_description = description;
    param.m_check = check;
    m_data.push_back(std::move(param));
    return std::move(*this);
}
Args &&Args::addPattern(const std::string& pattern) noexcept {
    m_pattern = pattern;
    return std::move(*this);
}

void Args::process(int argc, char *argv[]) {
    if (argc < 1) {
        throw WrongArgument("No any arguments");
    }

    // insertion part
    for (int i = 1; i < argc; ++i) {
        if (isArg(argv[i])) {
            // add arg
            auto it = std::find_if(m_data.begin(),
                                   m_data.end(),
                                   [name = std::string(argv[i])](
                                           auto &&param) -> bool {
                                       return param == name;
                                   });
            if (it == m_data.end()) {
                throw WrongArgument(argv[i]);
            }

            m_args.push_back(
                    std::make_pair(*it, std::string()));
        } else {
            // add param to last arg
            std::size_t ind = m_args.size();
            if (ind == 0) {
                throw WrongArgument(argv[i]);
            }
            --ind;
            m_args[ind].second += argv[i];
        }
    }

    // validation part
    for (auto &&argument : m_args) {
        if (!argument.first.m_check(argument.second)) {
            throw ErrorChecking(argument.first.m_name);
        }
    }

    return;
}

bool Args::has(const std::string &name) const noexcept {
    return std::find_if(m_args.begin(),
                        m_args.end(),
                        [&name](const std::pair<Param, Param::Options>& data) -> bool {
                            return data.first == name;
                        }) != m_args.end();
}

Param::Options Args::get(const std::string &name) const noexcept {
    auto it = std::find_if(
            m_args.begin(), m_args.end(), [&name](auto &&data) -> bool {
                return (data.first == name);
            });

    if (it == m_args.end()) {
        return {};
    }

    return it->second;
}

std::string Args::defaultHelp() const noexcept {
    std::string description;
    
    if(!m_pattern.empty()) {
        description += "Example: ";
        description += m_pattern;
        description += "\n";
    }

    description += "Arguments: \n";
    for (auto &&arg : m_data) {
        description += " - ";
        description += arg.m_name;
        description += " [";
        description += arg.m_short;
        description += ']';

        description += " - ";
        description += arg.m_description;

        description += '\n';
    }
    return description;
}
