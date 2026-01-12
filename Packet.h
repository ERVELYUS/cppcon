#pragma once
#include <cstdint>
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
  Packet& operator<<(T data);

  Packet& operator<<(const std::string& data);

  template <typename T>
  Packet& operator>>(T& data);

  Packet& operator>>(std::string& data);
};
