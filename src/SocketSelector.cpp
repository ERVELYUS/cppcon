#include <cppcon/SocketSelector.h>

#include <algorithm>
#include <vector>

void SocketSelector::add(const BaseSocket& socket, const short mode) {
  if (IS_INVALID(socket.get_fd())) {
    throw std::logic_error("SocketSelector: invalid socket");
  }

  m_poll_fds.push_back({static_cast<socket_t>(socket.get_fd()), mode, 0});
}

void SocketSelector::remove(const BaseSocket& socket) {
  socket_t fd_to_remove = socket.get_fd();

  m_poll_fds.erase(std::remove_if(m_poll_fds.begin(), m_poll_fds.end(),
                                  [fd_to_remove](const pollfd& pfd) {
                                    return pfd.fd == fd_to_remove;
                                  }),
                   m_poll_fds.end());
}

std::vector<socket_t> SocketSelector::wait(int timeout) {
  if (m_poll_fds.empty()) return {};

#ifdef _WIN32
  ULONG nfds = static_cast<ULONG>(m_poll_fds.size());
#else
  nfds_t nfds = static_cast<nfds_t>(m_poll_fds.size());
#endif

  int count = ::poll(m_poll_fds.data(), nfds, timeout);

  if (count < 0) {
    throw std::runtime_error("poll() failed");
  }
  else if (count == 0) {
    return {};
  }

  std::vector<socket_t> ready_sockets;
  ready_sockets.reserve(count);

  for (const auto& pfd : m_poll_fds) {
    if (pfd.revents & (POLLIN | POLLOUT | POLLERR | POLLHUP)) {
      ready_sockets.push_back(pfd.fd);
    }
  }

  return ready_sockets;
}
