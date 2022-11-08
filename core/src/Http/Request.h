#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <random>
#include <string>
#include <tuple>

#include <types.h>
#include <utils/ClearFile.h>
#include <utils/GetFolder.h>
#include <utils/strFmt.h>

namespace {

std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

} // namespace

namespace http_get { inline namespace Http {

class Request {
public:
    Request(const std::string &data) : m_data(data) {}
    template<typename Iter>
    Request(Iter l, Iter r) : m_data(l, r) {
        if (m_data.find("Connection: close") != std::string::npos) {
            m_close_conn = true;
        }
    }
    ~Request() {
        if (m_file) {
            m_buffer.close();
            // m_buffer.open(m_buffer_filename,
            //               std::ofstream::out | std::ofstream::trunc);
            // m_buffer.close();
        }
    }

    std::string query() const {
        return m_data.substr(
                0,
                std::min(m_data.size(),
                         m_data.find(' ', m_data.find(' ') + 1)));
    }
    template<typename Iter>
    Request &append(Iter l, Iter r) {
        if (!m_file) {
            m_buffer.open(m_buffer_filename, std::ios::out);
            m_file = true;
        }
        std::copy(l, r, std::ostream_iterator<i8>{m_buffer});
        return *this;
    }
    Request *fin() {
        if (m_file) {
            m_buffer.close();
        }
        return this;
    }
    std::string body() const {
        return m_data.substr(
                std::min(m_data.size() - 4, m_data.find("\r\n\r\n")) +
                4);
    }
    std::size_t payloadSize() const {
        return std::stoull(
                m_data.substr(m_data.find("Content-Length: ") + 16));
    }
    bool copyTo(const std::string &filename) const {
        if (!m_file) {
            std::ofstream copy_to(filename,
                                  std::ios::binary | std::ios::trunc |
                                          std::ios::out);
            copy_to << this->body();
            return true;
        }

        namespace fs = std::filesystem;
        try {
            fs::path path_to(filename);
            path_to.remove_filename();
            if (!path_to.empty()) {
                fs::create_directories(path_to);
            }
            fs::rename(m_buffer_filename, filename);
        } catch (std::runtime_error &err) {
            std::cerr << "Could not move " << m_buffer_filename
                      << " to " << filename << " [" << err.what() << ']'
                      << std::endl;
            return false;
        }

        return true;
    }
    bool needContinue() const {
        return m_data.find("Expect: 100-continue") != std::string::npos;
    }
    bool needClose() const { return m_close_conn; }

private:
    const std::string m_buffer_filename = random_string(5);
    const std::string m_data;
    std::fstream m_buffer;
    bool m_file = false;
    bool m_close_conn = false;
};

}} // namespace http_get::Http