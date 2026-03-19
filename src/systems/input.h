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

    void processInput(Entity e) {

        ComponentStore<Direction> &directions = world.getStore<Direction>();

        //wasd
        if (IsKeyDown(KEY_W)) directions.get(e.id).y = -1.f;
        if (IsKeyDown(KEY_S)) directions.get(e.id).y = 1.f;
        if (IsKeyDown(KEY_A)) directions.get(e.id).x = -1.f;
        if (IsKeyDown(KEY_D)) directions.get(e.id).x = 1.f;

        //diagon normalised movment
        if (IsKeyDown(KEY_W) && IsKeyDown(KEY_D)) {
            directions.get(e.id).x = 0.7071;
            directions.get(e.id).y = -0.7071;
        }
        if (IsKeyDown(KEY_W) && IsKeyDown(KEY_A)) {
            directions.get(e.id).x = -0.7071;
            directions.get(e.id).y = -0.7071;
        }
        if (IsKeyDown(KEY_S) && IsKeyDown(KEY_D)) {
            directions.get(e.id).x = 0.7071;
            directions.get(e.id).y = 0.7071;
        }
        if (IsKeyDown(KEY_S) && IsKeyDown(KEY_A)) {
            directions.get(e.id).x = -0.7071;
            directions.get(e.id).y = 0.7071;
        }

        if (IsKeyReleased(KEY_W)) directions.get(e.id).y = 0;
        if (IsKeyReleased(KEY_S)) directions.get(e.id).y = 0;
        if (IsKeyReleased(KEY_A)) directions.get(e.id).x = 0;
        if (IsKeyReleased(KEY_D)) directions.get(e.id).x = 0;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos= GetMousePosition();
            Position playerPos = world.getStore<Position>().get(e.id);
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


