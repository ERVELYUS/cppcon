#include "TcpSocket.h"

#include <cerrno>
#include <stdexcept>
#include <system_error>

#include "AddrInfoResolver.h"

TcpSocket::TcpSocket() : BaseSocket(AF_INET, SOCK_STREAM, 0) {}

void TcpSocket::connect(const AddrInfoResolver::Endpoint& endpoint) {
  if (m_fd == -1) {
    throw std::logic_error("connect() called on invalid/moved socket");
  }

  while (::connect(m_fd, reinterpret_cast<const sockaddr*>(&endpoint.addr),
                   endpoint.addr_len) < 0) {
    if (errno == EINTR) {  // Retry if interrupted
      continue;
    }
    if (errno == EINPROGRESS) {  // Connection started
      return;
    }

    throw std::system_error(errno, std::system_category(), "connect()");
  }
}

void TcpSocket::send(const std::string& msg, int flags) {
  if (m_fd == -1) {
    throw std::logic_error("send() called on invalid/moved socket");
  }

  const char* data = msg.data();
  size_t total = msg.size();
  size_t sent{0};

  while (sent < total) {
    ssize_t n = ::send(m_fd, data + sent, total - sent, flags);

    if (n > 0) {
      sent += static_cast<size_t>(n);
    }
    else if (n == -1) {
      if (errno == EINTR) {  // Retry if interrupted
        continue;
      }

      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        throw std::runtime_error("Socket would block");
      }

      throw std::system_error(errno, std::system_category(), "send()");
    }
    else {
      break;
    }
  }
}

size_t TcpSocket::recv(void* buffer, size_t len, int flags) {
  if (m_fd == -1) {
    throw std::logic_error("recv() called on invalid/moved socket");
  }

  while (true) {
    ssize_t n = ::recv(m_fd, buffer, len, flags);

    if (n > 0) {
      return static_cast<size_t>(n);
    }
    else if (n == 0) {
      throw std::runtime_error("Connection closed by peer");
    }
    else {
      if (errno == EINTR) {  // Inerrupted, try again
        continue;
      }
      if (errno == EWOULDBLOCK || errno == EAGAIN) {  // If non-blocking
        throw std::runtime_error("No data available yet");
      }
      throw std::system_error(errno, std::system_category(), "recv()");
    }
  }
}
