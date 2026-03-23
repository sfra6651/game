#pragma once

#include "raylib.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

#include "components/entity.h"

struct World;

struct Ability { 
    float cd;
    float cdProg;
    float duration;
    void (*effect)(World&, int);
    int keyBind;
};
struct Damage { int v; };
struct Direction { float x; float y; };
struct Health { int v; };
struct HealthBar { bool visible; };
struct HitBox { Rectangle rect; };
struct LifeTime { float remaining; void (*onExpire)(); };
struct Owner { int id; };
struct Position { float x; float y; };
struct Renderable { Texture2D texture; };
struct Speed { int v; };
struct Size { int width; int height; };

using Components = std::tuple<
    Ability,
    Damage,
    Direction,
    Health,
    HealthBar,
    HitBox,
    LifeTime,
    Owner,
    Position,
    Renderable,
    Speed,
    Size
>;
static constexpr int MaxComponents = std::tuple_size_v<Components>;


template<typename T>
struct ComponentStore {
    using value_type = T;

    int sparse[MAX_ENTITIES] {};
    std::vector<T> dense {};
    std::vector<int> entities {};

    
    void add(int eId, T component) {
        dense.push_back(component);
        entities.push_back(eId);
        sparse[eId] = dense.size() - 1;
    };

    void remove(int eId){
        int denseIndex = sparse[eId];
        int lastId = entities[entities.size()-1];
        if (entities.size() > 1) {
            std::swap(entities[denseIndex], entities[entities.size()-1]);
            std::swap(dense[denseIndex], dense[dense.size()-1]);
            sparse[lastId] = denseIndex;
        };
        dense.pop_back();
        entities.pop_back();
    };

    int count() {
        return entities.size();
    };

    T& get(int eId) {
        int denseIndex = sparse[eId];
        return dense[denseIndex];
    };

    bool has(int eId) {
        int denseIndex = sparse[eId];
        return denseIndex < dense.size() && entities[denseIndex] == eId;
    };

    void clear() {
        dense.clear();
        entities.clear();
    };
};

template<typename T> struct MakeStores;

template<typename... Ts>
struct MakeStores<std::tuple<Ts...>> {
    using storeType = std::tuple<ComponentStore<Ts>...>;
};
