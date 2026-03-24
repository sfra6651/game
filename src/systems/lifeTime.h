#pragma once
#include "components/components.h"
#include "components/entity.h"
#include "ecs/world.h"

struct LifeTimeSystem {
    World& world;
    std::vector<int> removals;
    void processLifeTimes() {
        removals.clear();
        ComponentStore<LifeTime>& store = world.getStore<LifeTime>();
        for (int i = 0; i < world.entities.count; i++) {
            int id = world.entities.list[i].id;
            if(id == REMOVED_ENTITY_ID || !store.has(id)) { continue; };
            LifeTime& lifetime = store.get(id);
            lifetime.remaining -= TICK_RATE;
            if (lifetime.remaining <= 0) {
                lifetime.onExpire(world, lifetime);
                removals.push_back(id);
            }
            //tick down abilities
            if (world.getStore<AbilitySet>().has(id)) {
            }

        }


        for (auto& id : removals) {
            world.erase(world.entities.get(id));
        }

        ComponentStore<AbilitySet>& abStore = world.getStore<AbilitySet>();
        for (int i = 0; i < abStore.entities.size(); i++) {
            tickDownAbilities(abStore.entities[i]);
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
