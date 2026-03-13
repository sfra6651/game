#include "raylib.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <random>

#include "ecs/entity.h"
#include "entities/player.h"
#include "systems/input.h"
#include "systems/physics.h"
#include "ecs/world.h"



int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game");
    SetTargetFPS(60);

    World world{};
    InputSystem inputSystem { world };
    PhysicsSystem physicsSystem { world };

    world.registerCleanUp(world.renderables);
    world.registerCleanUp(world.positions);
    world.registerCleanUp(world.sizes);
    world.registerCleanUp(world.directions);
    world.registerCleanUp(world.speeds);

    Texture2D spaceMarineTexture = LoadTexture("assets/space_marine_top_down.png");
    assert(spaceMarineTexture.id != 0 && "spaceMarineTexture failed to load");

    Texture2D bulletTexture = LoadTexture("assets/bullet.png");
    assert(bulletTexture.id != 0 && "bullet texture failed to load");

    Entity player = playerFactory(world, {
        .texture = {spaceMarineTexture},
        .pos = {300, 400},
        .speed = { 5 }
    });

    world.registerInputSystem([&player, &inputSystem] () {
        inputSystem.proccessInput(player);
    });
    world.registerPhysicsSystem([&physicsSystem]() {
        physicsSystem.proccessPhysics();
    });
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> distW(0, WINDOW_WIDTH - 64);
    std::uniform_int_distribution<int> distH(0, WINDOW_HEIGHT - 64);
    std::uniform_int_distribution<int> distVx(-10, 10);
    std::uniform_int_distribution<int> distVy(-10, 10);

    for (int i = 0; i < 999; i++) {
       //Entity e = spaceMarineFactory(world, {.texture = spaceMarineTexture, .pos = {distW(rng), distH(rng)}, .v = {distVx(rng), distVy(rng)}});
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        // draw stuff here
        world.drawRenderables();
        world.inputSystem();
        world.physicsSystem();


        EndDrawing();
    }

    UnloadTexture(spaceMarineTexture);

    CloseWindow();
    return 0;
}
