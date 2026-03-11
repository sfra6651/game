#define pragma once

#include "raylib.h"

#include "ecs/entity.h"
#include "ecs/world.h"

struct ProjectileConfig {
    Texture2D texture{};
    Position pos{}; 
    Velocity velocity{};
    Entity owner{};
};

inline Entity projectileFactory(ProjectileConfig config, World &world) {
    Entity projectile = world.entities.create();
    world.attach(projectile, world.renderables, Renderable {config.texture});
    world.attach(projectile, world.velocities, Velocity {config.velocity.x, config.velocity.y});
    world.attach(projectile, world.positions, Position {config.pos.x, config.pos.y});

    return projectile;
};
