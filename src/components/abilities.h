#pragma once

#include "ecs/world.h"
#include "components/components.h"
#include "components/entity.h"

inline void dash(World& world, Ability& self) {
    Entity entity = world.entities.create();
    //world.attach(self.owner, world.getStore<VelocityOveride>(), {});
//    world.attach(entity, world.getStore<LifeTime>(), { 
//        .remaining = duration, 
//        .onExpire = [](world, entity) {
//            world.entities.remove(entity.id);
//        }} 
//    ); 
}
