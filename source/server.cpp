#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "socklib/utils.h"
#include "socklib/address_factory.h"
#include "socklib/out_stream.h"
#include "socklib/in_stream.h"

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
    std::vector<std::string> args(argv + 1, argv + argc);

    log("started");
    init_sock();

    auto start = std::chrono::system_clock::now();

    std::string ipAddress = "127.0.0.1:12345";
    if (!args.empty())
    {
        ipAddress = args.front();
    }
    auto listenAddress = SocketAddressFactory::createIPv4FromString(ipAddress);

    auto listenTCP = SocketUtils::createTCPSocket();

    listenTCP->bind(*listenAddress.get());
    log("listen ...");

    listenTCP->listen();
    auto tcp = listenTCP->accept();
    log("accept");

    Render render;
    World world;
    auto& repl = ReplicationManager::get();

    // initial object creation
    {
        OutStreamType out;
        ((RoboCat*)world.object.get())->posX = 300;
        ((RoboCat*)world.object.get())->posY = 200;
        repl.replicate(out, ReplicationAction::Create, world.object.get());

        tcp->send(out.getBuffer(), out.size());
    }

    InputState input;

    while (input.working) {
        auto current = std::chrono::system_clock::now();
        const float time = std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count() / 1000.f;

        do_input(input);

        OutStreamType out;
        ControlState control{input.up, input.down, input.left, input.right};
        world.update(time, control);

        repl.replicate(out, ReplicationAction::Update, world.object.get());

        auto errorCode = tcp->send(out.getBuffer(), out.size());
        if (errorCode == NoErrorCode) {
            std::cout << "client is disconected. stopping" << std::endl;
        }

        render.clear();
        render_object(world.object.get(), &render);
        render.flush();

        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(0.01s);
        }
    }
    return 0;
}