#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include "UniversalTypes.h"

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
    std::uint8_t bytes[sizeof(T)];
    std::memcpy(bytes, &data, sizeof(T));

    if constexpr (sizeof(T) == 2) {
      std::uint16_t raw;
      std::memcpy(&raw, bytes, 2);
      raw = htobe16(raw);
      std::memcpy(bytes, &raw, 2);
    }
    else if constexpr (sizeof(T) == 4) {
      std::uint32_t raw;
      std::memcpy(&raw, bytes, 4);
      raw = htobe32(raw);
      std::memcpy(bytes, &raw, 4);
    }
    else if constexpr (sizeof(T) == 8) {
      std::uint64_t raw;
      std::memcpy(&raw, bytes, 8);
      raw = htobe64(raw);
      std::memcpy(bytes, &raw, 8);
    }

    append(bytes, sizeof(T));
    return *this;
  }

  Packet& operator<<(const std::string& data);

  template <typename T>
  Packet& operator>>(T& data) {
    if (m_read_pos + sizeof(T) > m_data.size()) {
      throw std::runtime_error("Packet: Not enough data to read");
    }

    const std::uint8_t* src = &m_data[m_read_pos];

    if constexpr (sizeof(T) == 2) {
      std::uint16_t raw;
      std::memcpy(&raw, src, 2);
      raw = be16toh(raw);
      std::memcpy(&data, &raw, 2);
    }
    else if constexpr (sizeof(T) == 4) {
      std::uint32_t raw;
      std::memcpy(&raw, src, 4);
      raw = be32toh(raw);
      std::memcpy(&data, &raw, 4);
    }
    else if constexpr (sizeof(T) == 8) {
      std::uint64_t raw;
      std::memcpy(&raw, src, 8);
      raw = be64toh(raw);
      std::memcpy(&data, &raw, 8);
    }
    else {
      std::memcpy(&data, src, sizeof(T));
    }

    m_read_pos += sizeof(T);

    return *this;
  }

  Packet& operator>>(std::string& data);

  void resize(size_t size);
  std::uint8_t* buffer();
};
