#pragma once

#include <cmath>

#include "components/components.h"
#include "iostream"

#include "ecs/world.h"
#include "components/componentGroups.h"

struct QuadTree {
      Rectangle bounds;
      std::vector<std::pair<int, Rectangle>> entities;
      std::unique_ptr<QuadTree> children[4];
      int maxEntities = 8;
      int maxDepth = 5;
      int depth = 0;
      bool subdivided = false;

      QuadTree(Rectangle bounds, int maxEntities = 8, int maxDepth = 5, int depth = 0)
      : bounds(bounds),
      maxEntities(maxEntities),
      maxDepth(maxDepth),
      depth(depth)
      {}

      void subdivide() {
          if (subdivided) return;
          const float hw = bounds.width / 2;
          const float hh = bounds.height / 2;
          children[0] = std::make_unique<QuadTree>(Rectangle{bounds.x, bounds.y, hw, hh}, maxEntities, maxDepth, depth + 1);
          children[1] = std::make_unique<QuadTree>(Rectangle{bounds.x + hw, bounds.y,hw,hh}, maxEntities, maxDepth, depth + 1);
          children[2] = std::make_unique<QuadTree>(Rectangle{bounds.x, bounds.y + hh,hw,hh}, maxEntities, maxDepth, depth + 1);
          children[3] = std::make_unique<QuadTree>(Rectangle{bounds.x + hw, bounds.y + hh,hw,hh}, maxEntities, maxDepth, depth + 1);
          subdivided = true;
      }

      void insert(int entityId, Rectangle rect) {
          if (!CheckCollisionRecs(bounds, rect)) return;

          if (!subdivided && (entities.size() < maxEntities || depth >= maxDepth)) {
              entities.emplace_back(entityId, rect);
              return;
          }

          subdivide();

          // Re-insert existing entities into children
          for (auto& [id, r] : entities) {
              for (const auto& child : children) {
                  child->insert(id, r);
              }
          }
          entities.clear();

          // Insert the new entity into children
          for (const auto& child : children) {
              child->insert(entityId, rect);
          }
      }

      void query(const Rectangle area, std::vector<int>& found) {
          if (!CheckCollisionRecs(bounds, area)) return;

          for (auto& [id, r] : entities) {
              found.push_back(id);
          }

          if (subdivided) {
              for (const auto& child : children) {
                  child->query(area, found);
              }
          }
      }

      void clear() {
          entities.clear();
          if (subdivided) {
              for (auto& child : children) {
                  child.reset();
              }
              subdivided = false;
          }
      }
  };

struct CollisionSystem {
    World &world;
    std::vector<QuadTree> quadTrees;
    std::vector<int> removals {};

    void processCollisions() {
        removals.clear();
        QuadTree tree{Rectangle{0, 0, WORLD_WIDTH, WORLD_HEIGHT}};
        for (int i = 0; world.entities.count > i; i++) {
            const int eId = world.entities.list[i].id;
            if (eId == REMOVED_ENTITY_ID) { continue; }
            if (!collidable(eId, world)) {
                continue;
            }
            Position pos = world.getStore<Position>().get(eId);
            HitBox& hitBox = world.getStore<HitBox>().get(eId);
            tree.insert(eId, Rectangle{pos.x - (int)(hitBox.width/2), pos.y - (int)(hitBox.height/2), hitBox.width, hitBox.height});

        }

        // 2. For each entity, query the tree instead of all entities
        for (int i = 0; i < world.entities.count; i++) {
            const int eId = world.entities.list[i].id;
            if (eId == REMOVED_ENTITY_ID) continue;
            if (!collidable(eId, world)) continue;

            Position pos = world.getStore<Position>().get(eId);
            HitBox hitBox = world.getStore<HitBox>().get(eId);
            Rectangle rect{pos.x - (int)(hitBox.width/2),pos.y - (int)(hitBox.height/2), hitBox.width, hitBox.height};

            std::vector<int> nearby{};
            tree.query(rect, nearby);   // only entities in same region

            for (const int other_id : nearby) {
                if (other_id == eId) continue;
                HitBox& otherHb = world.getStore<HitBox>().get(other_id);
                Position otherPos = world.getStore<Position>().get(other_id);
                if (CheckCollisionRecs(rect,{
                    otherPos.x - (int)(otherHb.width/2),
                    otherPos.y - (int)(otherHb.height/2),
                    otherHb.width,
                    otherHb.height,
                })) {
                    handleCollision(eId, other_id);
                    break;
                }
            }
        }

        for (const auto& id : removals) {
            world.erase({id});
        }
    }

    void handleCollision(int e_id, int other_e_id) {
        ComponentStore<Damage>& damageStore = world.getStore<Damage>();
        ComponentStore<Owner>& ownerStore = world.getStore<Owner>();
        ComponentStore<CollisionBehavour>& behavourStore = world.getStore<CollisionBehavour>();

        if (damageStore.has(e_id) && behavourStore.get(e_id).type == DEALS_DAMAGE) {
             resolveDamage(e_id, other_e_id);           
        }
        if (behavourStore.get(e_id).destroyOnCollide == true) {
            removals.push_back(e_id);
        }

    }

    void resolveDamage(int e_id, int target_id) {
        world.getStore<Health>().get(target_id).current -= world.getStore<Damage>().get(e_id).v;
        if (world.getStore<Health>().get(target_id).current <= 0) {
            removals.push_back(target_id);
        }
    }
};


