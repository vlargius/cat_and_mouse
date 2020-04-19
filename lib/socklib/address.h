#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <memory>

#ifdef WIN
#include <winsock2.h>
using socklen_t = int;
#endif

#ifdef LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#endif

using SocketT = int;
const SocketT InvalidSocket = -1;

class SocketAddress
{
public:

    SocketAddress() {
        //fill with invalid
    }

    SocketAddress(std::uint32_t in_address, std::uint16_t in_port) {
        getAsSockAddrIn()->sin_family = AF_INET;
        getAsSockAddrIn()->sin_addr.s_addr = htonl(in_address);
        getAsSockAddrIn()->sin_port = htons(in_port);
    }

    SocketAddress(const sockaddr& in_sock_addr) {
        std::memcpy(&sockAddr, &in_sock_addr, sizeof(sockaddr));
    }

    size_t size() const { return sizeof(sockaddr); }

    const sockaddr&  getSockAddr() const {
        return sockAddr;
    }

    std::string to_string() const {
        return /* inet_ntoa(getAsSockAddrIn()->sin_addr) +  */ std::to_string(htons(getAsSockAddrIn()->sin_port));
    }

private:
    sockaddr sockAddr;

    sockaddr_in* getAsSockAddrIn() {
        return reinterpret_cast<sockaddr_in*>(&sockAddr);
    }

    const sockaddr_in* getAsSockAddrIn() const {
        return reinterpret_cast<const sockaddr_in*>(&sockAddr);
    }
};

using SocketAddressPtr = std::shared_ptr<SocketAddress>;
