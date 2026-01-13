#include <cppcon/AddrInfoResolver.h>

#include <cstring>
#include <memory>
#include <stdexcept>

std::vector<AddrInfoResolver::Endpoint> AddrInfoResolver::resolve(
    const std::string& host, const std::string& port, int family, int socktype,
    int flags) {
  std::vector<Endpoint> results;

  struct addrinfo hints{};
  hints.ai_family = family;
  hints.ai_socktype = socktype;
  hints.ai_family = family;
  hints.ai_flags = flags;

  struct addrinfo* res = nullptr;
  int status = getaddrinfo(host.empty() ? nullptr : host.data(), port.data(),
                           &hints, &res);
  if (status != 0) {
    throw std::runtime_error(std::string("getaddrinfo: ") +
                             gai_strerror(status));
  }

  struct AddrInfoDeleter {
    void operator()(addrinfo* p) const {
      if (p) {
        freeaddrinfo(p);
      }
    }
  };
  std::unique_ptr<addrinfo, AddrInfoDeleter> res_smart(res);

  for (auto* p = res_smart.get(); p != nullptr; p = p->ai_next) {
    Endpoint ep{};
    ep.host = host;
    ep.port = port;
    ep.family = p->ai_family;
    ep.socktype = p->ai_socktype;
    ep.protocol = p->ai_protocol;
    ep.addr_len = static_cast<socklen_t>(p->ai_addrlen);
    std::memcpy(&ep.addr, p->ai_addr, p->ai_addrlen);

    results.push_back(ep);
  }

  return results;
}
