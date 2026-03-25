#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "components/components.h"
#include "entities/projectile.h"
#include "ecs/world.h"
#include "lib/utils.h"

inline Vector2 calculateDirectionVec(Position origin, Position target){
    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    float len = std::sqrt(dx*dx + dy*dy);
    return { dx/len, dy/len };
};

inline Vector2 getMouseWorldPosition(const Camera2D& camera, const Vector2& virtualScale) {
    //window pixel position
    auto [x, y] = GetMousePosition();
    //window pixels -> virtual viewport coordinates
    Vector2 virtualMouse = { x/virtualScale.x , y/virtualScale.y };
    //viewport -> world coordinates
    return GetScreenToWorld2D(virtualMouse, camera);
}

inline Vector2 getMouseVirtualPosition(const Camera2D& camera, const Vector2& virtualScale) {
    //window pixel position
    auto [x, y] = GetMousePosition();
    //window pixels -> virtual viewport coordinates
    return { x/virtualScale.x , y/virtualScale.y };
}

inline void handleMovement(World& world, Entity& e) {
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
}

inline void handleMouseLeftClick(World& world, Entity& e, const Vector2& wMouse) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Position playerPos = world.getStore<Position>().get(e.id);
        Texture2D bulletTexture = world.textureManager.get("bullet.png");
        Vector2 dir = calculateDirectionVec({ playerPos.x, playerPos.y}, { wMouse.x, wMouse.y});
        projectileFactory(world, {
         .texture = bulletTexture,
         .owner = { e.id },
         .pos = { playerPos.x, playerPos.y},
         .direction = { dir.x, dir.y },
         .speed = { 10 },
         .damage = { 10 },
         .hitBox = { 16.0f, 8.0f }
     });
    }
}

inline void handleCameraMovement(Camera2D& camera, const Vector2& vMouse) {
    if (!IsCursorOnScreen()) { return; }
    const float mvSpd = 10.0f;

    //arrow keys
    if (IsKeyDown(KEY_RIGHT)) camera.target.x += mvSpd;
    if (IsKeyDown(KEY_LEFT)) camera.target.x -= mvSpd;
    if (IsKeyDown(KEY_UP)) camera.target.y -= mvSpd;
    if (IsKeyDown(KEY_DOWN)) camera.target.y += mvSpd;

    //mouse edge pan
    const int boxSize = 5;
    bool nearTop    = vMouse.y < boxSize;
    bool nearBottom = vMouse.y > VIRTUAL_HEIGHT - boxSize;
    bool nearLeft   = vMouse.x < boxSize;
    bool nearRight  = vMouse.x > VIRTUAL_WIDTH - boxSize;

    if (nearTop)    camera.target.y -= mvSpd;
    if (nearBottom) camera.target.y += mvSpd;
    if (nearLeft)   camera.target.x -= mvSpd;
    if (nearRight)  camera.target.x += mvSpd;
}

inline void handleAbilities(World& world,Entity& e , Camera2D& camera, const Vector2& mouse) {
    ComponentStore<AbilitySet>& abilitySetStore = world.getStore<AbilitySet>();
    if (IsKeyDown(KEY_SPACE)) {
        Position pos = world.getStore<Position>().get(e.id);
        if (abilitySetStore.has(e.id)) {
            Ability& ability = abilitySetStore.get(e.id).ability[0];
            ability.dir = calculateDirectionVec({pos.x, pos.y}, {mouse.x, mouse.y});
            ability.effect(world, ability);
        }
    }
    if (IsKeyDown(KEY_ONE)) {
        Position pos = world.getStore<Position>().get(e.id);
        if (abilitySetStore.has(e.id)) {
            Ability& ability = abilitySetStore.get(e.id).ability[1];
            ability.dir = calculateDirectionVec({pos.x, pos.y}, {mouse.x, mouse.y});
            ability.effect(world, ability);
        }
    }
}

struct InputSystem {
    World &world;
    Camera2D& camera; 
    Vector2 VirtualScale; //scale of monitiorSize / VirtualSize
    void processInput(Entity e) {
        Vector2 mouse = getMouseWorldPosition(camera, VirtualScale);
        Vector2 vMouse = getMouseVirtualPosition(camera, VirtualScale);

        handleMovement(world, e);
        handleCameraMovement(camera, vMouse);
        handleMouseLeftClick(world, e, { mouse.x, mouse.y });
        handleAbilities(world, e,camera, mouse);
    }
};
