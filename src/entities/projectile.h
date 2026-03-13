#pragma once

#include "raylib.h"

#include "shared/components.h"
#include "ecs/world.h"

struct ProjectileConfig {
    Texture2D &texture;
    Position pos = {(int)WINDOW_WIDTH/2, (int)WINDOW_HEIGHT/2};
    Size size = {16, 8};
    Direction direction = { 0, 0 };
    Speed speed = { 0 };

};

inline Entity projectileFactory (
    World &world,
    ProjectileConfig config)
{
    Entity projectile = world.entities.create();
    world.attach(projectile, world.positions, config.pos);
    world.attach(projectile, world.renderables, Renderable{config.texture});
    world.attach(projectile, world.sizes, config.size);
    world.attach(projectile, world.directions, config.direction);
    world.attach(projectile, world.speeds, config.speed);
    return projectile;
};
