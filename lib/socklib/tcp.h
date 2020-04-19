#pragma once

#include <memory>

#include "address.h"

const int NoErrorCode = 0;
const int ErrorCode = -1;

class SocketUtils;

class TCPSocket {
public:
    ~TCPSocket();
    int connect(const SocketAddressPtr& remote);
    int bind(const SocketAddress& to);
    int listen(int back_log = 32);
    std::shared_ptr<TCPSocket> accept(SocketAddress& from);
    std::shared_ptr<TCPSocket> accept();
    int send(const void* data, int length);
    int receive(void* buffer, int length);
    bool isActive() const;

private:
    friend class SocketUtils;
    explicit TCPSocket(SocketT socket):
        socket(socket),
        remoteAddress() {}

    SocketT socket;
    SocketAddress remoteAddress;
};

using TCPSocketPtr = std::shared_ptr<TCPSocket>;