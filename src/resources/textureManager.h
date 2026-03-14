#pragma once
#include <filesystem>
#include <ranges>
#include <unordered_map>

#include "raylib.h"

struct TextureManager {
    std::unordered_map<std::string, Texture2D> textures {};

    void load() {
        for (const auto& entry : std::filesystem::directory_iterator("assets/textures/")) {
            if (!entry.is_regular_file() || entry.path().extension() != ".png") { continue; }
            std::string filename = entry.path().filename().string();
            Texture2D texture = LoadTexture(entry.path().string().c_str());
            textures[filename] = texture;
        }
    }

    Texture2D& get(const std::string& filename) {
        return textures[filename];
    }

    void unload() {
        for (const auto &val: textures | std::views::values) {
            UnloadTexture(val);
        }
    }
};
