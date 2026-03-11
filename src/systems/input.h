#pragma once

#include "raylib.h"

#include "ecs/entity.h"
#include "ecs/world.h"


inline void proccessInput(World &w, Entity &e) {
    if (IsKeyDown(KEY_W)) w.velocities[e.id].y = -4;
    if (IsKeyDown(KEY_S)) w.velocities[e.id].y = 4;
    if (IsKeyDown(KEY_A)) w.velocities[e.id].x = -4;
    if (IsKeyDown(KEY_D)) w.velocities[e.id].x = 4;

    if (IsKeyReleased(KEY_W)) w.velocities[e.id].y = 0;
    if (IsKeyReleased(KEY_S)) w.velocities[e.id].y = 0;
    if (IsKeyReleased(KEY_A)) w.velocities[e.id].x = 0;
    if (IsKeyReleased(KEY_D)) w.velocities[e.id].x = 0;
};
