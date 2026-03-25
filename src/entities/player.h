#pragma once

#include "raylib.h"

#include "components/components.h"
#include "ecs/world.h"

struct PlayerConfig {
    Texture2D &texture;
    Position pos = { WORLD_WIDTH/2.0f, WORLD_HEIGHT/2.0f };
    Size size = { 64, 64 };
    Direction direction = { 0.f, 0.f };
    Speed speed = { 0 };
    Health health = { 100, 100 };
    HitBox hitBox = {};
};

inline Entity& playerFactory(
    World &world,
    PlayerConfig config)
{
    Entity& player = world.entityManager.create();
    world.attach(player, world.getStore<Position>(), config.pos);
    world.attach(player, world.getStore<Renderable>(), Renderable{config.texture});
    world.attach(player, world.getStore<Size>(), config.size);
    world.attach(player, world.getStore<Direction>(), config.direction);
    world.attach(player, world.getStore<Speed>(), config.speed);
    world.attach(player, world.getStore<Health>(), config.health);
    world.attach(player, world.getStore<HitBox>(), config.hitBox);
    return player;
};
