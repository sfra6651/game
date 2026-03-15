#pragma once

#include "raylib.h"

#include "shared/components.h"
#include "ecs/world.h"

struct EnemyConfig {
    Texture2D &texture;
    Position pos = { (int)WINDOW_WIDTH/2, (int)WINDOW_HEIGHT/2 };
    Size size = { 64, 64 };
    Direction direction = { 0.f, 0.f };
    Speed speed = { 0 };
};

inline Entity enemyFactory(
    World &world,
    EnemyConfig config)
{
    Entity enemy = world.entities.create();
    world.attach(enemy, world.positions, config.pos);
    world.attach(enemy, world.renderables, Renderable{config.texture});
    world.attach(enemy, world.sizes, config.size);
    world.attach(enemy, world.directions, config.direction);
    world.attach(enemy, world.speeds, config.speed);
    return enemy;
};
