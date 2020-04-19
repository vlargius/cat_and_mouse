#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "socklib/in_stream.h"
#include "socklib/utils.h"
#include "socklib/address_factory.h"

#include "world.h"
#include "render/render.h"
#include "replication_manager.h"
#include "input_manager.h"

void log(const std::string& msg) {
    std::cout << msg << std::endl;
}

void render_object(const GameObject* object, Render* render) {
    auto rc = reinterpret_cast<const RoboCat*>(object);
    std::cout << rc->posX << " " << rc->posY << std::endl;

    render->drawRect(rc->posX, rc->posY, 30, 30);
}

int main(int argc, const char* argv[]) {
    log("started");
    init_sock();

    std::vector<std::string> args(argv + 1, argv + argc);

    std::string serverIp = "127.0.0.1:12345";
    if (!args.empty())
    {
        serverIp = args.front();
    }
    auto serverAddress = SocketAddressFactory::createIPv4FromString(serverIp);

    auto tcp = SocketUtils::createTCPSocket();
    log("wait for connection");

    if (tcp->connect(serverAddress) == NoErrorCode) {
        log("connected");
    } else {
        log("connection error");
        return 0;
    }

    auto start = std::chrono::system_clock::now();

    Render render;
    auto& repl = ReplicationManager::get();

    // initial object creation
    {
        char buffer[MTU];
        int count = tcp->receive((void *)buffer, MTU);
        InStreamType in(buffer, count * 8);

        repl.processReplication(in);
    }

    InputState state;

    while (state.working) {
        do_input(state);

        char buffer[MTU];
        int count = tcp->receive((void *)buffer, MTU);
        InStreamType in(buffer, count * 8);

        repl.processReplication(in);

        render.clear();
        render_object(LinkingContext::get().getAllObjects().begin()->second, &render);
        render.flush();
    }

    return 0;
}