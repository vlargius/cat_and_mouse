
#include "udp.h"

#ifdef WIN

#elif LIN
#include <unistd.h>
#include <fcntl.h>
#endif

UDPSocket::~UDPSocket(){
    #ifdef WIN
    ::closesocket(socket);
    #elif LIN
    ::close(socket);
    #endif
}

const int NoErrorCode = 0;
const int ErrorCode = -1;

int UDPSocket::bind(const SocketAddressPtr& bind_address) {
    int error = ::bind(socket, &bind_address->getSockAddr(), bind_address->size());

    if (error) {
        // SocketUtil::reportError(L"UDPSocket::bind");
        return -1;//SocketUtil::getLastError();
    }
    return NoErrorCode;
}

int UDPSocket::send_to(const void* data, int length, const SocketAddressPtr& to) {
    int byteSentCount = ::sendto( socket,
                                  static_cast<const char*>(data),
                                  length,
                                  0,
                                  &to->getSockAddr(),
                                  to->size());
    if (byteSentCount >= 0) {
        return byteSentCount;
    } else {
        // SocketUtil::reportError(L"UDPSocket::send_to");
        // return -SocketUtil::GetLastError();
        return ErrorCode;
    }
}

const int maxLength = 65535;

int UDPSocket::receive_from(void* buffer, int length) {
    socklen_t fromLength;
    sockaddr fromAddr;
    int readByteCount = ::recvfrom( socket,
                                    static_cast<char*>(buffer),
                                    maxLength,
                                    0,
                                    &fromAddr,
                                    &fromLength);

    if (readByteCount >= 0) {
        lastRecvFrom = SocketAddressPtr(new SocketAddress{fromAddr});
        return readByteCount;
    } else {
        // SocketUtil::ReportError(L"UDPSocket::ReceiveFrom");
        // return -SocketUtil::GetLastError();
        return ErrorCode;
    }
}

int UDPSocket::setNonBlocking(bool is_non_blocking) {
    #ifdef WIN
    return NoErrorCode;
   //todo windows version
    #elif LIN
    int flags = ::fcntl(socket, F_GETFL, 0);
    flags = is_non_blocking
        ? (flags | O_NONBLOCK)
        : (flags & ~O_NONBLOCK);
    ::fcntl(socket, F_SETFL, flags);

    return NoErrorCode;
    #endif
}