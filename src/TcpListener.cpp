#include <cppcon/TcpListener.h>

#include <system_error>

#ifdef _WIN32
#define LAST_ERROR WSAGetLastError()
#define ERR_INTR WSAEINTR
#else
#define LAST_ERROR errno
#define ERR_INTR EINTR
#endif

TcpListener::TcpListener() : BaseSocket(AF_INET, SOCK_STREAM, 0) {
  int opt = 1;
  set_option(SOL_SOCKET, SO_REUSEADDR, opt);
}

void TcpListener::bind(const AddrInfoResolver::Endpoint& endpoint) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("bind() called on invalid/moved socket");
  }

  if (::bind(m_fd, reinterpret_cast<const sockaddr*>(&endpoint.addr),
             endpoint.addr_len)) {
    throw std::runtime_error("Failed to bind to port");
  }
}

void TcpListener::listen(int backlog) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("listen() called on invalid/moved socket");
  }

  if (::listen(m_fd, backlog) < 0) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

TcpSocket TcpListener::accept() {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("accept() called on invalid/moved socket");
  }

  struct sockaddr_storage client_addr{};
  socklen_t addr_len = sizeof(client_addr);
  socket_t client_fd{};

  do {
    client_fd = ::accept(m_fd, reinterpret_cast<struct sockaddr*>(&client_addr),
                         &addr_len);
  } while (client_fd == -1 && LAST_ERROR == ERR_INTR);

  if (IS_INVALID(client_fd)) {
    // FIX: Use std::system_error for cross-platform error strings
    throw std::system_error(LAST_ERROR, std::system_category(), "accept()");
  }

  return TcpSocket(client_fd);
}
