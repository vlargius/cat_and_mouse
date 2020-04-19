#pragma once

#include <functional>
#include <unordered_map>
#include <cassert>

#include "game.h"


typedef GameObject* (*GameObjectCreationFunction)();

class GameObjectCreationRegistry {
public:
    static GameObjectCreationRegistry& get() {
        static GameObjectCreationRegistry instance;
        return instance;
    }

    template<class T>
    void RegisterCreationFunction() {
        assert (name_creationFunction.find(T::classId)
            == name_creationFunction.end());
        name_creationFunction[T::classId] = T::createInstance;
    }

    GameObject* createGameObject(uint32_t class_id) {
        //todo check errors if class_id has not been found
        const auto creationFunc = name_creationFunction.find(class_id);
        assert(creationFunc != name_creationFunction.end());
        return creationFunc->second();
    }

private:
    GameObjectCreationRegistry() = default;
    GameObjectCreationRegistry(const GameObjectCreationRegistry&) = default;

    std::unordered_map<uint32_t, GameObjectCreationFunction> name_creationFunction;
};

void registerObjectCreation() {
    auto& registry = GameObjectCreationRegistry::get();

    registry.RegisterCreationFunction<GameObject>();
    registry.RegisterCreationFunction<RoboCat>();
    registry.RegisterCreationFunction<RoboMouse>();
}