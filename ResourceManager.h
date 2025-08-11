#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "raylib.h"


class ResourceManager 
{
public:
    // Load or get cached texture
    static std::shared_ptr<Texture2D> LoadTexture(const std::string& path);
    static std::shared_ptr<Sound> LoadSound(const std::string& path);
    static std::shared_ptr<Font> LoadFont(const std::string& path);
    // Load or get cached font (extended) with custom codepoints via LoadFontEx
    static std::shared_ptr<Font> LoadFontEx(const std::string& path, int fontSize, const std::vector<int>& codepoints);


    // Convenience: get previously loaded font (basic) or nullptr
    static std::shared_ptr<Font> GetFont(const std::string& path);
    // Convenience: get previously loaded extended font by key
    static std::shared_ptr<Font> GetFontEx(const std::string& path, int fontSize = 16);


    // Unload all resources
    static void UnloadAll();

private:
    ResourceManager() = delete;
    ~ResourceManager() = delete;

    static std::unordered_map<std::string, std::weak_ptr<Texture2D>> textures_;
    static std::unordered_map<std::string, std::weak_ptr<Font>> fonts_;
    static std::unordered_map<std::string, std::shared_ptr<Font>> fontsEx_;
    static std::unordered_map<std::string, std::weak_ptr<Sound>> sounds_;
};