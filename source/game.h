#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <cmath>

#include "socklib/out_stream.h"
#include "socklib/in_stream.h"
#include "socklib/connection.h"
#include "utils.h"


struct ControlState {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
};

class GameObject;

class GameObject {
public:
    const static uint32_t classId;
    virtual uint32_t getClassId() { return classId; }

    virtual ~GameObject() {}

    void Destroy() {}
    virtual void update(float time, const ControlState& control) {};

    static GameObject* createInstance() { return new GameObject(); }

    virtual std::string to_string() const { return "game object"; }

    virtual void serialize(InStreamType& in) { serialize<InStreamType>(in); }
    virtual void serialize(OutBitStream& out) { serialize<OutBitStream>(out); }

    template<class StreamType>
    void serialize(StreamType& stream) {}
};


class RoboCat : public GameObject {
public:
    const static uint32_t classId;
    uint32_t getClassId() override { return classId; }

    float         posX = 0;
    float         posY = 0;
    int         health = 10;
    int         meowCount = 1;
    GameObject* homeBase = nullptr;
    char        name[128] = "\0";
    std::vector<int> miceIndices;

    static GameObject* createInstance() { return new RoboCat(); }

    void update(float time, const ControlState& control) override {
        const float centerX = 300.f;
        const float centerY = 200.f;
        const float radius = 150.f;
        const float period = 1.f;

        float vel = 5.f;

        if (control.up)
            posX -= vel;
        if (control.down)
            posX += vel;
        if (control.left)
            posY -= vel;
        if (control.right)
            posY += vel;

        // posX = centerX + radius * sin(period * time);
        // posY = centerY + radius * cos(period * time);
    }

    std::string to_string() const override { return "robo cat"; }

    void serialize(InStreamType& in) override { serialize<InStreamType>(in); }
    void serialize(OutBitStream& out) override { serialize<OutBitStream>(out); }

    template <class StreamType>
    void serialize(StreamType &stream) {
        stream.serialize(posX);
        stream.serialize(posY);
        // stream.serialize(health);
        // stream.serialize(meowCount);
        //no solution for homeBase yet
        // stream.serializeArray(name, 128);
        //no solition for miceIndices yet
    }
};

class RoboMouse : public GameObject {
public:
    char name[128];
    int legCount;
    int headCount;
    int health;

    struct SerializationFlags {
        bool NAME : 1;
        bool LEG_COUNT : 1;
        bool HEAD_COUNT : 1;
        bool HEALTH : 1;
    } serializationFlags;

    const static uint32_t classId;
    uint32_t getClassId() override { return classId; }

    static GameObject* createInstance() { return new RoboMouse(); }

    RoboMouse():
        health(0),
        legCount(0),
        headCount(0) {
        name[0] = '\0';
    }

    std::string to_string() const override {
        return std::string(name) + " "
            + std::to_string(legCount) + " "
            + std::to_string(headCount) + " "
            + std::to_string(health);
    }

    void serialize(InStreamType& in) override { serialize<InStreamType>(in); }
    void serialize(OutBitStream& out) override { serialize<OutBitStream>(out); }

    template <class StreamType>
    void serialize(StreamType &stream) {
        stream.serialize(serializationFlags, 5);

        if (serializationFlags.NAME)
            stream.serializeArray(name, 128);

        if (serializationFlags.LEG_COUNT)
            stream.serialize(legCount);

        if (serializationFlags.HEAD_COUNT)
            stream.serialize(headCount);

        if (serializationFlags.HEALTH)
            stream.serialize(health);
    }
};
