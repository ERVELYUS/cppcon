#pragma once
#include <cppcon/AddrInfoResolver.h>
#include <cppcon/BaseSocket.h>

#include <utility>

class SocketSelector {
  std::vector<struct pollfd> m_poll_fds{};

 public:
  void add(const BaseSocket& socket, short mode);
  void remove(const BaseSocket& socket);

  // Returns pairs of (fd, revents) for all sockets with pending events.
  std::vector<std::pair<socket_t, short>> wait(int timeout = -1);
};
