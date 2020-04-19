#include "tcp.h"

#ifdef WIN

#elif
#include <unistd.h>
#endif

TCPSocket::~TCPSocket() {
    #ifdef WIN
    ::closesocket(socket);
    #elif LIN
    ::close(socket);
    #endif
}

int TCPSocket::connect(const SocketAddressPtr& remote) {
    int error = ::connect(socket, &remote->getSockAddr(), remote->size());
    if (error < 0) {
        // SocketUtil::ReportError(L"TCPSocket::Connect");
        // return -SocketUtil::GetLastError();
        return ErrorCode;
    }
    return NoErrorCode;
}

int TCPSocket::bind(const SocketAddress& to) {
    int error = ::bind(socket, &to.getSockAddr(), to.size());
     if (error < 0) {
        // SocketUtil::ReportError(L"TCPSocket::bind");
        // return -SocketUtil::GetLastError();
        return ErrorCode;
    }
    return NoErrorCode;
}

int TCPSocket::listen(int back_log/*=32*/) {
    int error = ::listen(socket, back_log);
    if (error < 0) {
        // SocketUtil::ReportError(L"TCPSocket::Listen");
        // return -SocketUtil::GetLastError();
        // return ErrorCode;
    }
    return NoErrorCode;
}

TCPSocketPtr TCPSocket::accept() {
    socklen_t length;
    sockaddr outSockaddr;

    SocketT newSocket = ::accept(socket, &outSockaddr, &length);

    if (newSocket != InvalidSocket) {
        auto tcpSocket = new TCPSocket(newSocket);
        tcpSocket->remoteAddress = outSockaddr;

        return TCPSocketPtr(tcpSocket);
    } else {
        // SocketUtil::ReportError(L"TCPSocket::Accept");
        return nullptr;
    }
}

int TCPSocket::send(const void* data, int length) {
    int bytesSentCount = ::send(
        socket,
        static_cast<const char*>(data),
        length,
        0);
    if (bytesSentCount < 0) {
        // SocketUtil::ReportError(L"TCPSocket::Send");
        // return -SocketUtil::GetLastError();
        return ErrorCode;
    }
    return bytesSentCount;
}

int TCPSocket::receive(void* buffer, int length) {
    int bufferPointer = 0;
    int bytesReceivedCount = 0;
    while (bytesReceivedCount == 0) {
        bytesReceivedCount = ::recv(socket, bufferPointer + static_cast<char*>(buffer), length, 0);

        if (bytesReceivedCount < 0) {
            // SocketUtil::ReportError(L"TCPSocket::Receive");
            // return -SocketUtil::GetLastError();
            return ErrorCode;
        }
        bufferPointer += bytesReceivedCount;
    }
    return bufferPointer;
}

bool TCPSocket::isActive() const {
    int error = 0;
    int length = sizeof(error);
    #ifdef WIN
    ::getsockopt(socket, SOL_SOCKET, SO_ERROR, (char*)&error, &length);
    #elif LIN
    ::getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &length);
    #endif
    return error == 0;
}
