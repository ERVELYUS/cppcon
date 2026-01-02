#pragma once
#include <cstring>

#include "AddrInfoResolver.h"
#include "BaseSocket.h"
#include "TcpSocket.h"

class TcpListener : public BaseSocket {
 public:
  TcpListener();

  void bind(const AddrInfoResolver::Endpoint& endpoint);
  void listen(int backlog = SOMAXCONN);

  TcpSocket accept();
};
