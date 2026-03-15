#pragma once
#include <vector>

#include "components.h"
#include "ecs/world.h"

inline bool hasPysics(int id, World& world) {
    return world.directions.has(id)
                && world.renderables.has(id)
                && world.sizes.has(id)
                && world.positions.has(id)
                && world.speeds.has(id);
};
