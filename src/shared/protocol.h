#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "ecs/world.h"
#include "shared/components.h"

constexpr int HAS_POSITION  = 1 << 0;  // = 1
constexpr int HAS_DIRECTION = 1 << 1;  // = 2
constexpr int HAS_SPEED     = 1 << 2;  // = 4
constexpr int HAS_SIZE      = 1 << 3;  // = 8

struct EntitySnapshot {
    int id;
    int components; // bit 0  = pos, bi 1 = dir, bit 2 = speed, bit 3 = size
    Position position;
    Direction direction;
    Speed speed;
    Size size;
};

struct WorldSnap {
    Entities entities{};
    std::unordered_map<int, Position> positions;
    std::unordered_map<int, Size> sizes;
    std::unordered_map<int, Direction> directions;
    std::unordered_map<int, Speed> speeds;
};

inline bool createEntitiesSnap(World &world, std::vector<EntitySnapshot> &entitiesSnap)
{
    for (int i = 0; i < world.entities.freeIndex; i++) {
        EntitySnapshot snap {}; 
        int id = world.entities.list[i].id;
        if (world.positions.count(id)) {
            snap.components |= HAS_POSITION;
            snap.position = world.positions[id];
        };
        if (world.directions.count(id)) {
            snap.components |= HAS_DIRECTION;
            snap.direction = world.directions[id];
        };
        if (world.speeds.count(id)) {
            snap.components |= HAS_SPEED;
            snap.speed = world.speeds[id];
        };
        if (world.sizes.count(id)) {
            snap.components = HAS_SIZE;
            snap.size = world.sizes[id];
        };
        entitiesSnap.push_back(snap);
    };
    return true;
}

inline bool parseEntiteisSnap(std::vector<EntitySnapshot> &entitiesSnap, WorldSnap &worldSnap)
{
    for (auto& entitySnap : entitiesSnap) {
        if (entitySnap.components & HAS_POSITION) {
            worldSnap.positions[entitySnap.id] = entitySnap.position;
        };
        if (entitySnap.components & HAS_DIRECTION) {
            worldSnap.directions[entitySnap.id] = entitySnap.direction;
        };
        if (entitySnap.components & HAS_SPEED) {
            worldSnap.speeds[entitySnap.id] = entitySnap.speed;
        };
        if (entitySnap.components & HAS_SIZE) {
            worldSnap.sizes[entitySnap.id] = entitySnap.size;
        };
    };
    return true;
}
