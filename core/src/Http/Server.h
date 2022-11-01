#pragma once

#include <memory>
#include <string>
#include <types.h>
#include <functional>

#include <Http/Request.h>
#include <Http/Response.h>

namespace http_get { inline namespace Http {

class Server {
public:
    using handler_t = std::function<Response(Request *)>;

    virtual bool start() noexcept = 0;
    virtual Server *addHandler(const std::string &pattern,
                               handler_t &&handler) noexcept = 0;

    // constructors
public:
    struct Settings {
        std::string address;
        u16 port;
    };

    static std::unique_ptr<Server> create(const Settings &settings,
                                          std::atomic_flag &m_stop);

    virtual ~Server() {}
};

}} // namespace http_get::Http