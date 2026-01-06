#include "UdpSocket.h"

#include <system_error>

UdpSocket::UdpSocket() : BaseSocket(AF_INET, SOCK_DGRAM, 0) {}

void UdpSocket::bind(const AddrInfoResolver::Endpoint& endpoint) {
  if (m_fd == -1) {
    throw std::logic_error("bind() called on invalid/moved socket");
  }

  if (::bind(m_fd, reinterpret_cast<const sockaddr*>(&endpoint.addr),
             endpoint.addr_len)) {
    throw std::runtime_error("Failed to bind to port");
  }
}

void UdpSocket::send_to(const std::string& msg,
                        const AddrInfoResolver::Endpoint& endpoint, int flags) {
  if (m_fd == -1) throw std::logic_error("Invalid socket");

  ssize_t n;
  do {
    n = ::sendto(m_fd, msg.data(), msg.size(), flags,
                 reinterpret_cast<const sockaddr*>(&endpoint.addr),
                 endpoint.addr_len);
  } while (n == -1 && errno == EINTR);

  if (n == -1) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      throw std::runtime_error("UDP send buffer full");
    }
    throw std::system_error(errno, std::system_category(), "sendto()");
  }
}

size_t UdpSocket::recv_from(void* buffer, size_t len,
                            AddrInfoResolver::Endpoint& endpoint, int flags) {
  if (m_fd == -1) {
    throw std::logic_error("recv_from() called on invalid/moved socket");
  }

  endpoint.addr_len = sizeof(endpoint.addr);
  while (true) {
    ssize_t n = ::recvfrom(m_fd, buffer, len, flags,
                           reinterpret_cast<struct sockaddr*>(&endpoint.addr),
                           &endpoint.addr_len);

    if (n > 0) {
      return static_cast<size_t>(n);
    }
    else if (n == 0) {
      throw std::runtime_error("Connection closed by peer");
    }
    else {
      if (errno == EINTR) {  // Interrupted, try again
        continue;
      }
      if (errno == EWOULDBLOCK || errno == EAGAIN) {  // If non-blocking
        throw std::runtime_error("No data available yet");
      }
      throw std::system_error(errno, std::system_category(), "recv_from()");
    }
  }
}
