#pragma once
#include <cppcon/AddrInfoResolver.h>
#include <cppcon/BaseSocket.h>
#include <cppcon/TcpSocket.h>

#include <cstring>

class TcpListener : public BaseSocket {
 public:
  TcpListener();

  void bind(const AddrInfoResolver::Endpoint& endpoint);
  void listen(int backlog = SOMAXCONN);

  TcpSocket accept();
};
