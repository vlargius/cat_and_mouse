#pragma once

#include <memory>

#include "game.h"

struct World {
    std::unique_ptr<GameObject> object;

    World() :
        object(new RoboCat())
    {}

    void update(float time, const ControlState& control) {
        object->update(time, control);
    }

};