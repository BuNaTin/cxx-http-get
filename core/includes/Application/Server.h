#pragma once

#include <atomic>
#include <memory>
#include <string>

#include <types.h>

namespace http_get {

class Server {
    // interface
public:
    virtual bool start() noexcept = 0;

    // constructors
public:
    class Builder;

    virtual ~Server() {}
};

class Server::Builder {
public:
    Builder &setPort(u16 port);
    Builder &setFolderName(const std::string &folder);
    Builder &setSigint(u32 sigint);
    Builder &setBufferSizeKb(u32 size);

    std::unique_ptr<Server> build();

private:
    std::string m_folder = "./";
    u32 m_sig_max = 3;
    u32 m_kb_buffer_size = 32; // 32Kb
    u16 m_port = 8080;
};

} // namespace http_get
