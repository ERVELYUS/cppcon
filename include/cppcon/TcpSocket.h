#pragma once
#include <cppcon/AddrInfoResolver.h>
#include <cppcon/BaseSocket.h>

#include <string>

class TcpSocket : public BaseSocket {
  friend class TcpListener;
  explicit TcpSocket(int fd) : BaseSocket(fd) {}

 public:
  TcpSocket();

  void connect(const AddrInfoResolver::Endpoint& endpoint);
  void send(const std::string& msg, int flags = 0);
  size_t recv(void* buffer, size_t len, int flags = 0);
};
