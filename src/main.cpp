#include <cassert>
#include <functional>
#include <cstring>

#include "raylib.h"


#include "components/abilities.h"
#include "components/entity.h"
#include "entities/player.h"
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
    SetTargetFPS(165);

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
        .abilities = { {1.0f, 0, dash, KEY_SPACE } },
        .texture = {world.textureManager.get("space_marine_top_down.png")},
        .pos = {WORLD_WIDTH/2, WORLD_HEIGHT/2},
        .speed = { 5 },
        .hitBox = {WORLD_WIDTH/2, WORLD_HEIGHT/2, 64, 64},
    });


    Entity ork = enemyFactory(world, {
        .texture = {world.textureManager.get("ork_down.png")},
        .pos = { WORLD_WIDTH/2 + 200, WORLD_HEIGHT/2 + 200},
        .speed = { 0 },
        .healthBar = { true },
        .hitBox = { WORLD_WIDTH/2 + 200, WORLD_HEIGHT/2 + 200, 64, 64},
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

    const float TICK_RATE = 1.0f / 60.0f;
    float accumulator = 0.0f;
    while (!WindowShouldClose()) {
        float frameTime = GetFrameTime();
        accumulator += frameTime;
        world.processInput();
        if (accumulator >= TICK_RATE) {
            world.processPhysics();
            world.processCollisions();
            accumulator -= TICK_RATE;
        }
        if (accumulator > TICK_RATE) {
            accumulator = TICK_RATE;
        }
        BeginTextureMode(target);
            BeginMode2D(camera);
                ClearBackground(BLACK);
                world.processAnimations();
                world.processRenders();
            EndMode2D();
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, VIRTUAL_HEIGHT - 30, 20, WHITE);
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
