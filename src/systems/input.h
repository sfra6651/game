#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "shared/components.h"
#include "entities/projectile.h"
#include "ecs/world.h"

inline Direction calculateDirectionVec(Position origin, Position target){
    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    float len = std::sqrt(dx*dx + dy*dy);
    return { dx/len, dy/len };
};

struct InputSystem {
    World &world;

    void proccessInput(Entity e) {

        if (IsKeyDown(KEY_W)) world.directions.get(e.id).y = -1.f;
        if (IsKeyDown(KEY_S)) world.directions.get(e.id).y = 1.f;
        if (IsKeyDown(KEY_A)) world.directions.get(e.id).x = -1.f;
        if (IsKeyDown(KEY_D)) world.directions.get(e.id).x = 1.f;
    
        if (IsKeyReleased(KEY_W)) world.directions.get(e.id).y = 0;
        if (IsKeyReleased(KEY_S)) world.directions.get(e.id).y = 0;
        if (IsKeyReleased(KEY_A)) world.directions.get(e.id).x = 0;
        if (IsKeyReleased(KEY_D)) world.directions.get(e.id).x = 0;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos= GetMousePosition();
            Position playerPos = world.positions.get(e.id);
            Texture2D bulletTexture = world.textureManager.get("bullet.png");
            Direction dir = calculateDirectionVec({playerPos.x-32, playerPos.y+32}, { (int)mousePos.x, (int)mousePos.y});
            projectileFactory(world, {
             .texture = bulletTexture,
             .owner = { e.id },
             .pos = playerPos,
             .direction = dir,
             .speed = { 10 },
             .damage = { 10 },
         });
        }
    }
};


