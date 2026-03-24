#pragma once
#include "components/components.h"
#include "ecs/world.h"

struct LifeTimeSystem {
    World& world;
    void processLifeTimes() {
        ComponentStore<LifeTime>& store = world.getStore<LifeTime>();
        for (const auto& id : store.entities) {
            LifeTime& lifetime = store.get(id);
            lifetime.remaining -= TICK_RATE;
//            if (lifetime.remaining <= 0) {
//                lifetime.onExpire(world, );
//                world.entities.remove(id);
//            }
        }
    }
};
