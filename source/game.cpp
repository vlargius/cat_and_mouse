#include "game.h"

std::size_t generateClassId()
{
    static uint32_t typeCounter = 0;
    typeCounter++;
    return typeCounter;
}

#define DEFINE_CLASS_ID(name) const uint32_t name::classId = generateClassId();

DEFINE_CLASS_ID(GameObject);
DEFINE_CLASS_ID(RoboCat);
DEFINE_CLASS_ID(RoboMouse);