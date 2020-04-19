#include "game.h"
#include "replication_manager.h"
#include "rpc_manager.h"


void printDump(InStreamType& stream) {
    int val;
    stream.serialize(val);
    std::cout << val << std::endl;
}

void naive_world_replication() {
     auto& rpc = RPCManager::get();
    void * data = nullptr;
    size_t size = 0;

    auto& repl = ReplicationManager::get();
    registerObjectCreation();

    RoboCat initCat;
    initCat.health = 111;
    initCat.meowCount = 333;
    std::strcpy(initCat.name, "hello kitty");

    {
        std::vector<GameObject*> objects;
        {
            RoboCat* cat = new RoboCat();
            cat->health = initCat.health;
            cat->meowCount = initCat.meowCount;
            std::strcpy(cat->name, initCat.name);
            objects.push_back(cat);
        }
        objects.push_back(new RoboCat());
        objects.push_back(new GameObject());

        {
            RoboMouse* mouse = new RoboMouse();
            mouse->health = 876;
            mouse->headCount = 3;
            mouse->legCount = 30303;

            objects.push_back(mouse);
        }


        OutStreamType out;
        auto& repl = ReplicationManager::get();
        repl.replicateWorldState(out, objects);

        for (GameObject * obj : objects) {
            std::cout << repl.linkingContext->getNetworkId(obj, false) << " - " << obj->to_string() << std::endl;
        }

        size = out.size();
        data = new char [size];
        std::memcpy(data, out.getBuffer(), out.size());
    }

    std::cout << "\n\n";

    {
        auto& repl = ReplicationManager::get();

        InStreamType in(data, size);

        repl.receiveWorldState(in);

        for (GameObject * obj : repl.objectsReplicateToMe) {
            std::cout << repl.linkingContext->getNetworkId(obj, false) << " - " << obj->to_string() << std::endl;
        }

        RoboCat* obj = (RoboCat*)*(repl.objectsReplicateToMe.begin());

        assert(obj->health == initCat.health
            && obj->meowCount == initCat.meowCount
            && strcmp(obj->name, initCat.name) == 0);
    }
}

void enhanced_word_replication() {
     auto& rpc = RPCManager::get();
    void * data = nullptr;
    size_t bitSize = 0;

    auto& repl = ReplicationManager::get();
    registerObjectCreation();

    RoboCat initCat;
    initCat.health = 111;
    initCat.meowCount = 333;
    std::strcpy(initCat.name, "hello kitty");

    {
        std::vector<GameObject*> objects;
        // {
        //     RoboCat* cat = new RoboCat();
        //     cat->health = initCat.health;
        //     cat->meowCount = initCat.meowCount;
        //     std::strcpy(cat->name, initCat.name);
        //     objects.push_back(cat);
        // }
        // objects.push_back(new RoboCat());
        // objects.push_back(new HomeBase());
        // objects.push_back(new GameObject());
        {
            RoboMouse* mouse = new RoboMouse();
            mouse->health = 876;
            mouse->headCount = 3;
            mouse->legCount = 30303;
            mouse->serializationFlags.HEALTH = true;
            mouse->serializationFlags.HEAD_COUNT = true;
            mouse->serializationFlags.LEG_COUNT = true;

            std::cout << mouse->to_string() << std::endl;

            objects.push_back(mouse);
        }

        OutStreamType out;

        auto& repl = ReplicationManager::get();
        for (GameObject * object : objects) {
            repl.replicate(out, ReplicationAction::Create, object);
            // std::cout << repl.linkingContext->getNetworkId(object, false)
            //     << " - " << object->to_string() << std::endl;
        }

        // ((RoboCat*)objects[0])->health = 123;
        // repl.replicate(out, ReplicationAction::Update, objects[0]);

        data = new char [out.size()];
        std::memcpy(data, out.getBuffer(), out.size());
        bitSize = out.bit_size();
    }

    std::cout << "\n\n";

    {
        InStreamType in(data, bitSize);

        auto& repl = ReplicationManager::get();

        while (!in.empty())
            repl.processReplication(in);

        for (const auto& id_obj : repl.linkingContext->getAllObjects()) {
            const auto& obj = id_obj.second;
            // std::cout << repl.linkingContext->getNetworkId(obj, false) << " - " << obj->to_string() << std::endl;
        }

        auto* obj = repl.linkingContext->getAllObjects().begin()->second;

        std::cout << obj->to_string() << std::endl;

        // assert(obj->health == initCat.health
        //     && obj->meowCount == initCat.meowCount
        //     && strcmp(obj->name, initCat.name) == 0);
    }
}