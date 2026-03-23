#pragma once
#include "components/components.h"
#include "ecs/world.h"

struct LifeTimeSystem {
    void processLifeTimes(World& world) {
        ComponentStore<LifeTime>& store = world.getStore<LifeTime>();
        for (const auto& entity : store.entities) {
            LifeTime& lifetime = store.get(entity);
            if (lifetime.remaining <= 0) {
                lifetime.onExpire();
            }
        }
    }
};
