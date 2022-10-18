#include <Http/Server.h>

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

void workWithClient(i32 fd) {}

} // namespace

namespace http_get { inline namespace Http {

class ServerImpl final : public Server {
public:
    bool start() noexcept override;

    Server *addHandler(const std::string &pattern,
                       Server::handler_t &&handler) noexcept override;

    // constructors
public:
    ServerImpl(i32 fd, std::atomic_flag &stop)
            : m_stop(stop), m_fd(fd) {}
    ~ServerImpl() = default;

private:
    std::atomic_flag &m_stop;
    i32 m_fd;
};

std::unique_ptr<Server> Server::create(const Server::Settings &settings,
                                       std::atomic_flag &m_stop) {
    int socket_desc;
    struct sockaddr_in server_addr;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        std::cerr << "HttpApplication: could not create socket"
                  << std::endl;
        return nullptr;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(settings.port);
    server_addr.sin_addr.s_addr = inet_addr(settings.address.c_str());
    int reuseaddr = 1;

    if (setsockopt(socket_desc,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuseaddr,
                   sizeof(int)) < 0) {
        std::cerr << "HttpApplication: Could not reuse port"
                  << std::endl;
        return nullptr;
    }

    if (bind(socket_desc,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        std::cerr << "HttpApplication: Couldn't bind to the port"
                  << std::endl;
        return nullptr;
    }
    return std::make_unique<ServerImpl>(socket_desc, m_stop);
}

bool ServerImpl::start() noexcept {
    if (m_fd == -1) {
        std::cerr << "Wrong fd" << std::endl;
        return false;
    }

    if (listen(m_fd, 1) < 0) {
        std::cerr << "HttpApplication: err while listening"
                  << std::endl;
        return false;
    }

    int client_sock, client_size;
    struct sockaddr_in client_addr;
    client_size = sizeof(client_addr);

    struct timeval timeout;
    fd_set dummy;

    std::cout << "HttpApplication listening" << std::endl;
    while (m_stop.test_and_set(std::memory_order_acquire)) {
        FD_ZERO(&dummy);
        FD_SET(m_fd, &dummy);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        // std::cout << "HttpApplication - tik" << std::endl;
        if (select(m_fd + 1, &dummy, NULL, NULL, &timeout) <= 0) {
            continue;
        };
        client_sock = accept(m_fd,
                             (struct sockaddr *)&client_addr,
                             (socklen_t *)&client_size);
        if (client_sock < 0) {
            std::cerr << "HttpApplication couldnot accept" << std::endl;
            return false;
        }
        std::cout << "Client No" << client_sock << std::endl;
        workWithClient(client_sock);
        if (close(client_sock) < 0) {
            std::cerr << "HttpApplication could not close client "
                      << client_sock << std::endl;
        }
    }
    m_stop.clear();
    return true;
}

Server *ServerImpl::addHandler(const std::string &pattern,
                               Server::handler_t &&handler) noexcept {

    return this;
}

}} // namespace http_get::Http