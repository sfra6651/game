#pragma once

#include "raylib.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cassert>

#include "shared/entity.h"

struct Animation {
    Texture2D spriteSheet;
    int frames;
    int frameRows;
    int frameCols;
    int frameDuration;
};
struct Position { int x; int y; };
struct Size { int width; int height; };
struct Renderable { Texture2D texture; };
struct Direction { float x; float y; };
struct Speed { int v; };
struct Damage { int v; };
struct Health { int v; };
struct Owner { int id; };



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
