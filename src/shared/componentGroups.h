#pragma once
#include <vector>

#include "components.h"
#include "ecs/world.h"

inline bool hasPysics(int id, World& world) {
    return world.directions.has(id)
                && world.positions.has(id)
                && world.speeds.has(id);
};

inline bool collidable(int id, World& world) {
    return world.positions.has(id) && world.sizes.has(id);
};
