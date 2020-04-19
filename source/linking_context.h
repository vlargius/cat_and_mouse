#pragma once

#include <unordered_map>
#include <cstdint>
#include <limits>

#include "game.h"

#undef max
const uint32_t InvalidId = std::numeric_limits<uint32_t>::max();

class LinkingContext {
public:
    LinkingContext() = default;

    static LinkingContext& get() {
        static LinkingContext instance;
        return instance;
    }

    uint32_t getNetworkId(GameObject* game_object, bool should_create_not_found)  {
        auto it = gameObject_networkId.find(game_object);
        if (it != gameObject_networkId.end()) {
            return it->second;
        } else if (should_create_not_found) {
            uint32_t newNetworkId = nextNetworkId++;
            addGameObject(game_object, newNetworkId);
            return newNetworkId;
        } else {
            return InvalidId;
        }
    }

    GameObject* getGameObject(uint32_t network_id) {
        auto it = networkId_gameObject.find(network_id);
        return it != networkId_gameObject.end()
            ? it->second
            : nullptr;
    }

    void addGameObject(GameObject* game_object, uint32_t network_id) {
        networkId_gameObject[network_id] = game_object;
        gameObject_networkId[game_object] = network_id;
    }

    void removeGameObject(GameObject* game_object) {
        uint32_t networkId = gameObject_networkId[game_object];
        gameObject_networkId.erase(game_object);
        networkId_gameObject.erase(networkId);
    }

    std::unordered_map<uint32_t, GameObject*> getAllObjects() const {
        return networkId_gameObject;
    }

private:
    std::unordered_map<uint32_t, GameObject*> networkId_gameObject;
    std::unordered_map<GameObject*, uint32_t> gameObject_networkId;

    uint32_t nextNetworkId = 0;
};