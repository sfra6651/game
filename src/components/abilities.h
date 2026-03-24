#pragma once

#include "ecs/world.h"
#include "components/components.h"
#include "components/entity.h"

inline void dash(World& world, Ability& self) {
    if (self.cdProg > 0) { return; }
    
    Entity& parent = world.entities.get(self.parent);
    Entity dashEntity = world.entities.create();
    int parentId = parent.id;
    self.cdProg = self.cd;
    world.attach(parent, world.getStore<VelocityOveride>(), {self.dir.x, self.dir.y, 10});
    world.attach(dashEntity, world.getStore<LifeTime>(), { 
        .remaining = self.duration, 
        .contextId = parent.id,
        .onExpire = [](World& world, LifeTime& lifeTime) {
            world.getStore<VelocityOveride>().remove(lifeTime.contextId);
        }} 
    ); 
}
