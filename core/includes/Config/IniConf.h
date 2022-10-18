#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::pair<std::string, std::string>
splitAndClear(const std::string &str, char sep) {
    if (str.empty()) {
        return {};
    }
    std::string::size_type sep_pos = str.find(sep);
    sep_pos = std::min(sep_pos, str.size());
    std::string first = str.substr(0, sep_pos);
    while (first.size() > 1 && first[first.size() - 1] == ' ')
        first.pop_back();

    for (; sep_pos < str.size() && str[sep_pos + 1] == ' '; ++sep_pos)
        ;
    if (sep_pos == str.size()) --sep_pos;
    return std::make_pair(first, str.substr(sep_pos + 1));
}

std::vector<std::string> splitAndClearVec(const std::string &str,
                                          char sep) {
    if (str.empty()) {
        return {};
    }
    std::vector<std::string> ans;
    std::size_t prev = 0;
    for (auto i = str.find(sep); i != std::string::npos;
         prev = i + 1, i = str.find(sep, prev)) {
        ans.emplace_back(str.substr(prev, i - prev));
    }
    if (prev != std::string::npos) {
        ans.emplace_back(str.substr(prev));
    }
    // clear
    for (auto &str : ans) {
        while (str.size() > 1 && str[0] == ' ') {
            str = str.substr(1);
        }
        while (str.size() > 1 && str[str.size() - 1] == ' ') {
            str.pop_back();
        }
    }
    return ans;
}
template<typename T,
         std::enable_if_t<std::is_constructible<std::string, T>::value,
                          bool> = true>
T cast_to(const std::string &str) {
    return T(str);
}
template<typename T,
         std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
T cast_to(const std::string &str) {
    if (str.empty()) return {};
    if (str[0] < '0' || '9' < str[0]) {
        return {};
    }
    return std::stold(str);
}
template<typename T,
         std::enable_if_t<std::is_same<T, bool>::value, bool> = true>
T cast_to(const std::string &str) {
    if (str == "1" || str == "true" || str == "TRUE" || str == "on" ||
        str == "ON")
        return true;
    return false;
}


template<typename T>
std::vector<T> cast_to_vec(const std::string &data) {
    auto strings = splitAndClearVec(data, ',');
    std::vector<T> ans;
    for (auto &&str : strings) {
        ans.push_back(cast_to<T>(str));
    }
    return ans;
}

} // namespace

namespace my_project {

/**
 * @brief Parsing .ini configuration files
 * 
 * Has some template magic
 * 
 * using like:
 * \code
 * IniConf config("file.ini");
 *
 * std::string scope_data = config("SCOPE")("data");
 * 
 * // can cast
 * int var1 = config("GLOBAL")("var1").as<int>();
 * 
 * \endcode 
 * 
 */
class IniConf {
    class Variable;
    class Section;
    // interface:
public:
    /**
     * @brief Read & parse .ini file
     * 
     */
    IniConf(const std::string &filename);


    /**
     * @brief return name of config file
     */
    std::string getName() const noexcept;
    /**
     * @brief bool cast to check if data is nice
     * 
     * @return true 
     * @return false 
     */
    operator bool() const;

    /**
     * @brief get Section to get variable from it
     * 
     * @param name - section name
     * @return const Section& 
     */
    const Section &operator()(const std::string &name) const;

    /**
     * @brief Check if section consists
     * 
     * @param section_name 
     * @return true 
     * @return false 
     */
    bool has(const std::string &section_name) const;

    /**
     * @brief Get sections names (all without "GLOBAL")
     * 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> sections() const;

    // in-class
private:
    class Variable {
    public:
        enum class TYPE {
            NON,
            VALUE,
            ARRAY,
        };

        // interface
    public:
        operator std::string() const;

        template<typename T>
        T as() const {
            if (m_type != TYPE::VALUE) {
                return T();
            }
            return cast_to<T>(m_data);
        }
        template<typename T,
                 std::enable_if_t<std::is_array<T>::value, bool> = true>
        std::vector<std::remove_pointer_t<std::decay_t<T>>> as() const {
            if (m_type != TYPE::ARRAY) {
                return {};
            }
            return cast_to_vec<std::remove_pointer_t<std::decay_t<T>>>(
                    m_data);
        }

        // constructors
    public:
        Variable() = default;
        Variable(const std::string &data, Variable::TYPE type);

        // data
    private:
        std::string m_data;
        TYPE m_type = TYPE::NON;
    };
    class Section {
        // interface
    public:
        const Variable &operator()(const std::string &name) const;
        bool has(const std::string &name) const;

        void insert(const std::pair<std::string, std::string> &data);

        // in-class interface
    private:
        Variable create(const std::string &data);

        // data
    private:
        std::unordered_map<std::string, Variable> m_data;
        Variable m_empty;
    };

    bool isSection(const std::string &str) const;
    std::string getSection(const std::string &str) const;

    // data
private:
    std::unordered_map<std::string, Section> m_data;
    Section m_empty;
    std::string m_name;
    bool m_valid;
};

} // namespace Mib2Core