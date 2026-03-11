#pragma once

#include "raylib.h"

#include <cassert>
#include <functional>
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
    std::unordered_map<int, Velocity> velocities;

    std::vector<std::function<void(int)>> cleanUps;

    std::function<void()> inputSystem;

    void registerInputSystem(std::function<void()> fn) {
        inputSystem = fn;
    };

    template<typename T>
    void registerCleanUp(std::unordered_map<int, T> &store){
        cleanUps.push_back([&store] (int id) { store.erase(id); });
    }

    template<typename T>
    void attach(Entity entity, std::unordered_map<int, T> &store, T component){
        store[entity.id] = component;
    };

    template<typename T>
    void erase(Entity entity) {
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

    void runPhysics(){
        int count = entities.freeIndex;
        for (int i = 0; i < count; i++) {
            int e_id = entities.list[i].id; 
            if (!velocities.count(e_id)) { 
                continue;
            };
            Velocity v = velocities[e_id];
            Position pos = positions[e_id];
            int x_new = pos.x += v.x;
            int y_new = pos.y += v.y;
            Position new_pos{ x_new, y_new};
            if (x_new >= WINDOW_WIDTH || x_new <= 0) {
                new_pos.x = pos.x; 
                velocities[e_id].x *= -1; 
            };
            if (y_new >= WINDOW_HEIGHT || y_new <= 0) {
                new_pos.y = pos.y; 
                velocities[e_id].y *= -1; 
            };
            positions[e_id] = new_pos;
        };
    };


};
