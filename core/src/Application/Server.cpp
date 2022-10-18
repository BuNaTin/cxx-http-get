#include <Application/Server.h>

#include <arpa/inet.h>
#include <cstdlib>
#include <functional>
#include <future>
#include <netdb.h>
#include <numeric>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

namespace {

void workWithClient(i32 fd, const u64 size) { u8 buffer[size]; }

std::string getPath(const std::string &file) {
    std::size_t pos = file.find_last_of('/') + 1;
    return file.substr(0, std::min(pos, file.size()));
}

std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }
} // namespace

namespace http_get {

class ServerImpl final : public Server {
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
    ServerImpl(const u16 port,
               const std::string &shared_folder,
               const u32 sig_max,
               const u64 buffer_size);
    virtual ~ServerImpl() = default;

    // data
private:
    const u64 m_buffer_size;
    std::string m_shared_folder;
    std::atomic_flag m_sig_handler = ATOMIC_FLAG_INIT;
    i32 m_fd;
    u32 m_sig_cnt = 0;
    u32 m_sig_max = std::numeric_limits<u32>::max() - 1;
};

using ABuilder = Server::Builder;

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

std::unique_ptr<Server> Server::Builder::build() {
    // TODO:
    //  - folder name check
    return std::make_unique<ServerImpl>(
            m_port,
            m_folder,
            m_sig_max,
            static_cast<u64>(m_kb_buffer_size) * 1024);
}

ServerImpl::ServerImpl(const u16 port,
                       const std::string &shared_folder,
                       const u32 sig_max,
                       const u64 buffer_size)
        : m_buffer_size(buffer_size),
          m_shared_folder(shared_folder),
          m_fd(-1),
          m_sig_max(sig_max) {
    shutdown_handler = [this](int) {
        hadSigint();
    };
    signal(SIGINT, signal_handler);
    m_sig_handler.test_and_set(std::memory_order_acquire);

    int socket_desc;
    struct sockaddr_in server_addr;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        std::cerr << "HttpServer: could not create socket" << std::endl;
        return;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    int reuseaddr = 1;

    if (setsockopt(socket_desc,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuseaddr,
                   sizeof(int)) < 0) {
        std::cerr << "HttpServer: Could not reuse port" << std::endl;
        return;
    }

    if (bind(socket_desc,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        std::cerr << "HttpServer: Couldn't bind to the port"
                  << std::endl;
        return;
    }
    m_fd = socket_desc;
}

bool ServerImpl::start() noexcept {
    if (m_fd == -1) {
        std::cerr << "Wrong fd" << std::endl;
        return false;
    }

    if (listen(m_fd, 1) < 0) {
        std::cerr << "HttpServer: err while listening" << std::endl;
        return false;
    }

    int client_sock, client_size;
    struct sockaddr_in client_addr;
    client_size = sizeof(client_addr);

    struct timeval timeout;
    fd_set dummy;

    std::cout << "HttpServer listening" << std::endl;
    while (m_sig_handler.test_and_set()) {
        FD_ZERO(&dummy);
        FD_SET(m_fd, &dummy);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        // std::cout << "HttpServer - tik" << std::endl;
        if (select(m_fd + 1, &dummy, NULL, NULL, &timeout) <= 0) {
            continue;
        };
        client_sock = accept(m_fd,
                             (struct sockaddr *)&client_addr,
                             (socklen_t *)&client_size);
        if (client_sock < 0) {
            std::cerr << "HttpServer couldnot accept" << std::endl;
            return false;
        }
        std::cout << "Client No" << client_sock << std::endl;
        workWithClient(client_sock, m_buffer_size);
        if (close(client_sock) < 0) {
            std::cerr << "HttpServer could not close client "
                      << client_sock << std::endl;
        }
    }
    m_sig_handler.clear();
    return true;
}

} // namespace http_get