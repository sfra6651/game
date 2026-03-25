#pragma once

#include <cmath>
#include "ecs/world.h"
#include "components/components.h"
#include "components/entity.h"
#include "lib/utils.h"

inline void dash(World& world, Ability& self) {
    if (self.cdProg > 0) { return; }
    log("dashing");
    Entity& parent = world.entityManager.get(self.parent);
    int speed = self.distance * TICK_RATE / self.duration;
    world.attach(parent, world.getStore<VelocityOveride>(), {self.dir.x, self.dir.y, speed });

    Entity dashEntity = world.entityManager.create();
    int parentId = parent.id;
    self.cdProg = self.cd;
    world.attach(dashEntity, world.getStore<LifeTime>(), { 
        .contextId = parent.id,
        .remaining = self.duration, 
        .onExpire = [](World& world, LifeTime& lifeTime) {
            world.getStore<VelocityOveride>().remove(lifeTime.contextId);
        }} 
    ); 
}

inline void frontalCone(World& world, Ability& self) {
    if (self.cdProg > 0) { return; };
    log("frontaCone");
    Entity& parent = world.entityManager.get(self.parent);

    if (!world.getStore<Position>().has(parent.id)) { return; }
    Position& parentPos = world.getStore<Position>().get(parent.id);

    Entity& hitBox = world.entityManager.create();

    self.cdProg = self.cd;
    
    world.attach(hitBox, world.getStore<Damage>(),  { self.damage });
    world.attach(hitBox, world.getStore<Owner>(), { parent.id });
    world.attach(hitBox, world.getStore<Position>(), {
        .x = parentPos.x + self.dir.x * self.distance/2, //destination point (1/2 distance for center of hitbox)
        .y = parentPos.y + self.dir.y * self.distance/2,
    });
    world.attach(hitBox, world.getStore<LifeTime>(), {
        .contextId = hitBox.id,
        .remaining = self.duration,
        .onExpire = [](World& world, LifeTime& lifeTime) {
        }
    });

    //collision components
    world.attach(hitBox, world.getStore<HitBox>(), { self.distance, self.width });
    world.attach(hitBox, world.getStore<CollisionBehavour>(), {.type = DEALS_DAMAGE, .destroyOnCollide = false });

    //renderable components 
    world.attach(hitBox, world.getStore<Size>(), { (int)self.distance, (int)self.width });
    Texture2D& texture = world.textureManager.get("white_square");
    world.attach(hitBox, world.getStore<Renderable>(), { .texture = texture });
}
