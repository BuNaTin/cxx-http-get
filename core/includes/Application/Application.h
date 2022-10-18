#pragma once

#include <atomic>
#include <memory>
#include <string>

#include <types.h>

namespace http_get {

class Application {
    // interface
public:
    virtual bool start() noexcept = 0;

    // constructors
public:
    class Builder;

    virtual ~Application() {}
};

class Application::Builder {
public:
    Builder &setPort(u16 port);
    Builder &setFolderName(const std::string &folder);
    Builder &setSigint(u32 sigint);
    Builder &setBufferSizeKb(u32 size);

    std::unique_ptr<Application> build();

private:
    std::string m_folder = "./";
    u32 m_sig_max = 3;
    u16 m_port = 8080;
};

} // namespace http_get
