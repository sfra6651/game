#include "raylib.h"

#include <cassert>
#include <functional>
#include <random>
#include <vector>

#include "shared/components.h"
#include "entities/player.h"
#include "shared/entity.h"
#include "shared/message.h"
#include "shared/snapshot.h"
#include "shared/tcp.h"
#include "systems/collision.h"
#include "systems/animation.h"
#include "systems/input.h"
#include "systems/physics.h"
#include "systems/renderer.h"
#include "ecs/world.h"
#include "entities/enemy.h"
#include "resources/textureManager.h"


int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game");
    SetTargetFPS(60);

    TcpClient tcpClient {};

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

    tcpClient.connect();
    TcpConnection &conn = tcpClient.connection();
    bool debugPaused = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        world.processInput();
        world.processPhysics();
        world.processCollisions();
        world.processAnimations();
        world.processRenders();

        if (!conn.isConnected()) {
            debugPaused = false;
        } else {
            auto snaps = createEntitySnaps(world);
            if (!sendMessage(conn, MessageType::FRAME_SNAPSHOT,
                        snaps.data(), snaps.size() * sizeof(EntitySnapshot))) {
                debugPaused = false;
            } else if (!debugPaused) {
                // Running: non-blocking drain, check for PAUSE
                MessageHeader header{};
                std::vector<uint8_t> payload;
                while (tryRecvMessage(conn, header, payload)) {
                    if (header.type == MessageType::PAUSE) { debugPaused = true; break; }
                }
            }

            // Paused: block waiting for STEP or RESUME
            while (debugPaused && conn.isConnected()) {
                MessageHeader header{};
                std::vector<uint8_t> payload;
                if (!recvMessage(conn, header, payload)) {
                    debugPaused = false;
                    break;
                }
                if (header.type == MessageType::STEP) break;
                if (header.type == MessageType::RESUME) debugPaused = false;
            }
        }

        EndDrawing();
    }

    tcpClient.disconnect();

    CloseWindow();
    return 0;
}
