#pragma once

#ifdef _WIN32
#include <winsock2.h>
#define LAST_ERROR WSAGetLastError()
#define ERR_INTR WSAEINTR
#define ERR_AGAIN WSAEWOULDBLOCK
#define ERR_WOULDBLOCK WSAEWOULDBLOCK
#define ERR_INPROGRESS WSAEINPROGRESS
using ssize_t = SSIZE_T;
#else
#include <cerrno>
#define LAST_ERROR errno
#define ERR_INTR EINTR
#define ERR_AGAIN EAGAIN
#define ERR_WOULDBLOCK EWOULDBLOCK
#define ERR_INPROGRESS EINPROGRESS
#endif
