#pragma once

#include "raylib.h"

#include "ecs/entity.h"
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
    Entity player = world.entities.create();
    world.attach(player, world.positions, config.pos);
    world.attach(player, world.renderables, Renderable{config.texture});
    world.attach(player, world.sizes, config.size);
    world.attach(player, world.directions, config.direction);
    world.attach(player, world.speeds, config.speed);
    return player;
};
