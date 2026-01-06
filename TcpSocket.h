#pragma once
#include <string>

#include "AddrInfoResolver.h"
#include "BaseSocket.h"

class TcpSocket : public BaseSocket {
  friend class TcpListener;

 private:
  explicit TcpSocket(int fd) : BaseSocket(fd) {}

 public:
  TcpSocket();

  void connect(const AddrInfoResolver::Endpoint& endpoint);
  void send(const std::string& msg, int flags = 0);
  size_t recv(void* buffer, size_t len, int flags = 0);
};
