#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "shared/components.h"
#include "entities/projectile.h"
#include "ecs/world.h"
#include "lib/utils.h"

inline Direction calculateDirectionVec(Position origin, Position target){
    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    float len = std::sqrt(dx*dx + dy*dy);
    return { dx/len, dy/len };
};

inline Vector2 getMouseWorldPosition(const Camera2D& camera, const Vector2& virtualScale) {
    //window pixel postion
    Vector2 mousePos = GetMousePosition();

    //window pixles -> virtual viewport coordinates
    Vector2 virtualMouse = { mousePos.x/virtualScale.x , mousePos.y/virtualScale.y };

    //veiwport -> world coordinates
    return GetScreenToWorld2D(virtualMouse, camera);
}

struct InputSystem {
    World &world;
    Camera2D& camera; 
    Vector2 VirtualScale; //scale of monitiorSize / VirtualSize

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
            Vector2 mousePos = getMouseWorldPosition(camera, VirtualScale);
            Position playerPos = world.getStore<Position>().get(e.id);
            Texture2D bulletTexture = world.textureManager.get("bullet.png");
            Direction dir = calculateDirectionVec({ playerPos.x + 32, playerPos.y + 32 }, { mousePos.x, mousePos.y });
            projectileFactory(world, {
             .texture = bulletTexture,
             .owner = { e.id },
             .pos = { playerPos.x + 32, playerPos.y + 32 },
             .direction = dir,
             .speed = { 10 },
             .damage = { 10 },
         });
        }
    }
};
