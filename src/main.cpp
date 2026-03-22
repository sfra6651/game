#include <cassert>
#include <functional>
#include <cstring>

#include "raylib.h"


#include "entities/player.h"
#include "shared/entity.h"
#include "systems/collision.h"
#include "systems/animation.h"
#include "systems/input.h"
#include "systems/physics.h"
#include "systems/renderer.h"
#include "ecs/world.h"
#include "entities/enemy.h"
#include "resources/textureManager.h"
#include "lib/utils.h"
#include "lib/osScaling.h"


int main() {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(1, 1, "");
#ifdef DEBUG
    int monitor = GetMonitorCount() > 1 ? 1 : 0;
#else
    int monitor = 0;
#endif
    float osScale = getOsScaleFactor();
    int monitorWidth = (int)roundf(GetMonitorWidth(monitor) * osScale);
    int monitorHeight = (int)roundf(GetMonitorHeight(monitor) * osScale);
    Vector2 monitorPos = GetMonitorPosition(monitor);
    SetWindowPosition(monitorPos.x, monitorPos.y);
    ToggleBorderlessWindowed();
    SetWindowSize(monitorWidth, monitorHeight);
    ClearWindowState(FLAG_WINDOW_HIDDEN);
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

    Camera2D camera = {};
    camera.target = { (float)WORLD_WIDTH/2.0f, (float)WORLD_HEIGHT/2.0f };
    camera.offset = { VIRTUAL_WIDTH/2.0f, VIRTUAL_HEIGHT/2.0f };
    camera.zoom = 1.0f;

    World world{};
    InputSystem inputSystem { world, camera,Vector2 {(float)monitorWidth/VIRTUAL_WIDTH, (float)monitorHeight/VIRTUAL_HEIGHT} };
    PhysicsSystem physicsSystem { world };
    AnimationSystem animationSystem { world };
    RenderingSystem renderingSystem { world, camera };
    CollisionSystem collisionSystem { world };

    world.textureManager.load();

    Texture2D tilesTexture = LoadTexture("./assets/textures/ork_world_tileset.png");
    std::fill(&world.tilemap[0][0], &world.tilemap[MAP_ROWS-1][MAP_COLS], 0);

    Entity player = playerFactory(world, {
        .texture = {world.textureManager.get("space_marine_top_down.png")},
        .pos = {WORLD_WIDTH/2, WORLD_HEIGHT/2},
        .speed = { 5 },
        .hitBox = {WORLD_WIDTH/2, WORLD_HEIGHT/2, 64, 64},
    });

    Entity ork = enemyFactory(world, {
        .texture = {world.textureManager.get("ork_down.png")},
        .pos = { WORLD_WIDTH/2 + 200, WORLD_HEIGHT/2 + 200},
        .speed = { 0 },
        .hitBox = { WORLD_WIDTH/2 + 200, WORLD_HEIGHT/2 + 200, 64, 64},
        .healthBar = { true },
    });

    world.registerInputSystem([&player, &inputSystem] () {
        inputSystem.processInput(player);
    });
    world.registerPhysicsSystem([&physicsSystem]() {
        physicsSystem.processPhysics();
    });
    world.registerCollisionSystem([&collisionSystem]() {
       collisionSystem.processCollisions();
   });
    world.registerAnimationSystem([&animationSystem]() {
        animationSystem.processAnimations();
    });
    world.registerRenderSystem([&renderingSystem, &tilesTexture]() {
        renderingSystem.renderWorld(tilesTexture);
    });

    while (!WindowShouldClose()) {
        BeginTextureMode(target);
            BeginMode2D(camera);
                ClearBackground(BLACK);
                world.processInput();
                world.processPhysics();
                world.processCollisions();
                world.processAnimations();
                world.processRenders();
            EndMode2D();
        EndTextureMode();

        BeginDrawing();
        Rectangle src = {0, 0, VIRTUAL_WIDTH, -VIRTUAL_HEIGHT};
        Rectangle dst = {0, 0, (float)monitorWidth, (float)monitorHeight};
        DrawTexturePro(target.texture, src, dst, {0, 0}, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
