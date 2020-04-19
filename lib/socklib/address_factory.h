#pragma once

#include <string>

#include "address.h"

#ifdef WIN
#include <ws2tcpip.h>
#include <Windef.h>

void init_sock() {
    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;

    if (int err = WSAStartup(wVersionRequested, &wsaData)) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        std::cout << "WSAStartup failed with error: " << err << std::endl;
    }
}
#elif
#include <netdb.h>
#endif

const std::string defaultService = "0";
const auto defaultFamily = AF_INET;

class SocketAddressFactory {
public:
    static SocketAddressPtr createIPv4FromString(const std::string& host_name) {
        auto commaPos = host_name.find_last_of(':');
        std::string host, service;
        if (commaPos !=std::string::npos) {
            host = host_name.substr(0, commaPos);
            service = host_name.substr(commaPos + 1);
        } else {
            host = host_name;
            service = defaultService;
        }
        addrinfo hint;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = defaultFamily;

        addrinfo* result;
        int error = ::getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
        if (error != 0 || !result) {
            if (result != nullptr) {
                ::freeaddrinfo(result);
            }
            return nullptr;
        }

        while (!result->ai_addr && result->ai_next) {
            result = result->ai_next;
        }
        if (!result->ai_addr) {
            ::freeaddrinfo(result);
            return nullptr;
        }
        auto outAddress = std::make_shared<SocketAddress>(*result->ai_addr);
        ::freeaddrinfo(result);

        return outAddress;
    }

};