#pragma once

#include <vector>
#include <cassert>

#define REMOVED_ENTITY_ID (-1)

struct Entity {
    int id;
};

#define MAX_ENTITIES 1000

struct Entities {
    int count;
    std::vector<int> freeList;
    Entity list[MAX_ENTITIES]{};

    Entity get(int id) {
        return list[id];
    }

    Entity create() {
        int id;
        if (!freeList.empty()) {
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
