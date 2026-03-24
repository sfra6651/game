#pragma once

#include "components.h"
#include "ecs/world.h"
#include "lib/utils.h"

inline bool hasPhysics(int id, World& world) {
    return world.getStore<Direction>().has(id)
                && world.getStore<Position>().has(id)
                && world.getStore<Speed>().has(id);
};

inline bool collidable(int id, World& world) {
    return world.getStore<Position>().has(id) && world.getStore<HitBox>().has(id) && world.getStore<CollisionBehavour>().has(id);
};

inline bool renderable(int id, World& world) {
    return world.getStore<Renderable>().has(id) && world.getStore<Position>().has(id) && world.getStore<Size>().has(id);
}

inline bool hasLifeTime(int id, World& world) {
    return world.getStore<LifeTime>().has(id);
};
