#include "raylib.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "ecs/world.h"
#include "ecs/entity.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600



struct SpaceMarineConfig {
    Texture2D &texture;
    Position pos = {(int)WINDOW_WIDTH/2, (int)WINDOW_HEIGHT/2};
    Size size = {64, 64};
    Velocity v = { 10, 10 };

};

Entity spaceMarineFactory(
    World &world,
    SpaceMarineConfig config)
{
    Entity spaceMarine = world.entities.create();
    world.attach(spaceMarine, world.positions, config.pos);
    world.attach(spaceMarine, world.renderables, Renderable{config.texture});
    world.attach(spaceMarine, world.sizes, config.size);
    world.attach(spaceMarine, world.velocities, config.v);
    return spaceMarine;
};


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game");
    SetTargetFPS(60);

    World world{};

    world.registerCleanUp(world.renderables);
    world.registerCleanUp(world.positions);
    world.registerCleanUp(world.sizes);
    world.registerCleanUp(world.velocities);

    Texture2D spaceMarineTexture = LoadTexture("assets/space_marine_top_down.png");
    assert(spaceMarineTexture.id != 0 && "texture failed to load");

    Entity player = world.entities.create();
    world.attach(player, world.positions, Position{300, 400});
    world.attach(player, world.renderables, Renderable{spaceMarineTexture});
    world.attach(player, world.sizes, Size{64,64});
    world.attach(player, world.velocities, {0, 0});

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
        world.proccessInput(player.id);
        world.runPhysics();
        EndDrawing();
    }

    UnloadTexture(spaceMarineTexture);

    CloseWindow();
    return 0;
}
