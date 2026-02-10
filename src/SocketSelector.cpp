#include <cppcon/SocketSelector.h>

#include <algorithm>

void SocketSelector::add(const BaseSocket& socket, const short mode) {
  if (socket.get_fd() == -1) {
    throw std::logic_error(
        "SocketSelector: attempted to add invalid/moved socket");
  }

#ifdef _WIN32
  m_poll_fds.push_back({(SOCKET)socket.get_fd(), mode, 0});
#else
  m_poll_fds.push_back({socket.get_fd(), mode, 0});
#endif
}

void SocketSelector::remove(const BaseSocket& socket) {
  socket_t fd_to_remove = socket.get_fd();

  m_poll_fds.erase(std::remove_if(m_poll_fds.begin(), m_poll_fds.end(),
                                  [fd_to_remove](const pollfd& pfd) {
                                    return pfd.fd == fd_to_remove;
                                  }),
                   m_poll_fds.end());
}

void SocketSelector::wait(int timeout) {
  if (::poll(m_poll_fds.data(), m_poll_fds.size(), timeout) < 0) {
    throw std::runtime_error("poll() failed");
  }
}

bool SocketSelector::is_ready(const BaseSocket& socket) {
  for (const auto& pfd : m_poll_fds) {
    if (pfd.fd == socket.get_fd()) {
      if (pfd.revents & (pfd.events | POLLERR | POLLHUP)) {
        return true;
      }
      return false;
    }
  }
  return false;
}
