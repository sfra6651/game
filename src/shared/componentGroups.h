#pragma once

#include "components.h"
#include "ecs/world.h"

inline bool hasPysics(int id, World& world) {
    return world.getStore<Direction>().has(id)
                && world.getStore<Position>().has(id)
                && world.getStore<Speed>().has(id);
};

inline bool collidable(int id, World& world) {
    return world.getStore<Position>().has(id) && world.getStore<Size>().has(id);
};

inline bool renderable(int id, World& world) {
    return world.getStore<Renderable>().has(id) && world.getStore<Position>().has(id) && world.getStore<Size>().has(id);
}
