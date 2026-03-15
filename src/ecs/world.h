#pragma once

#include "raylib.h"

#include <cassert>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "resources/textureManager.h"
#include "shared/components.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

struct World {
    Entities entities{};

    ComponentStore<Damage> damages;
    ComponentStore<Direction> directions;
    ComponentStore<Health> healths;
    ComponentStore<Position> positions;
    ComponentStore<Owner> owners;
    ComponentStore<Renderable> renderables;
    ComponentStore<Size> sizes;
    ComponentStore<Speed> speeds;

    TextureManager textureManager {};

    std::vector<std::function<void(int)>> cleanUps;

    std::function<void()> processInput;
    std::function<void()> processPhysics;
    std::function<void()> processRenders;
    std::function<void()> processCollisions;

    void registerInputSystem(std::function<void()> fn) {
        processInput = std::move(fn);
    }

    void registerPhysicsSystem(std::function<void()> fn) {
        processPhysics = fn;
    }

    void registerRenderSystem(std::function<void()> fn) {
        processRenders = fn;
    }

    void registerCollisionSystem(std::function<void()> fn) {
        processCollisions = fn;
    }

    template<typename T>
    void registerCleanUp(ComponentStore<T> &store){
        cleanUps.push_back([&store] (int id) { if (store.has(id)) store.remove(id); });
    }

    template<typename T>
    void attach(Entity entity, ComponentStore<T> &store, T component){
        store.add(entity.id, component);
    }

    void erase(Entity entity) {
        for (auto& cleanUp: cleanUps) {
            cleanUp(entity.id);
        };
        entities.remove(entity.id);
    }
};
