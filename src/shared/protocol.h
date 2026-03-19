#pragma once

#include <vector>

#include "ecs/world.h"
#include "shared/components.h"
#include "shared/tcp.h"

enum ProtocolInstruction {
    PROCESS_NEXT_FRAME,
    WAIT,
};

struct EntitySnapshot {
    int id {};
    bool hasComponent[MaxComponents] {};
    Components data {};

    template<typename T>
        T& get() {
        return std::get<T>(data);
    }
};

struct WorldSnap {
    MakeStores<Components>::storeType stores;

    void clear(){
        std::apply([&](auto&... stores) { (stores.clear(), ...); }, stores);
    };
};

struct DebugProtocol {
    ProtocolInstruction instruction{};
    std::vector<EntitySnapshot> entitySnaps{};


    bool sendTo(TcpClient &client){
        int count = entitySnaps.size(); 
        return client.send(&instruction, sizeof(instruction))
            && client.send(&count, sizeof(count))
            && client.send(entitySnaps.data(), count * sizeof(EntitySnapshot));
    };

    bool recvFrom(TcpServer &server){
        int count {};
        if(!server.recv(&instruction, sizeof(instruction))) { return false; };
        if(!server.recv(&count, sizeof(count))) { return false; };
        entitySnaps.resize(count);
        return server.recv(entitySnaps.data(), count * sizeof(EntitySnapshot));
    }

    void createEntitySnaps(World &world)
    {
        entitySnaps.clear();
        for (int i = 0; i < world.entities.count; i++) {
            int id = world.entities.list[i].id;
            EntitySnapshot snap {};
            snap.id = id;
            int idx = 0;
            std::apply([&](auto&... stores) {
                ((stores.has(id)
                    ? (void)(snap.hasComponent[idx++] = true,
                       std::get<typename std::decay_t<decltype(stores)>::value_type>(snap.data) = stores.get(id))
                    : (void)idx++)
                , ...);
            }, world.stores);
            entitySnaps.push_back(snap);
        }
    }

    void parseEntitySnaps(WorldSnap &worldSnap)
    {
        worldSnap.clear();
        for (auto& entitySnap : entitySnaps) {
            int idx = 0;
            std::apply([&](auto&... stores) {
                ((entitySnap.hasComponent[idx++]
                    ? (stores.add(entitySnap.id,
                         std::get<typename std::decay_t<decltype(stores)>::value_type>(entitySnap.data)), 0)
                    : 0)
                , ...);
            }, worldSnap.stores);
        }
    }
};
