#pragma once
#include <cppcon/AddrInfoResolver.h>
#include <cppcon/BaseSocket.h>

class SocketSelector {
  std::vector<struct pollfd> m_poll_fds{};

 public:
  void add(const BaseSocket& socket, const short mode);
  void remove(const BaseSocket& socket);
  std::vector<socket_t> wait(int timeout = -1);
};
