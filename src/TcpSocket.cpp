#include <cppcon/AddrInfoResolver.h>
#include <cppcon/TcpSocket.h>

#include <cerrno>
#include <system_error>

#ifdef _WIN32
#define LAST_ERROR WSAGetLastError()
#define ERR_INTR WSAEINTR
#define ERR_AGAIN WSAEWOULDBLOCK
#define ERR_WOULDBLOCK WSAEWOULDBLOCK
#define ERR_INPROGRESS WSAEINPROGRESS
// Define ssize_t for Windows
using ssize_t = int;
#else
#include <endian.h>
#define LAST_ERROR errno
#define ERR_INTR EINTR
#define ERR_AGAIN EAGAIN
#define ERR_WOULDBLOCK EWOULDBLOCK
#define ERR_INPROGRESS EINPROGRESS
#endif

TcpSocket::TcpSocket() : BaseSocket(AF_INET, SOCK_STREAM, 0) {}

void TcpSocket::connect(const AddrInfoResolver::Endpoint& endpoint) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("connect() called on invalid/moved socket");
  }

  while (::connect(m_fd, reinterpret_cast<const sockaddr*>(&endpoint.addr),
                   endpoint.addr_len) < 0) {
    if (LAST_ERROR == ERR_INTR) {  // Retry if interrupted
      continue;
    }
    if (LAST_ERROR == ERR_INPROGRESS) {  // Connection started
      return;
    }

    throw std::system_error(LAST_ERROR, std::system_category(), "connect()");
  }
}

void TcpSocket::send(const std::string& msg, int flags) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("send() called on invalid/moved socket");
  }

  const char* data = msg.data();
  size_t total = msg.size();
  size_t sent{0};

  while (sent < total) {
    ssize_t n =
        ::send(m_fd, data + sent, static_cast<int>(total - sent), flags);

    if (n > 0) {
      sent += static_cast<size_t>(n);
    }
    else if (n == -1) {
      if (LAST_ERROR == ERR_INTR) {  // Retry if interrupted
        continue;
      }

      if (LAST_ERROR == ERR_WOULDBLOCK || LAST_ERROR == ERR_AGAIN) {
        throw std::runtime_error("Socket would block");
      }

      throw std::system_error(LAST_ERROR, std::system_category(), "send()");
    }
    else {
      break;
    }
  }
}

size_t TcpSocket::recv(void* buffer, size_t len, int flags) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("recv() called on invalid/moved socket");
  }

  while (true) {
    ssize_t n =
        ::recv(m_fd, static_cast<char*>(buffer), static_cast<int>(len), flags);

    if (n > 0) {
      return static_cast<size_t>(n);
    }
    else if (n == 0) {
      throw std::runtime_error("Connection closed by peer");
    }
    else {
      if (LAST_ERROR == ERR_INTR) {  // Inerrupted, try again
        continue;
      }
      if (LAST_ERROR == ERR_WOULDBLOCK ||
          LAST_ERROR == ERR_AGAIN) {  // If non-blocking
        throw std::runtime_error("No data available yet");
      }
      throw std::system_error(LAST_ERROR, std::system_category(), "recv()");
    }
  }
}

void TcpSocket::send_all(const void* data, size_t len, int flags) {
  const char* start = static_cast<const char*>(data);
  size_t total_sent{0};

  while (total_sent < len) {
    ssize_t bytes = ::send(m_fd, start + total_sent,
                           static_cast<int>(len - total_sent), flags);

    if (bytes == -1) {
      if (LAST_ERROR == ERR_INTR) continue;
      if (LAST_ERROR == ERR_WOULDBLOCK || LAST_ERROR == ERR_AGAIN) {
        throw std::runtime_error("Socket would block inside send_all");
      }
      throw std::system_error(LAST_ERROR, std::system_category(), "send_all()");
    }
    total_sent += bytes;
  }
}

void TcpSocket::send(const Packet& packet, int flags) {
  std::uint32_t payload_size = static_cast<std::uint32_t>(packet.get_size());
  std::uint32_t network_size = htobe32(payload_size);

  send_all(&network_size, sizeof(network_size), flags);

  if (payload_size > 0) {
    send_all(packet.get_data(), payload_size, flags);
  }
}

bool TcpSocket::recv_all(void* buffer, size_t len, int flags) {
  size_t total_received{0};
  char* start = static_cast<char*>(buffer);

  while (total_received < len) {
    ssize_t bytes =
        ::recv(m_fd, start + total_received, len - total_received, flags);

    if (bytes == 0) return false;  // Connection terminated
    if (bytes < 0) {
      if (LAST_ERROR == ERR_INTR) continue;
      if (LAST_ERROR == ERR_WOULDBLOCK || LAST_ERROR == ERR_AGAIN) {
        throw std::runtime_error("Socket would block (incomplete packet)");
      }
      throw std::system_error(LAST_ERROR, std::system_category(), "recv_all()");
    }

    total_received += bytes;
  }

  return true;
}

bool TcpSocket::recv(Packet& packet, int flags) {
  packet.clear();

  // Ask for packet size first
  std::uint32_t network_size = 0;
  if (!recv_all(&network_size, sizeof(network_size), flags)) {
    return false;
  }

  // Terminate if payload is too big
  std::uint32_t payload_size = be32toh(network_size);
  if (payload_size > 100 * 1024 * 1024) {
    throw std::runtime_error("Packet size too large");
  }

  // Resize packet and receive the rest of payload
  if (payload_size > 0) {
    packet.resize(payload_size);
    if (!recv_all(packet.buffer(), payload_size, flags)) {
      return false;
    }
  }

  return true;
}
