#pragma once

#include <cmath>
#include "ecs/world.h"
#include "components/components.h"
#include "components/entity.h"
#include "lib/utils.h"

inline void dash(World& world, Ability& self) {
    if (self.cdProg > 0) { return; }

    Entity& parent = world.entities.get(self.parent);
    int speed = self.distance * TICK_RATE / self.duration;
    world.attach(parent, world.getStore<VelocityOveride>(), {self.dir.x, self.dir.y, speed });

    Entity dashEntity = world.entities.create();
    int parentId = parent.id;
    self.cdProg = self.cd;
    world.attach(dashEntity, world.getStore<LifeTime>(), { 
        .remaining = self.duration, 
        .contextId = parent.id,
        .onExpire = [](World& world, LifeTime& lifeTime) {
            world.getStore<VelocityOveride>().remove(lifeTime.contextId);
        }} 
    ); 
}
