#include <cppcon/SocketSelector.h>

void SocketSelector::add(const BaseSocket& socket, const short mode) {
  if (socket.get_fd() == -1) {
    throw std::logic_error(
        "SocketSelector: attempted to add invalid/moved socket");
  }

  m_poll_fds.push_back({socket.get_fd(), mode, 0});
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
