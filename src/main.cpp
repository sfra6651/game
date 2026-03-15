#include "raylib.h"

#include <cassert>
#include <functional>
#include <random>
#include <vector>

#include "shared/components.h"
#include "entities/player.h"
#include "shared/protocol.h"
#include "shared/tcp.h"
#include "systems/collision.h"
#include "systems/input.h"
#include "systems/physics.h"
#include "ecs/world.h"
#include "entities/enemy.h"
#include "resources/textureManager.h"
#include "systems/renderer.h"


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game");
    SetTargetFPS(60);

    TcpClient tcpClient {};

    World world{};
    InputSystem inputSystem { world };
    PhysicsSystem physicsSystem { world };
    RenderingSystem renderingSystem { world };
    CollisionSystem collisionSystem { world };

    world.registerCleanUp(world.renderables);
    world.registerCleanUp(world.positions);
    world.registerCleanUp(world.sizes);
    world.registerCleanUp(world.directions);
    world.registerCleanUp(world.speeds);
    world.registerCleanUp(world.damages);
    world.registerCleanUp(world.owners);
    world.registerCleanUp(world.healths);



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
        inputSystem.proccessInput(player);
    });
    world.registerPhysicsSystem([&physicsSystem]() {
        physicsSystem.proccessPhysics();
    });
    world.registerRenderSystem([&renderingSystem]() {
        renderingSystem.renderWorld();
    });
    world.registerCollisionSystem([&collisionSystem]() {
        collisionSystem.processCollisions();
    });

    tcpClient.connect();
    DebugProtocol protocol{};


    while (!WindowShouldClose()) { 
        BeginDrawing();
        ClearBackground(BLACK);

        // draw stuff here
        world.processInput();
        world.processPhysics();
        world.processCollisions();
        world.processRenders();

        if(tcpClient.isConnected()) {
            protocol.instruction = PROCCESS_NEXT_FRAME;
            protocol.createEntitySnaps(world);
            protocol.sendTo(tcpClient);
        };

        EndDrawing();
    }

    tcpClient.disconnect();

    CloseWindow();
    return 0;
}
