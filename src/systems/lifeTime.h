#pragma once
#include "components/components.h"
#include "ecs/world.h"

struct LifeTimeSystem {
    World& world;
    std::vector<int> removals;
    void processLifeTimes() {
        ComponentStore<LifeTime>& store = world.getStore<LifeTime>();
        for (int i = 0; i < world.entities.count; i++) {
            int id = world.entities.get(id).id;
            LifeTime& lifetime = store.get(id);
            lifetime.remaining -= TICK_RATE;
            if (lifetime.remaining <= 0) {
                lifetime.onExpire(world, lifetime);
                removals.push_back(id);
            }
            //tick down abilities
            if (world.getStore<AbilitySet>().has(id)) {
                tickDownAbilities(id);
            }

        }

        for (auto& id : removals) {
            world.entities.remove(id);
        }
    }

    void tickDownAbilities(int id) {
        AbilitySet& abilitySet = world.getStore<AbilitySet>().get(id);
        for (int i = 0; i < MAX_ABILITIES; i++) {
            float& cdProg = abilitySet.ability[i].cdProg;
            if (cdProg == 0) { continue; }
            cdProg -= TICK_RATE;
            if (cdProg < 0) {
                cdProg = 0;
            }
        }
    }
};
