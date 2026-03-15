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
            int collision_e_id = collidesWith(e_id);
            if (collision_e_id != -1) { handleCollision(e_id, collision_e_id, new_pos, removals); };

            world.positions.get(e_id) = new_pos;

        };
       for (auto id : removals) {
           world.erase(world.entities.list[id]);
       }
    };

    int collidesWith(int current_e_id) {
        for (auto& entity : world.entities.list) {
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
    };

    void handleCollision(int e_id, int collision_e_id, Position new_pos,std::vector<int>& removals) {
       if (world.damages.has(e_id) && world.healths.has(collision_e_id)) {
          world.healths.get(collision_e_id).v -= world.damages.get(e_id).v;
          if (world.healths.get(collision_e_id).v <= 0) {
              removals.push_back(collision_e_id);
          }
       };
    };

};


