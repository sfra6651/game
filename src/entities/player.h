#pragma once

#include "raylib.h"

#include "components/components.h"
#include "ecs/world.h"

struct PlayerConfig {
    //Ability abilities[5] {};
    Texture2D &texture;
    Position pos = { WORLD_WIDTH/2.0f, WORLD_HEIGHT/2.0f };
    Size size = { 64, 64 };
    Direction direction = { 0.f, 0.f };
    Speed speed = { 0 };
    Health health = { 100, 100 };
    HitBox hitBox = {};
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
    world.attach(player, world.getStore<Health>(), config.health);
    world.attach(player, world.getStore<HitBox>(), config.hitBox);
    //for (int i = 0; i < 5; i++) {
    //    if (config.abilities[i].effect) {
    //        Entity abilityEntity = world.entities.create();
    //        world.attach(abilityEntity, world.getStore<Ability>(), config.abilities[i]);
    //    }
    //}
    return player;
};
