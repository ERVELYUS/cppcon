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
include_directories(cppcon PUBLIC include)

add_executable(stress_test examples/stress_test.cpp)
target_link_libraries(stress_test PRIVATE cppcon)

if(WIN32)
  target_link_libraries(cppcon PUBLIC Ws2_32)
endif()
