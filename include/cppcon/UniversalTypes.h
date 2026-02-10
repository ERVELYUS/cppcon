#pragma once

#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI // ERROR define
    #include <winsock2.h>
    #include <ws2tcpip.h>

    using socket_t = SOCKET;
    using socklen_t = int;
    #define IS_INVALID(s) (s == INVALID_SOCKET)

    // Windows calls it WSAPoll, let's alias it so the code stays the same
    #define poll WSAPoll
#else
    #include <netinet/in.h>
    #include <sys/socket.h>
    
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netdb.h>  // For addrinfo
    #include <poll.h>
    #include <unistd.h>  // For close()

    using socket_t = int;
    #define IS_INVALID(s) (s < 0)
    #define INVALID_SOCKET -1
#endif

#ifndef htobe16
    #define htobe16(x) htons(x)
    #define htobe32(x) htonl(x)
    #define be16toh(x) ntohs(x)
    #define be32toh(x) ntohl(x)
#endif
