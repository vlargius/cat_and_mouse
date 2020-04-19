#include "utils.h"

UDPSocketPtr SocketUtils::createUDPSocket(SocketAddressFamily family/*=inet*/) {
    SocketT s = ::socket(static_cast<int>(family), SOCK_DGRAM, IPPROTO_UDP);
    #ifdef WIN
    bool opt = true;
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    #elif LIN
    int* opt = 0;
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    #endif
    if (s != InvalidSocket) {
        return UDPSocketPtr(new UDPSocket(s));
    } else {
        // ReportError(L"SocketUtil::CreateUDPSocket");
        return nullptr;
    }
}

TCPSocketPtr SocketUtils::createTCPSocket(SocketAddressFamily family/*=inet*/) {
    SocketT s = ::socket(static_cast<int>(family), SOCK_STREAM, IPPROTO_TCP);
    #ifdef WIN
    bool opt = true;
    int optLen = sizeof(opt);
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    #elif LIN
    int* opt = 0;
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    #endif
    if (s != InvalidSocket) {
        return TCPSocketPtr(new TCPSocket(s));
    } else {
        // ReportError(L"SocketUtils::CreateUDPSocket");
        return nullptr;
    }
}