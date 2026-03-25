#pragma once

#include <cmath>

#include "components/components.h"
#include "iostream"

#include "ecs/world.h"
#include "components/componentGroups.h"

  // Helper: get the 4 corners of a rotated rectangle
inline void getRotatedCorners(Vector2 center, float w, float h, float rotDeg, Vector2 out[4]) {
      float rad = rotDeg * DEG2RAD;
      float c = cosf(rad);
      float s = sinf(rad);
      float hw = w / 2.0f;
      float hh = h / 2.0f;
      out[0] = { center.x + c*(-hw) - s*(-hh), center.y + s*(-hw) + c*(-hh) };
      out[1] = { center.x + c*( hw) - s*(-hh), center.y + s*( hw) + c*(-hh) };
      out[2] = { center.x + c*( hw) - s*( hh), center.y + s*( hw) + c*( hh) };
      out[3] = { center.x + c*(-hw) - s*( hh), center.y + s*(-hw) + c*( hh) };
}

  // Helper: project all corners onto an axis, return min and max
inline void projectOntoAxis(Vector2 corners[4], Vector2 axis, float& min, float& max) {
      min = max = corners[0].x * axis.x + corners[0].y * axis.y;
      for (int i = 1; i < 4; i++) {
          float proj = corners[i].x * axis.x + corners[i].y * axis.y;
          if (proj < min) min = proj;
          if (proj > max) max = proj;
      }
}

  // SAT check: do two rotated rectangles overlap?
inline bool checkRotatedCollision(Vector2 posA, float wA, float hA, float rotA,
    Vector2 posB, float wB, float hB, float rotB)
{
      Vector2 cornersA[4], cornersB[4];
      getRotatedCorners(posA, wA, hA, rotA, cornersA);
      getRotatedCorners(posB, wB, hB, rotB, cornersB);

      // 4 axes: 2 edge normals from each rectangle
      Vector2 axes[4];
      // Edges of rect A
      axes[0] = { cornersA[1].x - cornersA[0].x, cornersA[1].y - cornersA[0].y };
      axes[1] = { cornersA[1].x - cornersA[2].x, cornersA[1].y - cornersA[2].y };
      // Edges of rect B
      axes[2] = { cornersB[1].x - cornersB[0].x, cornersB[1].y - cornersB[0].y };
      axes[3] = { cornersB[1].x - cornersB[2].x, cornersB[1].y - cornersB[2].y };

      for (int i = 0; i < 4; i++) {
          float minA, maxA, minB, maxB;
          projectOntoAxis(cornersA, axes[i], minA, maxA);
          projectOntoAxis(cornersB, axes[i], minB, maxB);
          // If there's a gap on this axis, no collision
          if (maxA < minB || maxB < minA) return false;
      }
      return true;
}

// Compute AABB that encloses a rotated rectangle (for broad-phase)
inline Rectangle getRotatedAABB(Vector2 center, float w, float h, float rotDeg) {
    Vector2 corners[4];
    getRotatedCorners(center, w, h, rotDeg, corners);
    float minX = corners[0].x, maxX = corners[0].x;
    float minY = corners[0].y, maxY = corners[0].y;
    for (int i = 1; i < 4; i++) {
        if (corners[i].x < minX) minX = corners[i].x;
        if (corners[i].x > maxX) maxX = corners[i].x;
        if (corners[i].y < minY) minY = corners[i].y;
        if (corners[i].y > maxY) maxY = corners[i].y;
    }
    return { minX, minY, maxX - minX, maxY - minY };
}

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
        for (int i = 0; world.entityManager.count > i; i++) {
            const int eId = world.entityManager.list[i].id;
            if (eId == REMOVED_ENTITY_ID) { continue; }
            if (!collidable(eId, world)) {
                continue;
            }
            Position pos = world.getStore<Position>().get(eId);
            HitBox& hitBox = world.getStore<HitBox>().get(eId);
            tree.insert(eId, getRotatedAABB({pos.x, pos.y}, hitBox.width, hitBox.height, pos.rt));

        }

        // 2. For each entity, query the tree instead of all entities
        for (int i = 0; i < world.entityManager.count; i++) {
            const int eId = world.entityManager.list[i].id;
            if (eId == REMOVED_ENTITY_ID) continue;
            if (!collidable(eId, world)) continue;

            Position pos = world.getStore<Position>().get(eId);
            HitBox hitBox = world.getStore<HitBox>().get(eId);
            Rectangle rect = getRotatedAABB({pos.x, pos.y}, hitBox.width, hitBox.height, pos.rt);

            std::vector<int> nearby{};
            tree.query(rect, nearby);   // broad phase: AABB query

            for (const int other_id : nearby) {
                if (other_id == eId) continue;
                HitBox& otherHb = world.getStore<HitBox>().get(other_id);
                Position otherPos = world.getStore<Position>().get(other_id);
                // narrow phase: precise rotated collision
                if (checkRotatedCollision(
                    {pos.x, pos.y}, hitBox.width, hitBox.height, pos.rt,
                    {otherPos.x, otherPos.y}, otherHb.width, otherHb.height, otherPos.rt))
                {
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


        if (!damageStore.has(e_id)) {
            return;
        }

        if (behavourStore.get(e_id).type == DEALS_DAMAGE) {
            resolveDamage(e_id, other_e_id);           
        }

        if (behavourStore.get(e_id).destroyOnCollide == true) {
            removals.push_back(e_id);
        }

    }

    void resolveDamage(int e_id, int target_id) {
        ComponentStore<HasDamaged>& hasDamgedStore = world.getStore<HasDamaged>();

        if(hasDamgedStore.has(e_id)) {
            std::vector<int>& ids = hasDamgedStore.get(e_id).ids;
            if (std::find(ids.begin(), ids.end(), target_id) != ids.end()) {
                //exists
                return;
            }
            ids.push_back(target_id);
        }
        hasDamgedStore.add(e_id, {.ids = { target_id } });

        world.getStore<Health>().get(target_id).current -= world.getStore<Damage>().get(e_id).v;
        if (world.getStore<Health>().get(target_id).current <= 0) {
            removals.push_back(target_id);
        }
    }
};


