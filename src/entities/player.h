#pragma once

#include "raylib.h"

#include "shared/components.h"
#include "ecs/world.h"

struct PlayerConfig {
    Texture2D &texture;
    Position pos = { (int)WINDOW_WIDTH/2, (int)WINDOW_HEIGHT/2 };
    Size size = { 64, 64 };
    Direction direction = { 0.f, 0.f };
    Speed speed = { 0 };
    Health health = { 100 };

};

inline Entity playerFactory(
    World &world,
    PlayerConfig config)
{
    Entity player = world.entities.create();
    world.attach(player, world.positions, config.pos);
    world.attach(player, world.renderables, Renderable{config.texture});
    world.attach(player, world.sizes, config.size);
    world.attach(player, world.directions, config.direction);
    world.attach(player, world.speeds, config.speed);
    return player;
};
