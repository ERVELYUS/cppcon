#pragma once
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>
#include <vector>

class AddrInfoResolver {
 public:
  struct Endpoint {
    std::string host;
    std::string port;
    sockaddr_storage addr{};
    socklen_t addr_len{};
    int family{};
    int socktype{};
    int protocol{};
  };

  static std::vector<Endpoint> resolve(const std::string& host,
                                       const std::string& port,
                                       int family = AF_UNSPEC,
                                       int socktype = SOCK_STREAM,
                                       int flags = 0);
};
