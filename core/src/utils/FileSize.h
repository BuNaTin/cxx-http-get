#pragma once

#include <fstream>

namespace utils {

inline std::ifstream::pos_type FileSize(const char *filename) {
    std::ifstream in(filename,
                     std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

} // namespace utils