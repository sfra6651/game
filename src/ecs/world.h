#pragma once

#include "raylib.h"

#include <cassert>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "entity.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

struct World {
    Entities entities{};

    std::unordered_map<int, Position> positions;
    std::unordered_map<int, Size> sizes;
    std::unordered_map<int, Renderable> renderables;
    std::unordered_map<int, Direction> directions;
    std::unordered_map<int, Speed> speeds;

    std::unordered_map<std::string, Texture2D*> textures;

    std::vector<std::function<void(int)>> cleanUps;

    std::function<void()> inputSystem;
    std::function<void()> physicsSystem;

    void registerInputSystem(std::function<void()> fn) {
        inputSystem = fn;
    };

    void registerPhysicsSystem(std::function<void()> fn) {
        physicsSystem = fn;
    };

    template<typename T>
    void registerCleanUp(std::unordered_map<int, T> &store){
        cleanUps.push_back([&store] (int id) { store.erase(id); });
    }

    template<typename T>
    void attach(Entity entity, std::unordered_map<int, T> &store, T component){
        store[entity.id] = component;
    };

    void erase(Entity &entity) {
        for (auto& cleanUp: cleanUps) {
            cleanUp(entity.id);
        };
        entities.remove(entity.id);
    };

    void drawRenderables(){
        int count = entities.freeIndex;
        for (int i = 0; i < count; i++) {
            int e_id = entities.list[i].id;
            if (!positions.count(e_id) || !renderables.count(e_id) || !sizes.count(e_id)) 
            {
                continue;
            }
            Rectangle src {
                0,
                0,
                (float)renderables[e_id].texture.width,
                (float)renderables[e_id].texture.height
            };
            Rectangle dest { 
                (float)positions[e_id].x,
                (float)positions[e_id].y,
                (float)sizes[e_id].width,
                (float)sizes[e_id].height
            };
            DrawTexturePro(renderables[e_id].texture , src, dest, {0, 0}, 0.0f, WHITE );
        };
    };

 


};
