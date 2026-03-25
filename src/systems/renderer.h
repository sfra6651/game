#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "components/components.h"
#include "components/componentGroups.h"
#include "entities/projectile.h"
#include "ecs/world.h"
#include "lib/utils.h"

inline void drawRectangleRotatedLines(Vector2 center, float width, float height, float rotation, Color color) {
    float rad = rotation * DEG2RAD;
    float c = cosf(rad);
    float s = sinf(rad);
    float hw = width / 2.0f;
    float hh = height / 2.0f;

    // corners relative to center, then rotated
    Vector2 corners[4] = {
        { center.x + c * (-hw) - s * (-hh), center.y + s * (-hw) + c * (-hh) },
        { center.x + c * ( hw) - s * (-hh), center.y + s * ( hw) + c * (-hh) },
        { center.x + c * ( hw) - s * ( hh), center.y + s * ( hw) + c * ( hh) },
        { center.x + c * (-hw) - s * ( hh), center.y + s * (-hw) + c * ( hh) },
    };

    for (int i = 0; i < 4; i++) {
        DrawLineV(corners[i], corners[(i + 1) % 4], color);
    }
}

inline void renderUiElement(World& world, int id) {
    const UiElement& element = world.getStore<UiElement>().get(id);
    ComponentStore<Health> healths = world.getStore<Health>();
    ComponentStore<AnchorPoint> anchorPoints = world.getStore<AnchorPoint>();
    ComponentStore<Owner> owners = world.getStore<Owner>();
    ComponentStore<Position> positions = world.getStore<Position>();
    ComponentStore<Renderable> renderables = world.getStore<Renderable>();
    ComponentStore<Size> sizes = world.getStore<Size>();
    if (!element.visible ||
        !owners.has(id) ||
        !positions.has(owners.get(id).id)
    ) {
        return;
    }

    int ownerId = owners.get(id).id;
    
    if (element.type == HEALTH_BAR &&
        healths.has(ownerId) &&
        anchorPoints.has(id)
    ) { 
        auto [offsetX, offsetY] = anchorPoints.get(id);
        auto [parentX, parentY, parentRt] = positions.get(ownerId);
        Rectangle src {
            0.0f,
            0.0f,
            (float)renderables.get(id).texture.width,
            (float)renderables.get(id).texture.height,
        };      
        Rectangle dest {
            parentX + offsetX,
            parentY + offsetY,
            (float)sizes.get(id).width * healths.get(ownerId).current / healths.get(ownerId).max,
            (float)sizes.get(id).height,
        };
        DrawTexturePro(renderables.get(id).texture, src, dest, {0, 0}, 0.0f, WHITE );
    }
}

struct RenderingSystem{
    World& world;
    Camera2D& camera;

    void drawTiles(Texture2D& srcset) {
        float camLeft = camera.target.x - VIRTUAL_WIDTH / 2.0f;
        float camTop = camera.target.y - VIRTUAL_HEIGHT / 2.0f;
        float camRight = camLeft + VIRTUAL_WIDTH;
        float camBottom = camTop + VIRTUAL_HEIGHT;

        //Convert to tile indices, + 1 to give some padding for partially visible tiles:
        int startCol = std::max(0, (int)(camLeft / TILE_SIZE));
        int startRow = std::max(0, (int)(camTop / TILE_SIZE));
        int endCol = std::min(MAP_COLS, (int)(camRight / TILE_SIZE) + 1);
        int endRow = std::min(MAP_ROWS, (int)(camBottom / TILE_SIZE) + 1);
        for (int i = startRow; i < endRow; i++) {
            for (int j = startCol; j < endCol; j++) {
                Rectangle src {
                    0,
                    0,
                    89,
                    92
                };
                Rectangle dest {
                    (float)(j * TILE_SIZE), 
                    (float)(i * TILE_SIZE),
                    (float)TILE_SIZE,
                    (float)TILE_SIZE,
                };
                DrawTexturePro(srcset, src, dest, {0,0}, 0.0f, WHITE);
            }
        }
    }

    void drawEntities() {
        EntityManager &entities = world.entityManager;
       ComponentStore<Position> &positions = world.getStore<Position>();
       ComponentStore<Renderable> &renderables = world.getStore<Renderable>();
       ComponentStore<Size> &sizes = world.getStore<Size>();
        ComponentStore<UiElement>& uiElements = world.getStore<UiElement>();
        int count = entities.count;
        for (int i = 0; i < count; i++) {
            int e_id = entities.list[i].id;
            if  (e_id == REMOVED_ENTITY_ID) { continue; };

            if (!renderable(e_id, world))
            {
                continue;
            }

            if (uiElements.has(e_id)) {
                renderUiElement(world, e_id);
                continue;
            }

            Size& size = sizes.get(e_id);
            Position& pos = positions.get(e_id);

            Rectangle src {
                0,
                0,
                (float)renderables.get(e_id).texture.width,
                (float)renderables.get(e_id).texture.height
            };
            Rectangle dest { 
                pos.x, // confusingly the origin in DrawTexturePro also shifs the anchor point or something
                pos.y, // so we need to not apply the shift here but later
                (float)size.width,
                (float)size.height
            };
            renderUiElement(world, e_id);
            DrawTexturePro(renderables.get(e_id).texture,
                            src,
                            dest,
                            {dest.width/2, dest.height/2}, // now we apply the shift so that texture is correctly centered on position
                            pos.rt,
                            WHITE
            );

            bool drawHitbox = false;
            if (drawHitbox && world.getStore<HitBox>().has(e_id)) {
                HitBox& hb = world.getStore<HitBox>().get(e_id);
               drawRectangleRotatedLines({pos.x, pos.y}, hb.width, hb.height, pos.rt, GREEN);
            }
            bool drawPosition = false;
            if (drawPosition) {
                DrawCircle(pos.x, pos.y, 3.0f, RED);
            }
          
        };
    }

   void renderWorld(Texture2D& srcset){
        drawTiles(srcset);
        drawEntities();
    };

};


