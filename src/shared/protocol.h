#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "ecs/world.h"
#include "shared/components.h"
#include "shared/tcp.h"

constexpr int HAS_POSITION  = 1 << 0;  // = 1
constexpr int HAS_DIRECTION = 1 << 1;  // = 2
constexpr int HAS_SPEED     = 1 << 2;  // = 4
constexpr int HAS_SIZE      = 1 << 3;  // = 8

enum ProtocolInstruction {
    PROCCESS_NEXT_FRAME,
    WAIT,
};

struct EntitySnapshot {
    int id;
    int components; // bit 0  = pos, bi 1 = dir, bit 2 = speed, bit 3 = size
    Position position;
    Direction direction;
    Speed speed;
    Size size;
};

struct WorldSnap {
    std::vector<Entity> entities;
    std::unordered_map<int, Position> positions;
    std::unordered_map<int, Size> sizes;
    std::unordered_map<int, Direction> directions;
    std::unordered_map<int, Speed> speeds;

    void clear(){
        entities.clear();
        positions.clear();
        sizes.clear();
        directions.clear();
        speeds.clear();
    };
};

struct DebugProtocol {
    ProtocolInstruction instruction{};
    std::vector<EntitySnapshot> entitySnaps{};

    bool sendTo(TcpClient &client){
        int count = entitySnaps.size(); 
        return client.send(&instruction, sizeof(instruction))
            && client.send(&count, sizeof(count))
            && client.send(entitySnaps.data(), count * sizeof(EntitySnapshot));
    };

    bool recvFrom(TcpServer &server){
        int count {};
        if(!server.recv(&instruction, sizeof(instruction))) { return false; };
        if(!server.recv(&count, sizeof(count))) { return false; };
        entitySnaps.resize(count);
        return server.recv(entitySnaps.data(), count * sizeof(EntitySnapshot));
    }

    void createEntitySnaps(World &world)
    {
        entitySnaps.clear();
        for (int i = 0; i < world.entities.freeIndex; i++) {
            EntitySnapshot snap {}; 
            int id = world.entities.list[i].id;
            snap.id = id;
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
                snap.components |= HAS_SIZE;
                snap.size = world.sizes[id];
            };
            entitySnaps.push_back(snap);
        };
    }
    
    void parseEntitySnaps(WorldSnap &worldSnap)
    {
        worldSnap.clear();
        for (const auto& entitySnap : entitySnaps) {
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
            worldSnap.entities.push_back({.id = entitySnap.id});
        };
    }
};
