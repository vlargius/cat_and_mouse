#pragma once

#include <memory>

#include "address.h"

class SocketUtils;

class UDPSocket {
public:
    ~UDPSocket();
    int bind(const SocketAddressPtr& bind_address);
    int send_to(const void* data, int length, const SocketAddressPtr& to);
    int receive_from(void* buffer, int length);
    int setNonBlocking(bool is_non_blocking);

    SocketAddressPtr lastRecvFrom;
private:
    friend class SocketUtils;
    UDPSocket(SocketT socket) :
        socket(socket) {}
    SocketT socket;
};

using UDPSocketPtr = std::shared_ptr<UDPSocket>;
