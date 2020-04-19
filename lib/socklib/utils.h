#pragma once

#ifdef WIN
#include <WinSock2.h>
#endif

#ifdef LIN
#include <sys/socket.h>
#endif

#include "udp.h"
#include "tcp.h"

enum class SocketAddressFamily {
    INET = AF_INET,
    INET6 = AF_INET6
};

const auto inet = SocketAddressFamily::INET;
const auto inent6 = SocketAddressFamily::INET6;

const size_t MTU = 1300;

class SocketUtils {
public:
    static UDPSocketPtr createUDPSocket(SocketAddressFamily family = inet);
    static TCPSocketPtr createTCPSocket(SocketAddressFamily family = inet);

private:
};