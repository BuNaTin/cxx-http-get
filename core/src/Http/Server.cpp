#include <Http/Server.h>


#ifdef __MINGW32__

#include <WinSock2.h>
#include <mstcpip.h>

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

#include <cstdlib>
#include <numeric>
#include <signal.h>
#include <functional>
#include <vector>

#include <iostream>

#ifdef __MINGW32__
// Макросы для выражений зависимых от OS
#define WIN(exp) exp
#define NIX(exp)

#else

#define WIN(exp)
#define NIX(exp) exp

#endif

#ifdef __MINGW32__ // Windows NT

typedef SOCKET Socket;

#else // POSIX

typedef int Socket;

#endif

namespace {

void workWithClient(
        const std::vector<
                std::pair<std::string, http_get::Server::handler_t>>
                &handlers,
        Socket client_desc);

/**
 * @brief read TCP package
 *
 * @param client_desc - socket descriptor
 * @param data - where to read
 * @param size - max buffer size
 * @param read_size - size of read data
 * @return true
 * @return false
 */
bool readRequest(Socket client_desc,
                 u8 *data,
                 const i64 size,
                 i64 &read_size);
bool readContinue(Socket client_desc, u8 *data, const i64 size);

bool sendContinue(Socket client_desc);
bool sendNotImplemented(Socket client_desc);

bool sendResponse(Socket client_desc, const http_get::Response &resp);

/**
 * @brief Send long response payload stored in file
 *
 * @param client_desc
 * @param data
 * @param size
 * @param read_from - filename
 * @return true
 * @return false
 */
bool sendResponse(Socket client_desc,
                  u8 *data,
                  const i64 size,
                  std::ifstream &read_from);

// implementation

void workWithClient(
        const std::vector<
                std::pair<std::string, http_get::Server::handler_t>>
                &handlers,
        Socket client_desc) {
    const i64 size = 32 * 1024;
    u8 buffer[size] = {0};
    i64 read_size;
    if (!readRequest(client_desc, buffer, size, read_size)) return;
    u8 *end = buffer + read_size;

    std::unique_ptr<http_get::Request> cur_req =
            std::make_unique<http_get::Request>(buffer, end);
    if (cur_req->needContinue()) {
        i64 need_size = cur_req->payloadSize();
        if (!sendContinue(client_desc)) return;
        while (need_size > 0 &&
               readRequest(client_desc, buffer, size, read_size) &&
               read_size) {
            need_size -= read_size;
            cur_req->append(buffer, buffer + read_size);
        }
    }
    auto handler_filter = [query = cur_req->query()](const auto &pair) {
        return utils::fmt::cmp(pair.first, query);
    };
    auto p_handler = std::find_if(
            handlers.begin(), handlers.end(), handler_filter);
    // did not find any handler
    if (p_handler == handlers.end()) {
        return (void)sendNotImplemented(client_desc);
    }
    http_get::Response response = p_handler->second(cur_req->fin());
    if (!sendResponse(client_desc, response) ||
        !response.needContinue()) {
        return;
    }
    // data is big & stored in file
    std::ifstream read_from_stream(response.readFrom());
    while (sendResponse(client_desc, buffer, size, read_from_stream)) {
        ; // do literally nothing, just send data
    }
}

bool readRequest(Socket client_desc,
                 u8 *data,
                 const i64 size,
                 i64 &read_size) {
    read_size = recv(client_desc, data, size, 0);
    if (read_size < 0) {
        std::cerr << "HttpServer couldn't receive message from client "
                  << client_desc << std::endl;
        return false;
    }
    return true;
}

bool readContinue(Socket client_desc, u8 *data, const i64 size) {
    int read_size = recv(client_desc, data, size, 0);
    if (read_size < 0) {
        std::cerr << "HttpServer couldn't receive message from client "
                  << client_desc << std::endl;
        return false;
    }
    if (std::string(data, data + read_size)
                .find("HTTP/1.1 100 Continue") == std::string::npos) {
        std::cerr << "No continue from client " << client_desc
                  << std::endl;
        return false;
    }
    return true;
}

bool sendContinue(Socket client_desc) {
    const i64 size = 50;
    u8 data[size] =
            "HTTP/1.1 100 Continue\r\nConnection: keep-alive\r\n\r\n";

    if (send(client_desc, data, size, 0) < 0) {
        std::cerr << "Can't send 100-continue to " << client_desc
                  << std::endl;
        return false;
    }
    return true;
}

bool sendNotImplemented(Socket client_desc) {
    const i64 size = 33;
    u8 data[size] = "HTTP/1.1 501 Not Implemented\r\n\r\n";

    if (send(client_desc, data, size, 0) < 0) {
        std::cerr << "Can't send 501-not-implemented to " << client_desc
                  << std::endl;
        return false;
    }
    return true;
}

bool sendResponse(Socket client_desc, const http_get::Response &resp) {
    const std::string &data = resp.create();
    if (send(client_desc, data.c_str(), data.size(), 0) < 0) {
        std::cerr << "Can't send response to " << client_desc
                  << std::endl;
        return false;
    }
    return true;
}

bool sendResponse(Socket client_desc,
                  u8 *data,
                  const i64 size,
                  std::ifstream &read_from) {
    std::fill(data, data + size, '\0');
    read_from.read((char *)data, size);
    if (read_from.gcount() == 0) {
        return false;
    }
    if (send(client_desc, data, size, 0) < 0) {
        std::cerr << "Can't send response to " << client_desc
                  << std::endl;
        return false;
    }
    return true;
}

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
    ~ServerImpl();

private:
    std::vector<std::pair<std::string, handler_t>> m_data;
    std::atomic_flag &m_stop;
    i32 m_fd;
};

std::unique_ptr<Server> Server::create(const Server::Settings &settings,
                                       std::atomic_flag &m_stop) {
    Socket socket_desc;
#ifdef __MINGW32__
    socket_desc = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

#else
    struct sockaddr_in server_addr;

    WIN(if(WSAStartup(MAKEWORD(2, 2), &w_data) == 0) {})

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        std::cerr << "HttpServer: could not create socket"
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
        std::cerr << "HttpServer: Could not reuse port"
                  << std::endl;
        return nullptr;
    }

    if (bind(socket_desc,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        std::cerr << "HttpServer: Couldn't bind to the port"
                  << std::endl;
        return nullptr;
    }
#endif


    return std::make_unique<ServerImpl>(socket_desc, m_stop);
}

bool ServerImpl::start() noexcept {
    if (m_fd == -1) {
        std::cerr << "Wrong fd" << std::endl;
        return false;
    }

    if (listen(m_fd, 1) < 0) {
        std::cerr << "HttpServer: err while listening"
                  << std::endl;
        return false;
    }

    int client_sock, client_size;
    struct sockaddr_in client_addr;
    client_size = sizeof(client_addr);

    struct timeval timeout;
    fd_set dummy;

    std::cout << "HttpServer listening" << std::endl;
    while (m_stop.test_and_set(std::memory_order_acquire)) {
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
        // std::cout << "Client number_" << client_sock << std::endl;
        workWithClient(m_data, client_sock);
        if (close(client_sock) < 0) {
            std::cerr << "HttpServer could not close client "
                      << client_sock << std::endl;
        }
    }
    m_stop.clear();
    return true;
}

Server *ServerImpl::addHandler(const std::string &pattern,
                               Server::handler_t &&handler) noexcept {
    m_data.push_back({pattern, std::move(handler)});
    return this;
}

ServerImpl::~ServerImpl() {
    WIN(WSACleanup());
}

}} // namespace http_get::Http