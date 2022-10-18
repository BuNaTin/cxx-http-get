#include "Config/IniConf.h"

namespace my_project {
    // INICONF
    IniConf::IniConf(const std::string &filename) {
        std::ifstream in(filename);
        if (!in) {
            m_valid = false;
            return;
        }
        m_name = filename;
        std::string cur_section_name = "GLOBAL";
        std::string buffer;
        std::string accum;
        
        while (std::getline(in, buffer)) {
            if (buffer.empty()) {
                continue;
            }
            if (buffer[buffer.size() - 1] == '\\') {
                accum += buffer;
                accum.pop_back();
                // std::cout << "Nice line {" << accum << "}\n";
                continue;
            }
            buffer = accum + buffer;
            accum = "";
            
            auto str = splitAndClear(buffer, ';');
            if (str.first.empty()) {
                continue;
            }
            if (isSection(str.first)) {
                cur_section_name = getSection(str.first);
                continue;
            }
            m_data[cur_section_name].insert(splitAndClear(str.first, '='));
        }
        
        m_valid = true;
    }
    IniConf::operator bool() const { return m_valid; }
    
    std::string IniConf::getName() const noexcept { return m_name; }
    
    const IniConf::Section &
        IniConf::operator()(const std::string &name) const {
        auto it = m_data.find(name);
        if (it == m_data.end()) {
            return m_empty;
        }
        return it->second;
    }
    bool IniConf::has(const std::string &name) const {
        return (m_data.find(name) != m_data.end());
    }
    
    bool IniConf::isSection(const std::string &str) const {
        if (str.size() < 3) return false;
        return (str[0] == '[' && str[str.size() - 1] == ']');
    }
    std::string IniConf::getSection(const std::string &str) const {
        if (str.size() < 3) return {};
        return str.substr(1, str.size() - 2);
    }
    std::vector<std::string> IniConf::sections() const {
        std::vector<std::string> ans;
        for (auto &&pair : m_data) {
            if (pair.first != "global") {
                ans.push_back(pair.first);
            }
        }
        return ans;
    }
    
    // VARIABLE
    IniConf::Variable::Variable(const std::string &data,
                                Variable::TYPE type)
        : m_data(data), m_type(type) {}
    IniConf::Variable::operator std::string() const { return m_data; }
    
    // SECTION
    const IniConf::Variable &
        IniConf::Section::operator()(const std::string &name) const {
        auto it = m_data.find(name);
        if (it == m_data.end()) {
            return m_empty;
        }
        return it->second;
    }
    bool IniConf::Section::has(const std::string &name) const {
        return (m_data.find(name) != m_data.end());
    }
    
    void IniConf::Section::insert(
                                  const std::pair<std::string, std::string> &data) {
        m_data[data.first] = create(data.second);
    }
    IniConf::Variable IniConf::Section::create(const std::string &data) {
        if (data.find(',') != std::string::npos) {
            return Variable(data, Variable::TYPE::ARRAY);
        }
        return Variable(data, Variable::TYPE::VALUE);
    }
} // namespace Mib2Core
