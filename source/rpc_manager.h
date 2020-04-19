#pragma once

#include <unordered_map>

#include "replication_header.h"


typedef void (*RPCUnwrapFunc)(InStreamType&);

class RPCManager {
public:

    static RPCManager& get() {
        static RPCManager instance;
        return instance;
    }

    void registerUnwrapFunction(RPCUnwrapFunc function) {
        id_RPC[currentId] = function;
        RPC_id[function] = currentId;
        ++currentId;
    }

    uint16_t getId(RPCUnwrapFunc function)
    {
        auto it = RPC_id.find(function);
        return it != RPC_id.end() ? it->second : std::numeric_limits<uint16_t>::max();
    }

    bool processRPC(InStreamType& stream) {
        uint16_t id;
        stream.serialize(id);
        auto function = id_RPC.find(id);
        if (function != id_RPC.end())
        {
            function->second(stream);
            return true;
        }
        return false;
    }

private:
    RPCManager() = default;
    RPCManager(const RPCManager&) = default;

    uint16_t currentId = 0;
    std::unordered_map<uint16_t, RPCUnwrapFunc> id_RPC;
    std::unordered_map<RPCUnwrapFunc, uint16_t> RPC_id;
};