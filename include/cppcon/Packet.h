#pragma once
#include <endian.h>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

class Packet {
  std::vector<uint8_t> m_data{};
  size_t m_read_pos{};

 public:
  Packet() = default;

  void append(const void* data, size_t size);
  void clear();

  const void* get_data() const;
  size_t get_size() const;

  template <typename T>
  Packet& operator<<(T data) {
    T network_data = data;
    if constexpr (sizeof(T) == 2) {
      network_data = htobe16(static_cast<std::uint16_t>(network_data));
    }
    else if constexpr (sizeof(T) == 4) {
      network_data = htobe32(static_cast<std::uint32_t>(network_data));
    }
    else if constexpr (sizeof(T) == 8) {
      network_data = htobe64(static_cast<std::uint64_t>(network_data));
    }

    const std::uint8_t* bytes =
        reinterpret_cast<const std::uint8_t*>(&network_data);
    append(bytes, sizeof(T));
    return *this;
  }

  Packet& operator<<(const std::string& data);

  template <typename T>
  Packet& operator>>(T& data) {
    if (m_read_pos + sizeof(T) > m_data.size()) {
      throw std::runtime_error("Packet: Not enough data to read");
    }

    std::memcpy(&data, &m_data[m_read_pos], sizeof(T));

    if constexpr (sizeof(T) == 2) {
      data = be16toh(static_cast<std::uint16_t>(data));
    }
    else if constexpr (sizeof(T) == 4) {
      data = be32toh(static_cast<std::uint32_t>(data));
    }
    else if constexpr (sizeof(T) == 8) {
      data = be64toh(static_cast<std::uint64_t>(data));
    }

    m_read_pos += sizeof(T);

    return *this;
  }

  Packet& operator>>(std::string& data);
};
