#include <cppcon/BaseSocket.h>
#include <fcntl.h>

#include <stdexcept>

BaseSocket::BaseSocket(int family, int type, int protocol)
    : m_fd(socket(family, type, protocol)) {
  if (m_fd == -1) {
    throw std::runtime_error("Failed to create a socket");
  }
}

void BaseSocket::close() {
  if (m_fd != -1) {
    ::close(m_fd);
    m_fd = -1;
  }
}

BaseSocket::~BaseSocket() { this->close(); }

BaseSocket::BaseSocket(BaseSocket&& other) noexcept : m_fd(other.m_fd) {
  other.m_fd = -1;
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
  if (m_fd == -1)
    throw std::logic_error("set_non_blocking() called on invalid/moved socket");

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
    throw std::runtime_error("fnctl set failed");
  }
}

int BaseSocket::get_fd() const { return m_fd; }
