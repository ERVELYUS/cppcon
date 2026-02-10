#pragma once
#include <cppcon/AddrInfoResolver.h>
#include <cppcon/BaseSocket.h>

#ifdef _WIN32
#include <winsock2.h>
// Windows calls it WSAPoll, let's alias it so the code stays the same
#define poll WSAPoll
#else
#include <poll.h>
#endif

class SocketSelector {
  std::vector<struct pollfd> m_poll_fds{};

 public:
  void add(const BaseSocket& socket, const short mode);
  void remove(const BaseSocket& socket);
  void wait(int timeout = -1);
  bool is_ready(const BaseSocket& socket);
};
