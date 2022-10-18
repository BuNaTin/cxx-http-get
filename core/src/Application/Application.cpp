#include <Application/Application.h>

#include <functional>
#include <future>
#include <numeric>
#include <signal.h>

#include <iostream>

namespace {

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
                    const u32 sig_max,
                    const u32 buffer_size);
    virtual ~ApplicationImpl() = default;

    // data
private:
    const u32 m_buffer_size;
    std::string m_shared_folder;
    std::atomic_flag m_sig_handler = ATOMIC_FLAG_INIT;
    u32 m_sig_cnt = 0;
    u32 m_sig_max = std::numeric_limits<u32>::max();
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
ABuilder &ABuilder::setBufferSizeKb(u32 size) {
    m_kb_buffer_size = size;
    return *this;
}

std::unique_ptr<Application> Application::Builder::build() {
    // TODO:
    //  - folder name check
    return std::make_unique<ApplicationImpl>(
            m_port, m_folder, m_sig_max, m_kb_buffer_size * 1024);
}

ApplicationImpl::ApplicationImpl(const u16 port,
                                 const std::string &shared_folder,
                                 const u32 sig_max,
                                 const u32 buffer_size)
        : m_buffer_size(buffer_size),
          m_shared_folder(shared_folder),
          m_sig_max(sig_max) {
    shutdown_handler = [this](int) {
        hadSigint();
    };
    signal(SIGINT, signal_handler);
    m_sig_handler.test_and_set(std::memory_order_acquire);
}

bool ApplicationImpl::start() noexcept {

    std::cout << "Some application work" << std::endl;

    return true;
}

} // namespace http_get