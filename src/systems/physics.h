#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "ecs/entity.h"
#include "entities/projectile.h"
#include "ecs/world.h"


struct PhysicsSystem {
    World &world;

   void proccessPhysics(){
        int count = world.entities.freeIndex;
        for (int i = 0; i < count; i++) {
            int e_id = world.entities.list[i].id; 
            if (!world.directions.count(e_id)) { 
                continue;
            };
            Direction dir = world.directions[e_id];
            Position pos = world.positions[e_id];
            Speed speed = world.speeds[e_id];

            int x_new = pos.x += dir.x * speed.v;
            int y_new = pos.y += dir.y * speed.v;
            Position new_pos{ x_new, y_new};
            world.positions[e_id] = new_pos;
            if (new_pos.x >= WINDOW_WIDTH * 2 || new_pos.y >= WINDOW_HEIGHT) {
                world.erase(world.entities.list[i]);
            };
        };
    };

};


