cmake_minimum_required(VERSION 3.10)

project(cppcon VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_FILES
  src/AddrInfoResolver.cpp
  src/BaseSocket.cpp
  src/Packet.cpp
  src/SocketSelector.cpp
  src/TcpListener.cpp
  src/TcpSocket.cpp
  src/UdpSocket.cpp
)

add_library(cppcon ${SOURCE_FILES})

target_include_directories(cppcon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(test packet_test.cpp)

target_link_libraries(test PRIVATE cppcon)
