#pragma once

#include <cmath>

#include "components/components.h"
#include "components/componentGroups.h"
#include "ecs/world.h"
#include "lib/utils.h"

struct PhysicsSystem {
    World &world;

   void processPhysics(){
        ComponentStore<Direction>& directions = world.getStore<Direction>();
        ComponentStore<Position>& positions = world.getStore<Position>();
        ComponentStore<Speed>& speeds = world.getStore<Speed>();
        ComponentStore<AnchorPoint>& anchorPoints = world.getStore<AnchorPoint>();
        ComponentStore<Owner>& owners = world.getStore<Owner>();
        ComponentStore<VelocityOveride>& velocityOverides = world.getStore<VelocityOveride>();

        int count = world.entityManager.count;
        std::vector<int> removals {};

        for (int i = 0; i < count; i++) {
            int e_id = world.entityManager.list[i].id;
            //guard
            if (e_id == REMOVED_ENTITY_ID
                || !world.getStore<Direction>().has(e_id)
                || !hasPhysics(e_id, world)) {
                continue;
            }

            Direction dir = directions.get(e_id);
            Position pos = positions.get(e_id);
            Speed speed = speeds.get(e_id);

            Position new_pos {};
            if (velocityOverides.has(e_id)) {
                auto [x, y, speed] = velocityOverides.get(e_id);
                new_pos = {pos.x + x * speed, pos.y + y * speed };
            } else {
                new_pos = {pos.x + dir.x * speed.v, pos.y + dir.y * speed.v};
            }

            //remove on leaving world TODO: add collision or something for this
            if (std::abs(new_pos.x) >= WORLD_WIDTH ||
                std::abs(new_pos.y) >= WORLD_HEIGHT* 2) {
                removals.push_back(i);
                continue;
            }
            world.getStore<Position>().get(e_id) = new_pos;


            ///if (world.getStore<HitBox>().has(e_id)) {
            ////    world.getStore<HitBox>().get(e_id).rect.x = new_pos.x;
            ////    world.getStore<HitBox>().get(e_id).rect.y = new_pos.y;
            ////}

        }

        //update entities with anchor points after main loop so we can guareenty parent position is up to date
        for (int i = 0; i < count; i++) {
            Entity& e = world.entityManager.list[i];
            if (!anchorPoints.has(e.id) ||
                !positions.has(e.id) ||
                !owners.has(e.id)
            ){
                continue;
            };
            AnchorPoint& anchorPoint = anchorPoints.get(e.id);
            Owner parent = owners.get(e.id);
            if (!positions.has(parent.id)) {
                logErr("Anchor point parent must have a postion. parent id: ", parent.id);
                continue;
            }
            positions.get(e.id) = { 
                .x = positions.get(parent.id).x + anchorPoint.x,
                .y = positions.get(parent.id).y + anchorPoint.y
            };
        }

        //process removals
        for (auto id : removals) {
            world.erase(world.entityManager.list[id]);
        }
    }
};


