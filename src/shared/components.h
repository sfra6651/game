#pragma once

#include "raylib.h"

#include <vector>
#include <cassert>

#define MAX_ENTITIES 1000
#define REMOVED_ENTITY -1

struct Position { int x; int y; };
struct Size { int width; int height; };
struct Renderable { Texture2D texture; };
struct Direction { float x; float y; };
struct Speed { int v; };

struct Entity {
    int id;
};

struct Entities {
    int freeIndex;
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

        assert(freeIndex < MAX_ENTITIES && "entity limit reached");

        id = freeIndex;
        list[id].id = id;
        freeIndex += 1;
        return list[id];
    };

    void remove(int id) {
        freeList.push_back(id);
        list[id].id = REMOVED_ENTITY;
    };
};
