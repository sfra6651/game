#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "components/components.h"
#include "components/componentGroups.h"
#include "entities/projectile.h"
#include "ecs/world.h"
#include "lib/utils.h"

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
        auto [parentX, parentY] = positions.get(ownerId);
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

            Rectangle src {
                0,
                0,
                (float)renderables.get(e_id).texture.width,
                (float)renderables.get(e_id).texture.height
            };
            Rectangle dest { 
                positions.get(e_id).x - (int)(sizes.get(e_id).width / 2),
                positions.get(e_id).y - (int)(sizes.get(e_id).height / 2),
                (float)sizes.get(e_id).width,
                (float)sizes.get(e_id).height
            };
            renderUiElement(world, e_id);

            bool drawHitbox = false;
            if (drawHitbox && world.getStore<HitBox>().has(e_id)) {
                HitBox& hb = world.getStore<HitBox>().get(e_id);
               DrawRectangleLinesEx({
                    (float)sizes.get(e_id).width - hb.width/2,
                    (float)sizes.get(e_id).height - hb.height/2,
                    hb.width,
                    hb.height,
                }, 1, GREEN);
            }
            DrawTexturePro(renderables.get(e_id).texture , src, dest, {0, 0}, 0.0f, WHITE );
        };
    }

   void renderWorld(Texture2D& srcset){
        drawTiles(srcset);
        drawEntities();
    };

};


