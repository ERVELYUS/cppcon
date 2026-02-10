#include <cppcon/UdpSocket.h>

#include <cerrno>
#include <stdexcept>
#include <system_error>

#ifdef _WIN32
#define LAST_ERROR WSAGetLastError()
#define ERR_INTR WSAEINTR
#define ERR_AGAIN WSAEWOULDBLOCK
#define ERR_WOULDBLOCK WSAEWOULDBLOCK
using ssize_t = int;
#else
#define LAST_ERROR errno
#define ERR_INTR EINTR
#define ERR_AGAIN EAGAIN
#define ERR_WOULDBLOCK EWOULDBLOCK
#endif

UdpSocket::UdpSocket() : BaseSocket(AF_INET, SOCK_DGRAM, 0) {}

void UdpSocket::bind(const AddrInfoResolver::Endpoint& endpoint) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("bind() called on invalid/moved socket");
  }

  if (::bind(m_fd, reinterpret_cast<const sockaddr*>(&endpoint.addr),
             endpoint.addr_len)) {
    throw std::runtime_error("Failed to bind to port");
  }
}

void UdpSocket::send_to(const std::string& msg,
                        const AddrInfoResolver::Endpoint& endpoint, int flags) {
  if (IS_INVALID(m_fd)) throw std::logic_error("Invalid socket");

  ssize_t n;
  do {
    n = ::sendto(m_fd, msg.data(), msg.size(), flags,
                 reinterpret_cast<const sockaddr*>(&endpoint.addr),
                 endpoint.addr_len);
  } while (n == -1 && LAST_ERROR == ERR_INTR);

  if (n == -1) {
    if (LAST_ERROR == ERR_WOULDBLOCK || LAST_ERROR == ERR_AGAIN) {
      throw std::runtime_error("UDP send buffer full");
    }
    throw std::system_error(LAST_ERROR, std::system_category(), "sendto()");
  }
}

size_t UdpSocket::recv_from(void* buffer, size_t len,
                            AddrInfoResolver::Endpoint& endpoint, int flags) {
  if (IS_INVALID(m_fd)) {
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
      if (LAST_ERROR == ERR_INTR) {  // Interrupted, try again
        continue;
      }
      if (LAST_ERROR == ERR_WOULDBLOCK ||
          LAST_ERROR == ERR_AGAIN) {  // If non-blocking
        throw std::runtime_error("No data available yet");
      }
      throw std::system_error(LAST_ERROR, std::system_category(),
                              "recv_from()");
    }
  }
}

void UdpSocket::send_to(const Packet& packet,
                        const AddrInfoResolver::Endpoint& endpoint, int flags) {
  if (IS_INVALID(m_fd))
    throw std::logic_error("send_to() called on invalid/moved socket");

  const void* data = packet.get_data();
  size_t size = packet.get_size();

  ssize_t n{};
  do {
    n = ::sendto(m_fd, data, size, flags,
                 reinterpret_cast<const sockaddr*>(&endpoint.addr),
                 endpoint.addr_len);
  } while (n == -1 && LAST_ERROR == ERR_INTR);

  if (n == -1) {
    if (LAST_ERROR == ERR_WOULDBLOCK || LAST_ERROR == ERR_AGAIN) {
      throw std::runtime_error("UDP send buffer full");
    }
    throw std::system_error(LAST_ERROR, std::system_category(), "sendto()");
  }
}

size_t UdpSocket::recv_from(Packet& packet,
                            AddrInfoResolver::Endpoint& endpoint, int flags) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("recv_from() called on invalid/moved socket");
  }

  constexpr std::uint16_t BIGGEST_POSSIBLE_PACKET{65535};
  packet.resize(BIGGEST_POSSIBLE_PACKET);

  endpoint.addr_len = sizeof(endpoint.addr);
  ssize_t n{};

  do {
    n = ::recvfrom(m_fd, packet.buffer(), packet.get_size(), flags,
                   reinterpret_cast<struct sockaddr*>(&endpoint.addr),
                   &endpoint.addr_len);
  } while (n == -1 && LAST_ERROR == ERR_INTR);

  if (n > 0) {
    packet.resize(static_cast<size_t>(n));
    return static_cast<size_t>(n);
  }
  else if (n == 0) {
    packet.resize(0);
    return 0;
  }
  else {
    if (LAST_ERROR == ERR_WOULDBLOCK || LAST_ERROR == ERR_AGAIN) {
      throw std::runtime_error("No data available");
    }
    throw std::system_error(LAST_ERROR, std::system_category(), "recvfrom()");
  }
}
