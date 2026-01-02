#include "BaseSocket.h"

#include <fcntl.h>

#include <stdexcept>

BaseSocket::BaseSocket(int family, int type, int protocol)
    : m_fd(socket(family, type, protocol)) {
  if (m_fd == -1) {
    throw std::runtime_error("Failed to create a socket");
  }
}

BaseSocket::~BaseSocket() {
  if (m_fd != 1) {
    ::close(m_fd);
    m_fd = -1;
  }
}

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

template <typename T>
void BaseSocket::set_option(int level, int option_name, T option_value) {
  if (m_fd == -1)
    throw std::logic_error("set_option() called on invalid/moved socket");

  if (setsockopt(m_fd, level, option_name, &option_value, sizeof(T)) < 0) {
    throw std::runtime_error("setsockopt failed");
  }
}
