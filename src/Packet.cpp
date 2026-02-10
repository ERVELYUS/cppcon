#include <cppcon/Packet.h>

#include <cstring>

void Packet::append(const void* data, size_t size) {
  if (data && size > 0) {
    const std::uint8_t* bytes = static_cast<const std::uint8_t*>(data);
    m_data.insert(m_data.end(), bytes, bytes + size);
  }
}

void Packet::clear() {
  m_data.clear();
  m_read_pos = 0;
}

const void* Packet::get_data() const { return m_data.data(); }

size_t Packet::get_size() const { return m_data.size(); }

Packet& Packet::operator<<(const std::string& data) {
  std::uint32_t len = static_cast<std::uint32_t>(data.size());
  *this << len;

  m_data.insert(m_data.end(), data.begin(), data.end());

  return *this;
}

Packet& Packet::operator>>(std::string& data) {
  std::uint32_t len = 0;
  *this >> len;

  if (m_read_pos + len > m_data.size()) {
    throw std::runtime_error("Packet: String length exceeds buffer size");
  }

  data.assign(reinterpret_cast<const char*>(&m_data[m_read_pos]), len);

  m_read_pos += len;

  return *this;
}

void Packet::resize(size_t size) {
  m_data.resize(size);
  m_read_pos = 0;
}

std::uint8_t* Packet::buffer() { return m_data.data(); }
