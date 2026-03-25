#pragma once
#include "components/components.h"
#include "components/entity.h"
#include "ecs/world.h"

struct LifeTimeSystem {
    World& world;
    std::vector<int> removals;

    // remove entities whos remaing duration has expired and trigger the onExpire function.
    // tick down each lifetime by the tick rate.
    void processLifeTimes() {
        removals.clear();
        ComponentStore<LifeTime>& store = world.getStore<LifeTime>();
        for (int i = 0; i < world.entityManager.count; i++) {
            int id = world.entityManager.list[i].id;
            if(id == REMOVED_ENTITY_ID || !store.has(id)) { continue; };
            LifeTime& lifetime = store.get(id);
            lifetime.remaining -= TICK_RATE;
            if (lifetime.remaining <= 0) {
                lifetime.onExpire(world, lifetime); //triger the onExpire
                removals.push_back(id); //remove entity associated with the lifetime
            }
        }


        for (auto& id : removals) {
            world.erase(world.entityManager.get(id));
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
