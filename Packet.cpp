#include "Packet.h"

#include <netinet/in.h>

#include <cstring>
#include <stdexcept>

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

template <typename T>
Packet& Packet::operator<<(T data) {
  T network_data = data;
  if (sizeof(T) == 2) {
    network_data = htons(static_cast<std::uint16_t>(network_data));
  }
  else if (sizeof(T) == 4) {
    network_data = htonl(static_cast<std::uint32_t>(network_data));
  }
  else if (sizeof(T) == 8) {
    // TODO
  }

  const std::uint8_t* bytes =
      reinterpret_cast<const std::uint8_t*>(&network_data);
  for (size_t i = 0; i < sizeof(T); ++i) {
    m_data.push_back(bytes[i]);
  }

  return *this;
}

Packet& Packet::operator<<(const std::string& data) {
  std::uint32_t len = static_cast<std::uint32_t>(data.size());
  *this << len;

  for (char c : data) {
    m_data.push_back(static_cast<std::uint8_t>(c));
  }

  return *this;
}

template <typename T>
Packet& Packet::operator>>(T& data) {
  if (m_read_pos + sizeof(T) > m_data.size()) {
    throw std::runtime_error("Packet: Not enough data to read");
  }

  std::memcpy(&data, &m_data[m_read_pos], sizeof(T));

  if (sizeof(T) == 2) {
    data = ntohs(static_cast<uint16_t>(data));
  }
  else if (sizeof(T) == 4) {
    data = ntohl(static_cast<uint32_t>(data));
  }
  else if (sizeof(T) == 8) {
    // TODO
  }

  m_read_pos += sizeof(T);

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
