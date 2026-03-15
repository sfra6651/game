#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "shared/components.h"
#include "entities/projectile.h"
#include "ecs/world.h"

enum CollisionResult {
    DESTROY_SELF,
    DESTROY_TARGET,
};
struct PhysicsSystem {
    World &world;

   void proccessPhysics(){
        int count = world.entities.count;
        std::vector<int> removals {};
        for (int i = 0; i < count; i++) {
            int e_id = world.entities.list[i].id;
            if (e_id == REMOVED_ENTITY_ID || !world.directions.has(e_id)) {
                continue;
            };
            Direction dir = world.directions.get(e_id);
            Position pos = world.positions.get(e_id);
            Speed speed = world.speeds.get(e_id);

            int x_new = pos.x += dir.x * speed.v;
            int y_new = pos.y += dir.y * speed.v;
            Position new_pos{ x_new, y_new};

            if (std::abs(new_pos.x) >= WINDOW_WIDTH * 2 ||
                std::abs(new_pos.y) >= WINDOW_HEIGHT * 2) {
                removals.push_back(i);
                continue;
            };
                world.positions.get(e_id) = new_pos;

        };
       for (auto id : removals) {
           world.erase(world.entities.list[id]);
       }
    };
};


