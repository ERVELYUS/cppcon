# cppcon: C++ Networking Engine

A lightweight, high-performance C++17 networking library providing an object-oriented wrapper around Linux Sockets. This engine handles the complexities of **Endianness**, **TCP Byte-Streaming** and **Socket Multiplexing**, allowing you to focus on application logic.

---

## Features

* **TCP & UDP Support**: Full abstraction for both connection-oriented and connectionless protocols.
* **Intelligent Packet System**: Automatic serialization/deserialization of C++ types (int, string, float, etc.) with built-in **Big-Endian** conversion for cross-platform compatibility.
* **Address Resolution**: Built-in `AddrInfoResolver` to handle DNS and service-to-port mapping.
* **Socket Selector**: Multiplexing support via `poll()` to handle multiple of concurrent connections in a single thread.
* **Modern C++ Design**: Uses RAII for socket resource management and exception-based error handling.

---

## Architecture

The engine is divided into three logical layers:

1.  **The Transport Layer (`TcpSocket`, `UdpSocket`)**: Handles the raw transmission of bytes and error checking.
2.  **The Presentation Layer (`Packet`)**: Transforms high-level C++ data into "Network Order" byte buffers.
3.  **The Management Layer (`TcpListener`, `SocketSelector`)**: Handles incoming connections and event monitoring.

---

## Getting Started

### Prerequisites
* C++17 Compiler (GCC/Clang)
* CMake 3.10+
* Linux environment (POSIX Sockets)

### Installation & Build

```bash
# Clone the repository
git clone https://github.com/ERVELYUS/cppcon.git
cd cppcon

# Build the library and examples
mkdir build && cd build
cmake ..
make
```

---

# Usage Example

### TCP

```cpp
// Create a packet and pack data
Packet packet;
packet << 1 << std::string("Hello World") << 3.14f;

// Send via TcpSocket
socket.send(packet);
```

### Receiving Data

```cpp
Packet packet;
if (socket.recv(packet)) {
    std::uint32_t id;
    std::string msg;
    packet >> id >> msg;
    // Process your logic
}
```

---

# Testing

The engine includes a **Stress Test** that verifies the integrity of the TCP stream by firing 1,000 rapid-fire packets and a 5MB payload to ensure no data loss occurs.

To run the test:

1. **Start Server:** `./stress_test server`
    
2. **Start Client:** `./stress_test client`
