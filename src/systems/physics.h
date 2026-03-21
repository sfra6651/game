#pragma once

#include <cmath>

#include "shared/components.h"
#include "ecs/world.h"
#include "shared/componentGroups.h"

enum CollisionResult {
    DESTROY_SELF,
    DESTROY_TARGET,
};
struct PhysicsSystem {
    World &world;

   void processPhysics(){
        int count = world.entities.count;
        std::vector<int> removals {};
        for (int i = 0; i < count; i++) {
            int e_id = world.entities.list[i].id;
            //guard
            if (e_id == REMOVED_ENTITY_ID
                || !world.getStore<Direction>().has(e_id)
                || !hasPysics(e_id, world)) {
                continue;
            };
            Direction dir = world.getStore<Direction>().get(e_id);
            Position pos = world.getStore<Position>().get(e_id);
            Speed speed = world.getStore<Speed>().get(e_id);

            Position new_pos {pos.x + dir.x * speed.v, pos.y + dir.y * speed.v};

            if (std::abs(new_pos.x) >= WORLD_WIDTH ||
                std::abs(new_pos.y) >= WORLD_HEIGHT* 2) {
                removals.push_back(i);
                continue;
            };
            world.getStore<Position>().get(e_id) = new_pos;

        };
       for (auto id : removals) {
           world.erase(world.entities.list[id]);
       }
    };
};


