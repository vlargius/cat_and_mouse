#pragma once

#include "utils.h"
#include "socklib/in_stream.h"
#include "socklib/out_stream.h"

enum class PacketType : uint8_t {
    HELLO,
    REPLICATION_DATA,
    DISCONNECT,

    PacketTypeCount
};

enum class ReplicationAction : uint8_t {
    Create,
    Update,
    Destroy,
    RPC,
    RMI,

    ReplicationActionCount
};

class ReplicationHeader {
public:
    ReplicationHeader() = default;

    ReplicationHeader(ReplicationAction action) :
        action(action) {}

    ReplicationHeader(ReplicationAction action, uint32_t network_id, uint32_t class_id = 0) :
        action(action),
        networkId(network_id),
        classId(class_id) {}

    ReplicationAction   action;
    uint32_t            networkId;
    uint32_t            classId;

    template<class StreamType>
    void serialize(StreamType& stream) {
        stream.serialize(action, bits_required<(int)ReplicationAction::ReplicationActionCount>::value);

        switch (action)
        {
            case ReplicationAction::RPC: {
                break;
            }
            default: {
                stream.serialize(networkId);
                if (action != ReplicationAction::Destroy) {
                    stream.serialize(classId);
                }
                break;
            }
        }
    }

};