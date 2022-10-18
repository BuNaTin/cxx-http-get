#include <Application/Application.h>

#include <functional>
#include <future>
#include <signal.h>

#include <iostream>

namespace {

std::string getPath(const std::string &file) {
    std::size_t pos = file.find_last_of('/') + 1;
    return file.substr(0, std::min(pos, file.size()));
}

std::size_t size_t_max() {
    return std::numeric_limits<std::size_t>::max();
}

std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }
} // namespace

namespace my_project {

class ApplicationImpl final : public Application {
    // interface
public:
    bool start() noexcept override;

    std::atomic_flag &getSigHandler() noexcept override {
        return m_sig_handler;
    }

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
    ApplicationImpl(const Args &args,
                    const std::size_t sig_max);
    virtual ~ApplicationImpl() = default;

    // data
private:
    std::atomic_flag m_sig_handler = ATOMIC_FLAG_INIT;
    std::size_t m_sig_cnt = 0;
    std::size_t m_sig_max = size_t_max();
};

using ABuilder = Application::Builder;

ABuilder &ABuilder::setConfig(const std::string &config_file) {
    m_config = std::make_unique<IniConf>(config_file);
    return *this;
}

ABuilder &ABuilder::setArgs(const Args &args) {
    m_args = &args;
    return *this;
}

std::unique_ptr<Application> Application::Builder::build() {

    using AN = Args::Name;
    std::string worker_config;
    if (m_config) {
        // setup settings from config
        if (!*m_config) {
            return nullptr;
        }
        m_sig_max = (*m_config)(AN::NAMESPACE_DEFAULT)(
                            AN::CATCH_SIGINT_COUNT)
                            .as<std::size_t>();
    }
    if (m_args) {
        // setup setting from args
        if (m_args->has(AN::CATCH_SIGINT_COUNT)) {
            m_sig_max = std::stoi(m_args->get(AN::CATCH_SIGINT_COUNT));
        }
    }

    // check settings before building app
    if (m_sig_max == size_t_max()) {
        return nullptr;
    }
    if (!m_args) {
        return nullptr;
    }
    return std::make_unique<ApplicationImpl>(
            *m_args, m_sig_max);
}

ApplicationImpl::ApplicationImpl(const Args &args,
                                 const std::size_t sig_max)
        : m_sig_max(sig_max) {
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

} // namespace Mib3