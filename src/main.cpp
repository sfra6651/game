#include "raylib.h"

#include <cassert>
#include <functional>
#include <random>
#include <vector>

#include "shared/components.h"
#include "entities/player.h"
#include "shared/protocol.h"
#include "shared/tcp.h"
#include "systems/input.h"
#include "systems/physics.h"
#include "ecs/world.h"
#include "systems/renderer.h"


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game");
    SetTargetFPS(60);

    TcpClient tcpClient {};

    World world{};
    InputSystem inputSystem { world };
    PhysicsSystem physicsSystem { world };
    RenderingSystem renderingSystem { world };

    world.registerCleanUp(world.renderables);
    world.registerCleanUp(world.positions);
    world.registerCleanUp(world.sizes);
    world.registerCleanUp(world.directions);
    world.registerCleanUp(world.speeds);

    Texture2D spaceMarineTexture = LoadTexture("assets/space_marine_top_down.png");
    assert(spaceMarineTexture.id != 0 && "spaceMarineTexture failed to load");
    world.textures["space_marine_top_down"] = &spaceMarineTexture;

    Texture2D bulletTexture = LoadTexture("assets/bullet.png");
    assert(bulletTexture.id != 0 && "bullet texture failed to load");
    world.textures["bullet_texture"] = &bulletTexture;

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
    world.registerRenderSystem([&renderingSystem]() {
        renderingSystem.renderWorld();
    });

    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> distW(0, WINDOW_WIDTH - 64);
    std::uniform_int_distribution<int> distH(0, WINDOW_HEIGHT - 64);
    std::uniform_int_distribution<int> distVx(-10, 10);
    std::uniform_int_distribution<int> distVy(-10, 10);

    for (int i = 0; i < 999; i++) {
       //Entity e = spaceMarineFactory(world, {.texture = spaceMarineTexture, .pos = {distW(rng), distH(rng)}, .v = {distVx(rng), distVy(rng)}});
    };

    tcpClient.connect();
    DebugProtocol protocol{};

    while (!WindowShouldClose()) { 
        BeginDrawing();
        ClearBackground(BLACK);
        // draw stuff here
        world.processPhysics();
        world.processInput();
        world.processRenders();

        if(tcpClient.isConnected()) {
            protocol.instruction = PROCCESS_NEXT_FRAME;
            protocol.createEntitySnaps(world);
            protocol.sendTo(tcpClient);
        };


        EndDrawing();
    }

    tcpClient.disconnect();

    UnloadTexture(spaceMarineTexture);

    CloseWindow();
    return 0;
}
