#pragma once

#include <cppcon/AddrInfoResolver.h>

#include <stdexcept>

class BaseSocket {
 protected:
  socket_t m_fd = INVALID_SOCKET;

  BaseSocket(int family, int type, int protocol);
  explicit BaseSocket(socket_t fd) : m_fd(fd) {}

 public:
  void close();
  virtual ~BaseSocket();

  BaseSocket(const BaseSocket& socket) = delete;
  BaseSocket& operator=(const BaseSocket& socket) = delete;

  BaseSocket(BaseSocket&& other) noexcept;
  BaseSocket& operator=(BaseSocket&& other) noexcept;

  void set_non_blocking(bool non_blocking);
  socket_t get_fd() const;

  template <typename T>
  void set_option(int level, int option_name, T option_value) {
    if (IS_INVALID(m_fd))
      throw std::logic_error("set_option() called on invalid/moved socket");

    const char* opt_ptr = reinterpret_cast<const char*>(&option_value);
    if (setsockopt(m_fd, level, option_name, opt_ptr,
                   static_cast<socklen_t>(sizeof(T))) < 0) {
      throw std::runtime_error("setsockopt failed");
    }
  }
};
