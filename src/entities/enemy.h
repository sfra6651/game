#pragma once

#include "raylib.h"

#include "shared/components.h"
#include "ecs/world.h"

struct EnemyConfig {
    Texture2D &texture;
    Position pos = { (int)VIRTUAL_WIDTH/2, (int)VIRTUAL_HEIGHT/2 };
    Size size = { 64, 64 };
    Direction direction = { 0.f, 0.f };
    Speed speed = { 0 };
    Health health = { 100 };
};

inline Entity enemyFactory(
    World &world,
    EnemyConfig config)
{
    const Entity enemy = world.entities.create();
    world.attach(enemy, world.getStore<Position>(), config.pos);
    world.attach(enemy, world.getStore<Renderable>(), Renderable{config.texture});
    world.attach(enemy, world.getStore<Size>(), config.size);
    world.attach(enemy, world.getStore<Direction>(), config.direction);
    world.attach(enemy, world.getStore<Speed>(), config.speed);
    world.attach(enemy, world.getStore<Health>(), config.health);
    return enemy;
};
