#pragma once

#include <cmath>

#include "iostream"

#include "ecs/world.h"

struct CollisionSystem {
    World &world;
    std::vector<int> removals {};

    void processCollisions() {
        removals.clear();
        for (auto& entity : world.entities.list) {
            if (!world.directions.has(entity.id)
                || !world.renderables.has(entity.id)
                || !world.sizes.has(entity.id)
                || !world.positions.has(entity.id)
             ) {
                continue;
            }
            int collision_e_id = collidesWith(entity.id);
            if (collision_e_id != -1) {
                handleCollision(entity.id, collision_e_id);
            };

        }    
        for (auto& id : removals) {
            world.erase({id});
        }
    }

    int collidesWith(int current_e_id) {
        for (auto& entity : world.entities.list) {
            if (current_e_id == entity.id) { continue; }
            if (world.owners.has(entity.id)) { continue; }
            if (!world.directions.has(entity.id)
                || !world.renderables.has(entity.id)
                || !world.sizes.has(entity.id)
                || !world.positions.has(entity.id)
             ) {
               continue;
            }
            Rectangle rect {
                (float)world.positions.get(current_e_id).x,
                (float)world.positions.get(current_e_id).y,
                (float)world.sizes.get(current_e_id).width,
                (float)world.sizes.get(current_e_id).height
            };
            Rectangle other_rect {
                (float)world.positions.get(entity.id).x,
                (float)world.positions.get(entity.id).y,
                (float)world.sizes.get(entity.id).width,
                (float)world.sizes.get(entity.id).height
            };
            if (CheckCollisionRecs(rect, other_rect)) {
                return entity.id;
            }
        }
        return -1;
    }

    void handleCollision(int e_id, int collision_e_id) {
        if (world.damages.has(e_id) && world.healths.has(collision_e_id)) {
            resolveDamage(e_id, collision_e_id);
        }
    }

    void resolveDamage(int damaging_id, int target_id) {
        world.healths.get(target_id).v -= world.damages.get(damaging_id).v;
        if (world.healths.get(target_id).v <= 0) {
            removals.push_back(target_id);
        }
        removals.push_back(damaging_id);
    }
};


