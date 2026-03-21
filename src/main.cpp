#include <cassert>
#include <functional>
#include <random>
#include <syncstream>
#include <vector>

#include "raylib.h"

#include "shared/components.h"
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

#define VIRTUAL_WIDTH 1920
#define VIRTUAL_HEIGHT 1080

int main() {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(1, 1, "");
#ifdef DEBUG
    int monitor = GetMonitorCount() > 1 ? 1 : 0;
#else
    int monitor = 0;
#endif
    float osScale = getOsScaleFactor();
    int mW = (int)roundf(GetMonitorWidth(monitor) * osScale);
    int mH = (int)roundf(GetMonitorHeight(monitor) * osScale);
    Vector2 monitorPos = GetMonitorPosition(monitor);
    SetWindowPosition(monitorPos.x, monitorPos.y);
    ToggleBorderlessWindowed();
    SetWindowSize(mW, mH);
    ClearWindowState(FLAG_WINDOW_HIDDEN);
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

    println(osScale);
    println("Monitor", mW, mH);
    println("Window:", GetScreenWidth(), "Render:", GetRenderWidth());
    println("Screen:", GetScreenWidth(), GetScreenHeight());
    println("Render:", GetRenderWidth(), GetRenderHeight());

    World world{};
    InputSystem inputSystem { world };
    PhysicsSystem physicsSystem { world };
    AnimationSystem animationSystem { world };
    RenderingSystem renderingSystem { world };
    CollisionSystem collisionSystem { world };

    world.textureManager.load();

    Entity player = playerFactory(world, {
        .texture = {world.textureManager.get("space_marine_top_down.png")},
        .pos = {300, 400},
        .speed = { 5 },
    });

    Entity ork = enemyFactory(world, {
        .texture = {world.textureManager.get("ork_down.png")},
        .pos = { 800, 800},
        .speed = { 0 }
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
    world.registerRenderSystem([&renderingSystem]() {
        renderingSystem.renderWorld();
    });

    while (!WindowShouldClose()) {
        BeginTextureMode(target);
        ClearBackground(BLACK);

        world.processInput();
        world.processPhysics();
        world.processCollisions();
        world.processAnimations();
        world.processRenders();
        EndTextureMode();

        BeginDrawing();
        Rectangle src = {0, 0, VIRTUAL_WIDTH, -VIRTUAL_HEIGHT};
        Rectangle dst = {0, 0, (float)mW, (float)mH};
        DrawTexturePro(target.texture, src, dst, {0, 0}, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
