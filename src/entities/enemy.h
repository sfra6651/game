#pragma once

#include "raylib.h"

#include "components/components.h"
#include "ecs/world.h"


struct HealthBarConfig {
    Texture2D& texture;
    Owner owner;
    Position pos = { 0.0f, 0.0f };
    AnchorPoint anchorPoint = { -40, -40}; 
    Size size = { 88, 20 };
    UiElement uiElement = {HEALTH_BAR ,true};
};

struct EnemyBuilder {
    Texture2D &texture;
    Position pos = { WORLD_WIDTH/2.0f, WORLD_HEIGHT/2.0f };
    Size size = { 64, 64 };
    Direction direction = { 0.f, 0.f };
    Speed speed = { 0 };
    Health health = { 100, 100 };
    HitBox hitBox = {};
    CollisionBehavour collisionBehavour;

    Entity& attachToWorld(World& world) {
        Entity enemy  = world.entities.create();
        world.attach(enemy, world.getStore<Position>(), pos);
        world.attach(enemy, world.getStore<Renderable>(), { texture });
        world.attach(enemy, world.getStore<Size>(), size);
        world.attach(enemy, world.getStore<Direction>(), direction);
        world.attach(enemy, world.getStore<Speed>(), speed);
        world.attach(enemy, world.getStore<Health>(), health);
        world.attach(enemy, world.getStore<HitBox>(), hitBox);
        world.attach(enemy, world.getStore<CollisionBehavour>(), collisionBehavour);

        return world.entities.get(enemy.id);

    }

    void attachHealthBar(World& world, HealthBarConfig config) {
        Entity healthBar = world.entities.create();
         
        world.attach(healthBar, world.getStore<Renderable>(), Renderable{config.texture});
        world.attach(healthBar, world.getStore<Position>(), config.pos);
        world.attach(healthBar, world.getStore<AnchorPoint>(), config.anchorPoint);
        world.attach(healthBar, world.getStore<Size>(), config.size);
        world.attach(healthBar, world.getStore<Owner>(), config.owner);
        world.attach(healthBar, world.getStore<UiElement>(), config.uiElement);
    }
};
