#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "shared/components.h"
#include "entities/projectile.h"
#include "ecs/world.h"


struct RenderingSystem{
    World &world;

   void renderWorld(){
        Entities &entities = world.entities;
        int count = entities.count;
        for (int i = 0; i < count; i++) {
            int e_id = entities.list[i].id;
            if (!world.positions.has(e_id) || !world.renderables.has(e_id) || !world.sizes.has(e_id)) 
            {
                continue;
            }
            Rectangle src {
                0,
                0,
                (float)world.renderables.get(e_id).texture.width,
                (float)world.renderables.get(e_id).texture.height
            };
            Rectangle dest { 
                (float)world.positions.get(e_id).x,
                (float)world.positions.get(e_id).y,
                (float)world.sizes.get(e_id).width,
                (float)world.sizes.get(e_id).height
            };
            DrawTexturePro(world.renderables.get(e_id).texture , src, dest, {0, 0}, 0.0f, WHITE );
        };
    };

};


