#pragma once

#include <limits>
#include <unordered_set>

#include "utils.h"
#include "game.h"
#include "linking_context.h"
#include "object_creating_registry.h"
#include "replication_header.h"
#include "rpc_manager.h"


class ReplicationManager {
public:

    static ReplicationManager& get() {
        static ReplicationManager instance;
        return instance;
    }

    // @brief naive implementation
    void replicateWorldState(
        OutStreamType& stream,
        const std::vector<GameObject*>& all_objects) {

        stream.serialize(
            PacketType::REPLICATION_DATA,
            bits_required<(int)PacketType::PacketTypeCount>::value);

        for (GameObject* obj : all_objects) {
            replicate(stream, obj);
        }
    }

    // @brief naive implementation
    void receiveWorldState(InStreamType& stream) {
        PacketType packetType;
        stream.serialize(packetType, bits_required<(int)PacketType::PacketTypeCount>::value);

        if (packetType != PacketType::REPLICATION_DATA)
            return;

        std::unordered_set<GameObject*> receivedGameObjects;

        while (!stream.empty()) {
            GameObject* recivedGameObject = replicate(stream);
            receivedGameObjects.insert(recivedGameObject);
        }

        for (GameObject* obj : objectsReplicateToMe) {
            if (receivedGameObjects.find(obj) != receivedGameObjects.end()) {
                linkingContext->removeGameObject(obj);
                (*obj).~GameObject();
            }
        }

        objectsReplicateToMe = receivedGameObjects;
    }

    void replicate(OutStreamType& stream, ReplicationAction action, GameObject* game_object = nullptr) {
        if (game_object) {
            switch (action)
            {
            case ReplicationAction::Create: {
                replicateCreate(stream, game_object);
                break;
            }
            case ReplicationAction::Update: {
                replicateUpdate(stream, game_object);
                break;
            }
            case ReplicationAction::Destroy: {
                replicateDestroy(stream, game_object);
                break;
            }
            default:
                break;
            }
        } else {
            ReplicationHeader(action).serialize(stream);
        }
    }

    void processReplication(InStreamType& stream) {
        ReplicationHeader rh;
        rh.serialize(stream);

        switch(rh.action) {
            case ReplicationAction::Create: {
                GameObject* obj = GameObjectCreationRegistry::get().createGameObject(rh.classId);
                linkingContext->addGameObject(obj, rh.networkId);
                obj->serialize(stream);
                break;
            }
            case ReplicationAction::Update: {
                GameObject* obj = linkingContext->getGameObject(rh.networkId);
                if (obj) {
                    obj->serialize(stream);
                } else {
                    obj = GameObjectCreationRegistry::get().createGameObject(rh.classId);
                    obj->serialize(stream);
                    delete obj;
                }
                break;
            }
            case ReplicationAction::Destroy: {
                GameObject*obj = linkingContext->getGameObject(rh.networkId);
                linkingContext->removeGameObject(obj);
                obj->Destroy();
                break;
            }
            case ReplicationAction::RMI: {
                //rmiManager->processRMI(stream
                break;
            }
            case ReplicationAction::RPC: {
                rpcManager->processRPC(stream);
                break;
            }
            default:
            {
                //none
                break;
            }
        }
    }

    std::unordered_set<GameObject*> objectsReplicateToMe;

private:
    ReplicationManager() {
        linkingContext = &LinkingContext::get();
        rpcManager = &RPCManager::get();
        registerObjectCreation();
    }

    ReplicationManager(const ReplicationManager&) = default;

    // @brief naive implementation
    void replicate(
        OutStreamType& stream,
        GameObject* game_object) {

        stream.serialize(linkingContext->getNetworkId(game_object, true));
        stream.serialize(game_object->getClassId());
        game_object->serialize(stream);
    }

    // @brief naive implementation
    GameObject* replicate(InStreamType& stream) {
        uint32_t networkId;
        uint32_t classId;
        stream.serialize(networkId);
        stream.serialize(classId);

        GameObject* obj = linkingContext->getGameObject(networkId);
        if (!obj) {
            obj = GameObjectCreationRegistry::get().createGameObject(classId);
            linkingContext->addGameObject(obj, networkId);
        }

        obj->serialize(stream);

        return obj;
    }

    void replicateCreate(OutStreamType& out, GameObject* object) {
        ReplicationHeader header (
            ReplicationAction::Create,
            linkingContext->getNetworkId(object, true),
            object->getClassId());
        header.serialize(out);
        object->serialize(out);
    }

    void replicateUpdate(OutStreamType& out, GameObject* object) {
        ReplicationHeader header (
            ReplicationAction::Update,
            linkingContext->getNetworkId(object, false),
            object->getClassId());
        header.serialize(out);
        object->serialize(out);
    }

    void replicateDestroy(OutStreamType& out, GameObject* object) {
        ReplicationHeader header (
            ReplicationAction::Destroy,
            linkingContext->getNetworkId(object, false),
            object->getClassId());
        header.serialize(out);
    }
public:
    LinkingContext* linkingContext;
    RPCManager* rpcManager;
};
