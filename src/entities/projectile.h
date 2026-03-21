#pragma once

#include "raylib.h"

#include "shared/components.h"
#include "ecs/world.h"

struct ProjectileConfig {
    Texture2D &texture;
    Owner owner {};
    Position pos = {WORLD_WIDTH/2.0f, WORLD_HEIGHT/2.0f};
    Size size = {16, 8};
    Direction direction = { 0, 0 };
    Speed speed = { 0 };
    Damage damage = { 10 };
};

inline Entity projectileFactory (
    World &world,
    ProjectileConfig config)
{
    const Entity projectile = world.entities.create();

    world.attach(projectile, world.getStore<Owner>(), config.owner);
    world.attach(projectile, world.getStore<Position>(), config.pos);
    world.attach(projectile, world.getStore<Renderable>(), Renderable{config.texture});
    world.attach(projectile, world.getStore<Size>(), config.size);
    world.attach(projectile, world.getStore<Direction>(), config.direction);
    world.attach(projectile, world.getStore<Speed>(), config.speed);
    world.attach(projectile, world.getStore<Damage>(), config.damage);
    return projectile;
};
