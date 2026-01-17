#include <cppcon/AddrInfoResolver.h>
#include <cppcon/Packet.h>
#include <cppcon/TcpListener.h>
#include <cppcon/TcpSocket.h>
#include <cppcon/UdpSocket.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

const std::string PORT = "54000";
const std::string HOST = "127.0.0.1";

enum class CommandID {
  CMD_HELLO = 1,
  CMD_BURST = 2,
  CMD_HUGE_DATA = 3,
  CMD_UDP_PING = 4,
  CMD_BYE = 99
};

void log(const std::string& who, const std::string& msg) {
  std::cout << "[" << who << "] " << msg << std::endl;
}

// --- SERVER LOGIC ---
void run_server() {
  try {
    log("SERVER", "Starting...");

    // 1. Resolve and Bind TCP
    auto tcp_endpoints =
        AddrInfoResolver::resolve(HOST, PORT, AF_INET, SOCK_STREAM);
    if (tcp_endpoints.empty()) {
      throw std::runtime_error("Could not resolve TCP");
    }

    TcpListener listener;
    listener.bind(tcp_endpoints[0]);
    listener.listen(SOMAXCONN);
    log("SERVER", "TCP Bound and Listening...");

    // 2. Resolve and Bind UDP
    auto udp_endpoints =
        AddrInfoResolver::resolve(HOST, PORT, AF_INET, SOCK_DGRAM);
    UdpSocket udp_socket;
    udp_socket.bind(udp_endpoints[0]);

    // 3. Accept Client (Returns a TcpSocket)
    TcpSocket client = listener.accept();
    log("SERVER", "Client connected via TCP!");

    Packet packet;
    bool running = true;
    int burst_count = 0;

    while (running) {
      if (!client.recv(packet)) break;

      std::uint32_t cmd;
      packet >> cmd;

      if (cmd == static_cast<std::uint32_t>(CommandID::CMD_HELLO)) {
        std::string msg;
        packet >> msg;
        log("SERVER", "Hello received: " + msg);
      }
      else if (cmd == static_cast<std::uint32_t>(CommandID::CMD_BURST)) {
        std::uint32_t id;
        packet >> id;
        ++burst_count;
        if (burst_count % 200 == 0)
          log("SERVER", "Burst: " + std::to_string(burst_count));
      }
      else if (cmd == static_cast<std::uint32_t>(CommandID::CMD_HUGE_DATA)) {
        std::string large_str;
        packet >> large_str;
        log("SERVER", "Received huge string of size: " +
                          std::to_string(large_str.size()));

        Packet ack;
        ack << static_cast<uint32_t>(CommandID::CMD_HUGE_DATA)
            << std::string("ACK");
        client.send(ack);
      }
      else if (cmd == static_cast<std::uint32_t>(CommandID::CMD_BYE)) {
        log("SERVER", "Bye received.");
        running = false;
      }
    }

    // 4. Test UDP
    log("SERVER", "Waiting for UDP Ping...");
    AddrInfoResolver::Endpoint client_ep;
    Packet udp_p;
    udp_socket.recv_from(udp_p, client_ep);

    std::uint32_t udp_cmd;
    udp_p >> udp_cmd;
    if (udp_cmd == static_cast<std::uint32_t>(CommandID::CMD_UDP_PING)) {
      log("SERVER", "UDP Ping received from " + client_ep.host);
    }
  }
  catch (const std::exception& e) {
    log("SERVER ERROR", e.what());
  }
}

// --- CLIENT LOGIC ---
void run_client() {
  try {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    log("CLIENT", "Connecting...");

    // 1. TCP Connect
    auto tcp_endpoints =
        AddrInfoResolver::resolve(HOST, PORT, AF_INET, SOCK_STREAM);
    TcpSocket socket;
    socket.connect(tcp_endpoints[0]);
    log("CLIENT", "Connected!");

    // 2. Send Hello
    Packet p1;
    p1 << static_cast<uint32_t>(CommandID::CMD_HELLO)
       << std::string("Client v1.0");
    socket.send(p1);

    // 3. Burst
    log("CLIENT", "Sending burst...");
    for (std::uint32_t i = 0; i < 1000; ++i) {
      Packet p;
      p << static_cast<uint32_t>(CommandID::CMD_BURST) << i;
      socket.send(p);
    }

    // 4. Huge Data (5MB)
    log("CLIENT", "Sending 5MB string...");
    Packet huge;
    huge << static_cast<uint32_t>(CommandID::CMD_HUGE_DATA)
         << std::string(5 * 1024 * 1024, 'A');
    socket.send(huge);

    Packet ack;
    if (socket.recv(ack)) {
      std::uint32_t cmd;
      std::string msg;
      ack >> cmd >> msg;
      log("CLIENT", "Server ACK: " + msg);
    }

    // 5. Bye
    Packet bye;
    bye << static_cast<uint32_t>(CommandID::CMD_BYE);
    socket.send(bye);

    // 6. UDP Ping
    log("CLIENT", "Sending UDP Ping...");
    auto udp_endpoints =
        AddrInfoResolver::resolve(HOST, PORT, AF_INET, SOCK_DGRAM);
    UdpSocket udp;
    Packet udp_p;
    udp_p << static_cast<uint32_t>(CommandID::CMD_UDP_PING);
    udp.send_to(udp_p, udp_endpoints[0]);
  }
  catch (const std::exception& e) {
    log("CLIENT ERROR", e.what());
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./stress_test [server|client]" << std::endl;
    return 0;
  }

  std::string mode = argv[1];
  if (mode == "server") {
    run_server();
  }
  else {
    run_client();
  }

  return 0;
}
