#pragma once

#include <vector>

#include "ecs/world.h"
#include "shared/components.h"
#include "shared/tcp.h"

enum ProtocolInstruction {
    PROCESS_NEXT_FRAME,
    WAIT,
};


// the point of this is to send the data over to the debug server. the debug server only needs to recreate the data at this stage.
// So we create a struct to group each entity and its data then recreate the world state from this.
// Its a snapshot of the game state. Debug does not need to worry about systems, we can observer how the system(s) impacts the game state after each frame
struct EntitySnapshot {
    int id {};
    bool hasComponent[MaxComponents] {};
    Components data {};

    template<typename T>
        T& get() {
        return std::get<T>(data);
    }
};

//WorldSnap is supposed to represent the state of the data in the game. it is not a 1:1 copy of world.
struct WorldSnap {
    Entities entities;
    MakeStores<Components>::storeType stores;

    template<typename T>
        ComponentStore<T>& getStore() {
        return std::get<ComponentStore<T>>(stores);
    }


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

    //game generates a list of EntitySnaps to send to debug server.
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

    //we recreate the data in the same format its stored in world.
    //The snaps alone could be enough without this step, we are effectively serializing it to a familar format with all our templated type safety
    void createWorldSnap(WorldSnap &worldSnap)
    {
        worldSnap.clear();
        worldSnap.entities.count = entitySnaps.size();
        for (auto& entitySnap : entitySnaps) {
            worldSnap.entities.list[entitySnap.id].id = entitySnap.id;
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
