#pragma once

#include <functional>
#include <utility>
#include <vector>

#include "resources/textureManager.h"
#include "shared/components.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

struct World {
    Entities entities{};

    TextureManager textureManager {};

    MakeStores<Components>::storeType stores;

    std::function<void()> processInput;
    std::function<void()> processPhysics;
    std::function<void()> processAnimations;
    std::function<void()> processRenders;
    std::function<void()> processCollisions;

    void registerInputSystem(const std::function<void()>& fn) {
        processInput = fn;
    }

    void registerPhysicsSystem(const std::function<void()>& fn) {
        processPhysics = fn;
    }

    void registerAnimationSystem(const std::function<void()>& fn) {
        processAnimations = fn;
    };

    void registerRenderSystem(const std::function<void()>& fn) {
        processRenders = fn;
    }

    void registerCollisionSystem(const std::function<void()>& fn) {
        processCollisions = fn;
    }

    template<typename T>
    ComponentStore<T>& getStore() {
        return std::get<ComponentStore<T>>(stores);
    }

    template<typename T>
    void attach(Entity entity, ComponentStore<T> &store, T component){
        store.add(entity.id, component);
    }

    void erase(Entity entity) {
        std::apply([&](auto&... S) {
            ((S.has(entity.id) ? (S.remove(entity.id), 0) : 0), ...);
        }, stores);
        entities.remove(entity.id);
    }
};
