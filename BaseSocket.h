#pragma once
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>

#include "AddrInfoResolver.h"

class BaseSocket {
 protected:
  int m_fd = -1;

  BaseSocket(int family, int type, int protocol);
  explicit BaseSocket(int fd) : m_fd(fd) {}

 public:
  virtual ~BaseSocket();

  BaseSocket(const BaseSocket& socket) = delete;
  BaseSocket& operator=(const BaseSocket& socket) = delete;

  BaseSocket(BaseSocket&& other) noexcept;
  BaseSocket& operator=(BaseSocket&& other) noexcept;

  void set_non_blocking(bool non_blocking);
  int get_fd() const;

  template <typename T>
  void set_option(int level, int option_name, T option_value);
};
