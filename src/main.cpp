#include "raylib.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "ecs/entity.h"
#include "entities/space_marine.h"
#include "entities/projectile.h"
#include "systems/input.h"
#include "ecs/world.h"

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game");
    SetTargetFPS(60);

    World world{};

    world.registerCleanUp(world.renderables);
    world.registerCleanUp(world.positions);
    world.registerCleanUp(world.sizes);
    world.registerCleanUp(world.velocities);


    Texture2D spaceMarineTexture = LoadTexture("assets/space_marine_top_down.png");
    assert(spaceMarineTexture.id != 0 && "spaceMarineTexture failed to load");

    Texture2D bulletTexture = LoadTexture("assets/bullet.png");
    assert(spaceMarineTexture.id != 0 && "bullet texture failed to load");

    Entity player = world.entities.create();
    world.attach(player, world.positions, Position{300, 400});
    world.attach(player, world.renderables, Renderable{spaceMarineTexture});
    world.attach(player, world.sizes, Size{64,64});
    world.attach(player, world.velocities, {0, 0});

    world.registerInputSystem([&world, &player] () {
        proccessInput(world, player);
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
        world.runPhysics();
        EndDrawing();
    }

    UnloadTexture(spaceMarineTexture);

    CloseWindow();
    return 0;
}
