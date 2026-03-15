#pragma once

#include "raylib.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cassert>

#define MAX_ENTITIES 1000
#define REMOVED_ENTITY_ID (-1)

struct Position { int x; int y; };
struct Size { int width; int height; };
struct Renderable { Texture2D texture; };
struct Direction { float x; float y; };
struct Speed { int v; };
struct Damage { int v; };
struct Health { int v; };
struct Owner { int id; };

struct Entity {
    int id;
};

struct Entities {
    int count;
    std::vector<int> freeList;
    Entity list[MAX_ENTITIES]{};

    Entity create() {
        int id;
        if (freeList.size() > 0) {
            id = freeList[freeList.size() - 1];
            list[id].id = id;
            freeList.pop_back();
            return list[id];
        }

        assert(count < MAX_ENTITIES && "entity limit reached");

        id = count;
        list[id].id = id;
        count += 1;
        return list[id];
    };

    void remove(int id) {
        freeList.push_back(id);
        list[id].id = REMOVED_ENTITY_ID;
    };
};

template<typename T>
struct ComponentStore {
    int sparse[MAX_ENTITIES] {};
    std::vector<T> dense {};
    std::vector<int> entities {};

    
    void add(int e_id, T component) {
        dense.push_back(component);
        entities.push_back(e_id);
        sparse[e_id] = dense.size() - 1;
    };

    void remove(int e_id){
        int denseIndex = sparse[e_id];
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

    T& get(int e_id) {
        int denseIndex = sparse[e_id];
        return dense[denseIndex];
    };

    bool has(int e_id) {
        int denseIndex = sparse[e_id];
        return denseIndex < dense.size() && entities[denseIndex] == e_id;
    };

    void clear() {
        dense.clear();
        entities.clear();
    };
};
