#pragma once

#include "raylib.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <utility>
#include <vector>

#include "components/entity.h"

#define MAX_ABILITIES 4

struct World;

enum CollisionType {
    DEFAULT,
    DEALS_DAMAGE,
};

enum DamageType {
    ON_COLLIDE,
    CONTINUOUS,
};

enum UiElementType {
    HEALTH_BAR,
};

struct Ability { 
    int parent {};
    int damage {};
    float cd {};
    float cdProg {};
    float distance {};
    float duration {};
    float width {};
    void (*effect)(World&, Ability&);
    Vector2 dir {};
    Vector2 pos {};
};

//*********Componentents**********
struct AbilitySet {
    Ability ability[MAX_ABILITIES];
    int count; 
};
struct AnchorPoint { float x; float y; };
struct CollisionBehavour { CollisionType type = DEFAULT; bool destroyOnCollide = false; };
struct Damage { int v; int perTick; DamageType type; };
struct HasDamaged { std::vector<int> ids; };
struct Direction { float x; float y; };
struct Health { int max; int current; };
struct HitBox { float width; float height; };
struct LifeTime { 
    int contextId; //general purpose id if we want to do something else with something other than parent. could be entity or component.
    float remaining;
    void (*onExpire)(World&, LifeTime&);
};
struct Owner { int id; };
struct Position { float x; float y; float rt; };
struct Renderable { Texture2D texture; };
struct Speed { int v; };
struct Size { int width; int height; };
struct UiElement { UiElementType type; bool visible; };
struct VelocityOveride { float x; float y; int speed; };

using Components = std::tuple<
    AbilitySet,
    AnchorPoint,
    CollisionBehavour,
    Damage,
    Direction,
    HasDamaged,
    Health,
    HitBox,
    LifeTime,
    Owner,
    Position,
    Renderable,
    Speed,
    Size,
    UiElement,
    VelocityOveride
>;
//********************************

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
