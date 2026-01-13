#pragma once

#include <cppcon/AddrInfoResolver.h>
#include <cppcon/BaseSocket.h>

class UdpSocket : public BaseSocket {
 public:
  UdpSocket();

  void bind(const AddrInfoResolver::Endpoint& endpoint);
  void send_to(const std::string& msg,
               const AddrInfoResolver::Endpoint& endpoint, int flags = 0);
  size_t recv_from(void* buffer, size_t len,
                   AddrInfoResolver::Endpoint& endpoint, int flags = 0);
};
