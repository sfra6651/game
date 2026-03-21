#pragma once

#include "raylib.h"

#include <cmath>
#include <unordered_map>

#include "shared/components.h"
#include "entities/projectile.h"
#include "ecs/world.h"
#include "shared/componentGroups.h"
#include "lib/utils.h"

struct RenderingSystem{
    World& world;
    Camera2D& camera;

    void drawTiles(Texture2D& srcset) {
        float camLeft = camera.target.x - VIRTUAL_WIDTH / 2.0f;
        float camTop = camera.target.y - VIRTUAL_HEIGHT / 2.0f;
        float camRight = camLeft + VIRTUAL_WIDTH;
        float camBottom = camTop + VIRTUAL_HEIGHT;

        //Convert to tile indices:

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
        Entities &entities = world.entities;
       ComponentStore<Position> &positions = world.getStore<Position>();
       ComponentStore<Renderable> &renderables = world.getStore<Renderable>();
       ComponentStore<Size> &sizes = world.getStore<Size>();
        int count = entities.count;
        for (int i = 0; i < count; i++) {
            int e_id = entities.list[i].id;
            if  (e_id == REMOVED_ENTITY_ID) { continue; };
            if (!renderable(e_id, world))
            {
                continue;
            }
            Rectangle src {
                0,
                0,
                (float)renderables.get(e_id).texture.width,
                (float)renderables.get(e_id).texture.height
            };
            Rectangle dest { 
                (float)positions.get(e_id).x,
                (float)positions.get(e_id).y,
                (float)sizes.get(e_id).width,
                (float)sizes.get(e_id).height
            };
            DrawTexturePro(renderables.get(e_id).texture , src, dest, {0, 0}, 0.0f, WHITE );
        };
    }

   void renderWorld(Texture2D& srcset){
        drawTiles(srcset);
        drawEntities();
    };

};


