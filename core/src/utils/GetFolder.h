#pragma once

#include <string>

namespace utils {

inline std::string GetFolder(const std::string &filename) {
    return filename.substr(
            0, std::min(filename.size(), filename.find_last_of('/')));
}

} // namespace utils