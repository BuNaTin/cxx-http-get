#pragma once

#include <iostream>
#include <string>
#include <types.h>
#include <utils/FileSize.h>
#include <utils/strFmt.h>

namespace http_get { inline namespace Http {

class Response {
public:
    Response &code(u32 code, const std::string &str_code) {
        m_code = code;
        m_str_code = str_code;
        return *this;
    }
    template<typename Iter>
    Response &data(Iter first, Iter last) {
        m_data = std::string(first, last);
        return *this;
    }
    Response &data(const std::string &data) {
        m_data = data;
        return *this;
    }
    Response &fileData(const std::string &name) {
        m_read_from = name;
        return *this;
    }
    Response &connection(const std::string &type) {
        m_connection_type = type;
        return *this;
    }
    Response &content(const std::string &type) {
        m_content_type = type;
        return *this;
    }

    bool needContinue() const { return !m_read_from.empty(); }

    std::string readFrom() const { return m_read_from; }

    std::string create() const {
        if (!m_read_from.empty()) {
            std::cout << "Send file [" << m_read_from << "] size "
                      << utils::FileSize(m_read_from.c_str()) << '\n';
            return utils::strFmt(m_template,
                                 m_code,
                                 m_str_code,
                                 m_connection_type,
                                 m_content_type,
                                 utils::FileSize(m_read_from.c_str()));
        }
        return utils::strFmt(m_template,
                             m_code,
                             m_str_code,
                             m_connection_type,
                             m_content_type,
                             m_data.size()) +
               m_data;
    }

private:
    const std::string m_template =
            "HTTP/1.1 % %\r\n"
            "Server: audi\r\n"
            "Connection:%\r\n"   // keep-alive | close
            "Content-type:%\r\n" // multipart/form-data
                                 // | text/plain
            "Content-Length: %\r\n\r\n";
    std::string m_read_from;
    std::string m_data;
    std::string m_connection_type = "close";
    std::string m_str_code = "OK";
    std::string m_content_type = "application/octet-stream";
    u32 m_code = 200;
    b32 m_expected_continue = false;
};

}} // namespace http_get::Http
