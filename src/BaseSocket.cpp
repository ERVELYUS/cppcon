#include <cppcon/BaseSocket.h>

#ifdef _WIN32
// A small helper class to auto-initialize Winsock
struct WSAInit {
  WSAInit() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
  }
  ~WSAInit() { WSACleanup(); }
};
// Static instance ensures it runs once when the program starts
static WSAInit wsa_init_instance;
#endif

BaseSocket::BaseSocket(int family, int type, int protocol)
    : m_fd(::socket(family, type, protocol)) {
  if (IS_INVALID(m_fd)) {
    throw std::runtime_error("Failed to create a socket");
  }
}

void BaseSocket::close() {
  if (!IS_INVALID(m_fd)) {
#ifdef _WIN32
    ::closesocket(m_fd);
#else
    ::close(m_fd);
#endif
    m_fd = INVALID_SOCKET;
  }
}

BaseSocket::~BaseSocket() { this->close(); }

BaseSocket::BaseSocket(BaseSocket&& other) noexcept : m_fd(other.m_fd) {
  other.m_fd = INVALID_SOCKET;
}

BaseSocket& BaseSocket::operator=(BaseSocket&& other) noexcept {
  if (this != &other) {
    if (m_fd != -1) {
      ::close(m_fd);
    }
    m_fd = other.m_fd;
    other.m_fd = -1;
  }
  return *this;
}

void BaseSocket::set_non_blocking(bool non_blocking) {
  if (IS_INVALID(m_fd)) {
    throw std::logic_error("set_non_blocking() called on invalid/moved socket");
  }

#ifdef _WIN32
  u_long mode = non_blocking ? 1 : 0;
  if (ioctlsocket(m_fd, FIONBIO, &mode) != 0) {
    throw std::runtime_error("ioctlsocket failed");
  }
#else
  int flags = fcntl(m_fd, F_GETFL, 0);
  if (flags == -1) {
    throw std::runtime_error("fcntl get failed");
  }

  if (non_blocking) {
    flags |= O_NONBLOCK;  // turn on
  }
  else {
    flags &= ~O_NONBLOCK;  // turn off
  }

  if (fcntl(m_fd, F_SETFL, flags) == -1) {
    throw std::runtime_error("fcntl set failed");
  }
#endif
}

socket_t BaseSocket::get_fd() const { return m_fd; }
