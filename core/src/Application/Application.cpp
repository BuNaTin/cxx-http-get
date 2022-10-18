#include <Application/Application.h>

#include <functional>
#include <future>
#include <numeric>
#include <signal.h>

#include <Http/Server.h>

#include <iostream>

namespace {

void workWithClient(i32 fd) {
    const u64 size = 32 * 1024;
    u8 buffer[size] = {0};
}

std::string getPath(const std::string &file) {
    std::size_t pos = file.find_last_of('/') + 1;
    return file.substr(0, std::min(pos, file.size()));
}

std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }
} // namespace

namespace http_get {

class ApplicationImpl final : public Application {
    // interface
public:
    bool start() noexcept override;

private:
    void hadSigint() noexcept {
        ++m_sig_cnt;
        if (m_sig_cnt > m_sig_max) {
            std::terminate();
        }
        m_sig_handler.clear(std::memory_order_release);
    }

    // constructors
public:
    ApplicationImpl(const u16 port,
                    const std::string &shared_folder,
                    const u32 sig_max);
    virtual ~ApplicationImpl() = default;

    // data
private:
    std::unique_ptr<Server> m_server;
    std::atomic_flag m_sig_handler = ATOMIC_FLAG_INIT;
    u32 m_sig_cnt = 0;
    u32 m_sig_max = std::numeric_limits<u32>::max() - 1;
};

using ABuilder = Application::Builder;

ABuilder &ABuilder::setPort(u16 port) {
    m_port = port;
    return *this;
}
ABuilder &ABuilder::setFolderName(const std::string &folder) {
    m_folder = folder;
    return *this;
}
ABuilder &ABuilder::setSigint(u32 sigint) {
    m_sig_max = sigint;
    return *this;
}

std::unique_ptr<Application> Application::Builder::build() {
    // TODO:
    //  - folder name check
    return std::make_unique<ApplicationImpl>(
            m_port, m_folder, m_sig_max);
}

ApplicationImpl::ApplicationImpl(const u16 port,
                                 const std::string &shared_folder,
                                 const u32 sig_max)
        : m_sig_max(sig_max) {
    shutdown_handler = [this](int) {
        hadSigint();
    };
    signal(SIGINT, signal_handler);
    m_sig_handler.test_and_set(std::memory_order_acquire);

    m_server = Server::create({.port = port}, m_sig_handler);
}

bool ApplicationImpl::start() noexcept {
    if (!m_server) {
        return false;
    }
    return m_server->start();
}

} // namespace http_get