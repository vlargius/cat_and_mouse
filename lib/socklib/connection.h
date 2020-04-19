#include <memory>
#ifdef LINUX

#include "udp.h"

struct Connection {
    void send(const void * , size_t length);
    int recieve(void * , size_t length);

    void bind();

    SocketAddressPtr remoteAddress;
};

#endif
