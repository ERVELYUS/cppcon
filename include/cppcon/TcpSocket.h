#pragma once
#include <cppcon/AddrInfoResolver.h>
#include <cppcon/BaseSocket.h>
#include <cppcon/Packet.h>

#include <string>

class TcpSocket : public BaseSocket {
  friend class TcpListener;
  explicit TcpSocket(int fd) : BaseSocket(fd) {}

 public:
  TcpSocket();

  void connect(const AddrInfoResolver::Endpoint& endpoint);
  void send(const std::string& msg, int flags = 0);
  size_t recv(void* buffer, size_t len, int flags = 0);

  void send(const Packet& packet, int flags = 0);
  bool recv(Packet& packet, int flags = 0);

 private:
  void send_all(const void* buffer, size_t len, int flags);
  bool recv_all(void* buffer, size_t size, int flags);
};
