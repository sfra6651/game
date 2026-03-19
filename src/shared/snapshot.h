#pragma once

#include <vector>

#include "ecs/world.h"
#include "shared/components.h"

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
    Entities entities;
    MakeStores<Components>::storeType stores;

    template<typename T>
    ComponentStore<T>& getStore() {
        return std::get<ComponentStore<T>>(stores);
    }

    void clear() {
        std::apply([&](auto&... stores) { (stores.clear(), ...); }, stores);
    }
};

inline std::vector<EntitySnapshot> createEntitySnaps(World &world) {
    std::vector<EntitySnapshot> snaps;
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
        snaps.push_back(snap);
    }
    return snaps;
}

inline void createWorldSnap(const std::vector<EntitySnapshot> &entitySnaps, WorldSnap &worldSnap) {
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
