#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "ecs/world.h"
#include "shared/components.h"
#include "shared/tcp.h"

constexpr int HAS_DAMAGE        = 1 << 0;  // = 0
constexpr int HAS_DIRECTION     = 1 << 1;  // = 1
constexpr int HAS_HEALTH        = 1 << 2;
constexpr int HAS_OWNER         = 1 << 3;
constexpr int HAS_POSITION      = 1 << 4;
constexpr int HAS_RENDERABLE    = 1 << 5;
constexpr int HAS_SPEED         = 1 << 6;
constexpr int HAS_SIZE          = 1 << 7;

enum ProtocolInstruction {
    PROCCESS_NEXT_FRAME,
    WAIT,
};

struct EntitySnapshot {
    int id {};
    int components {}; // bit 0  = pos, bi 1 = dir, bit 2 = speed, bit 3 = size
    Damage damage {};
    Direction direction {};
    Health health {};
    Owner owner {};
    Position position {};
    Renderable renderable {};
    Speed speed {};
    Size size {};

};

struct WorldSnap {
    std::vector<Entity> entities;
    ComponentStore<Damage> damage;
    ComponentStore<Direction> directions;
    ComponentStore<Health> healths;
    ComponentStore<Owner> owners;
    ComponentStore<Position> positions;
    ComponentStore<Renderable> renderables;
    ComponentStore<Size> sizes;
    ComponentStore<Speed> speeds;

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
        for (int i = 0; i < world.entities.count; i++) {
            EntitySnapshot snap {}; 
            int id = world.entities.list[i].id;
            snap.id = id;
            if (world.damages.has(id)) {
                snap.components |= HAS_DAMAGE;
                snap.damage = world.damages.get(id);
            }
            if (world.directions.has(id)) {
                snap.components |= HAS_DIRECTION;
                snap.direction = world.directions.get(id);
            }
            if (world.healths.has(id)) {
                snap.components |= HAS_HEALTH;
                snap.health = world.healths.get(id);
            }
            if (world.owners.has(id)) {
                snap.components |= HAS_OWNER;
                snap.owner = world.owners.get(id);
            }
            if (world.positions.has(id)) {
                snap.components |= HAS_POSITION;
                snap.position = world.positions.get(id);
            }
            if (world.renderables.has(id)) {
                snap.components |= HAS_RENDERABLE;
                snap.renderable = world.renderables.get(id);
            }
            if (world.speeds.has(id)) {
                snap.components |= HAS_SPEED;
                snap.speed = world.speeds.get(id);
            }
            if (world.sizes.has(id)) {
                snap.components |= HAS_SIZE;
                snap.size = world.sizes.get(id);
            }
            entitySnaps.push_back(snap);
        }
    }
    
    void parseEntitySnaps(WorldSnap &worldSnap)
    {
        worldSnap.clear();
        for (auto& entitySnap : entitySnaps) {
            if (entitySnap.components & HAS_DAMAGE) {
                worldSnap.damage.add(entitySnap.id, entitySnap.damage);
            }
            if (entitySnap.components & HAS_DIRECTION) {
                worldSnap.directions.add(entitySnap.id, entitySnap.direction);
            }
            if (entitySnap.components & HAS_HEALTH) {
                worldSnap.healths.add(entitySnap.id, entitySnap.health);
            }
            if (entitySnap.components & HAS_OWNER) {
                worldSnap.owners.add(entitySnap.id, entitySnap.owner);
            }
            if (entitySnap.components & HAS_POSITION) {
                worldSnap.positions.add(entitySnap.id, entitySnap.position);
            }
            if (entitySnap.components & HAS_RENDERABLE) {
                worldSnap.renderables.add(entitySnap.id, entitySnap.renderable);
            }
            if (entitySnap.components & HAS_SPEED) {
                worldSnap.speeds.add(entitySnap.id, entitySnap.speed);
            }
            if (entitySnap.components & HAS_SIZE) {
                worldSnap.sizes.add(entitySnap.id, entitySnap.size);
            }
            worldSnap.entities.push_back({.id = entitySnap.id});
        };
    }
};
