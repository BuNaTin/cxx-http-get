#pragma once 

#include <fstream>
#include <string>

namespace utils { inline namespace file {

inline void clear(const std::string& filename) {
    std::ofstream ofs;
    ofs.open("test.txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

}}