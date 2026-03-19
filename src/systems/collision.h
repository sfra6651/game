#pragma once

#include <cmath>

#include "iostream"

#include "ecs/world.h"
#include "shared/componentGroups.h"

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

    void x() {

    };

    void processCollisions() {
        removals.clear();
        QuadTree tree{Rectangle{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}};
        for (int i = 0; world.entities.count > i; i++) {
            const int eId = world.entities.list[i].id;
            if (eId == REMOVED_ENTITY_ID) { continue; }
            if (!collidable(eId, world)) {
                continue;
            }
            Position pos = world.getStore<Position>().get(eId);
            Size size = world.getStore<Size>().get(eId);
            tree.insert(eId, Rectangle{(float)pos.x, (float)pos.y,
                                       (float)size.width, (float)size.height});

        }

        // 2. For each entity, query the tree instead of all entities
        for (int i = 0; i < world.entities.count; i++) {
            const int eId = world.entities.list[i].id;
            if (eId == REMOVED_ENTITY_ID) continue;
            if (!collidable(eId, world)) continue;

            Position pos = world.getStore<Position>().get(eId);
            Size size = world.getStore<Size>().get(eId);
            Rectangle rect{(float)pos.x, (float)pos.y,
                            (float)size.width, (float)size.height};

            std::vector<int> nearby{};
            tree.query(rect, nearby);   // only entities in same region

            for (const int other_id : nearby) {
                if (other_id == eId) continue;
                if (world.getStore<Owner>().has(other_id)) continue;

                Rectangle otherRect{(float)world.getStore<Position>().get(other_id).x,
                                     (float)world.getStore<Position>().get(other_id).y,
                                     (float)world.getStore<Size>().get(other_id).width,
                                     (float)world.getStore<Size>().get(other_id).height};

                if (CheckCollisionRecs(rect, otherRect)) {
                    handleCollision(eId, other_id);
                    break;
                }
            }
        }

        for (const auto& id : removals) {
            world.erase({id});
        }
    }

    void handleCollision(int e_id, int collision_e_id) {
        if (world.getStore<Damage>().has(e_id) && world.getStore<Health>().has(collision_e_id)) {
            resolveDamage(e_id, collision_e_id);
        }
    }

    void resolveDamage(int damaging_id, int target_id) {
        world.getStore<Health>().get(target_id).v -= world.getStore<Damage>().get(damaging_id).v;
        if (world.getStore<Health>().get(target_id).v <= 0) {
            removals.push_back(target_id);
        }
        removals.push_back(damaging_id);
    }
};


