#pragma once
#include <cstdint>

// WINDOWS
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <winsock2.h>
#include <ws2tcpip.h>

// Windows shims
#ifndef htobe16
#define htobe16(x) htons(static_cast<uint16_t>(x))
#define htobe32(x) htonl(static_cast<uint32_t>(x))
#define htobe64(x) htonll(static_cast<uint64_t>(x))
#define be16toh(x) ntohs(static_cast<uint16_t>(x))
#define be32toh(x) ntohl(static_cast<uint32_t>(x))
#define be64toh(x) ntohll(static_cast<uint64_t>(x))
#endif

using socket_t = SOCKET;
using socklen_t = int;
#define IS_INVALID(s) (s == INVALID_SOCKET)
#define poll WSAPoll

// LINUX
#else
#include <arpa/inet.h>
#include <endian.h>  // Linux native htobe/be64toh
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

using socket_t = int;
#define IS_INVALID(s) (s < 0)
#define INVALID_SOCKET -1
#endif
