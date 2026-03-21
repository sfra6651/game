#pragma once

#include "raylib.h"

#include "shared/components.h"
#include "ecs/world.h"

struct PlayerConfig {
    Texture2D &texture;
    Position pos = { (int)VIRTUAL_WIDTH/2, (int)VIRTUAL_HEIGHT/2 };
    Size size = { 64, 64 };
    Direction direction = { 0.f, 0.f };
    Speed speed = { 0 };
    Health health = { 100 };

};

inline Entity playerFactory(
    World &world,
    PlayerConfig config)
{
    const Entity player = world.entities.create();
    world.attach(player, world.getStore<Position>(), config.pos);
    world.attach(player, world.getStore<Renderable>(), Renderable{config.texture});
    world.attach(player, world.getStore<Size>(), config.size);
    world.attach(player, world.getStore<Direction>(), config.direction);
    world.attach(player, world.getStore<Speed>(), config.speed);
    return player;
};
