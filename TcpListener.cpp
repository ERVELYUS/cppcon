#include "TcpListener.h"

TcpListener::TcpListener() : BaseSocket(AF_INET, SOCK_STREAM, 0) {
  int opt = 1;
  set_option(SOL_SOCKET, SO_REUSEADDR, opt);
}

void TcpListener::bind(const AddrInfoResolver::Endpoint& endpoint) {
  if (m_fd == -1) {
    throw std::logic_error("bind() called on invalid/moved socket");
  }

  if (::bind(m_fd, reinterpret_cast<const sockaddr*>(&endpoint.addr),
             endpoint.addr_len)) {
    throw std::runtime_error("Failed to bind to port");
  }
}

void TcpListener::listen(int backlog) {
  if (m_fd == -1) {
    throw std::logic_error("listen() called on invalid/moved socket");
  }

  if (::listen(m_fd, backlog) < 0) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

TcpSocket TcpListener::accept() {
  if (m_fd == -1) {
    throw std::logic_error("accept() called on invalid/moved socket");
  }

  struct sockaddr_storage client_addr{};
  socklen_t addr_len = sizeof(client_addr);
  int client_fd{};

  do {
    client_fd = ::accept(m_fd, reinterpret_cast<struct sockaddr*>(&client_addr),
                         &addr_len);
  } while (client_fd == -1 && errno == EINTR);

  if (client_fd < 0) {
    throw std::runtime_error("Failed to accept connection" + std::string(std::strerror(errno)));
  }

  return TcpSocket(client_fd);
}
