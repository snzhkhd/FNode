#include "ResourceManager.h"
#include "Utils.h"

// Static member definitions
std::unordered_map<std::string, std::weak_ptr<Texture2D>> ResourceManager::textures_;
std::unordered_map<std::string, std::weak_ptr<Sound>> ResourceManager::sounds_;
std::unordered_map<std::string, std::weak_ptr<Font>> ResourceManager::fonts_;
std::unordered_map<std::string, std::shared_ptr<Font>> ResourceManager::fontsEx_;


static std::string NormalizePath(const std::string& path) {
    // You may want to normalize separators or absolute path
    std::filesystem::path p(path);
    try {
        p = std::filesystem::absolute(p);
    }
    catch (...) {
        // fallback: use original
    }
    return p.string();
}

std::shared_ptr<Texture2D> ResourceManager::LoadTexture(const std::string& path)
{
    static std::unordered_map<std::string, std::shared_ptr<Texture2D>> cache;
    auto it = cache.find(path);
    if (it != cache.end()) {
        return it->second; // shared_ptr уже хранится в кэше
    }
    // Загружаем
    Texture2D tex = ::LoadTexture(path.c_str());
    if (tex.id == 0) {
        Utils::LogError("Failed to load texture: " + path);
        return nullptr;
    }
    // Создаём shared_ptr с делетером:
    auto deleter = [](Texture2D* t) {
        if (t) {
            UnloadTexture(*t);
            delete t;
            Utils::LogInfo("ResourceManager: Unloaded texture in deleter");
        }
    };
    std::shared_ptr<Texture2D> sp(new Texture2D(tex), deleter);
    cache[path] = sp; // сохраняем в кэше
    Utils::LogInfo("ResourceManager: Loaded texture: " + path);
    return sp;
}

std::shared_ptr<Sound> ResourceManager::LoadSound(const std::string& path)
{
    std::string np = NormalizePath(path);
    auto it = sounds_.find(np);
    if (it != sounds_.end()) {
        if (auto sp = it->second.lock()) {
            return sp;
        }
    }
    if (!Utils::FileExists(np)) {
        Utils::LogError("Sound file not found: " + np);
        return nullptr;
    }
    Sound snd = ::LoadSound(np.c_str());
    // Check: RayLib LoadSound returns Sound; no direct id check, but sampleCount? Assume valid.
    auto deleter = [](Sound* s) {
        Utils::LogInfo("Unloading sound");
        ::UnloadSound(*s);
        delete s;
    };
    std::shared_ptr<Sound> sp(new Sound(snd), deleter);
    sounds_[np] = sp;
    Utils::LogInfo("Loaded sound: " + np);
    return sp;
}

std::shared_ptr<Font> ResourceManager::LoadFont(const std::string& path)
{
    std::string np = NormalizePath(path);
    auto it = fonts_.find(np);
    if (it != fonts_.end()) {
        if (auto sp = it->second.lock()) {
            return sp;
        }
    }
    if (!Utils::FileExists(np)) {
        Utils::LogError("Font file not found: " + np);
        return nullptr;
    }

    // Создаем массив кодпоинтов (латиница + кириллица)
    std::vector<int> codepoints;

    // ASCII 32..126
    for (int c = 32; c <= 126; c++) codepoints.push_back(c);

    // Кириллица А-Я
    for (int c = 0x410; c <= 0x42F; c++) codepoints.push_back(c);
    // Кириллица а-я
    for (int c = 0x430; c <= 0x44F; c++) codepoints.push_back(c);
    // Ё и ё
    codepoints.push_back(0x401);
    codepoints.push_back(0x451);

    // Загружаем шрифт с нужными кодпоинтами
    Font fnt = ::LoadFontEx(np.c_str(), 18, codepoints.data(), (int)codepoints.size());
    if (fnt.texture.id == 0) {
        Utils::LogError("Failed to load font with UTF-8: " + np);
        return nullptr;
    }

    GenTextureMipmaps(&fnt.texture);

    auto deleter = [](Font* f) {
        Utils::LogInfo("Unloading font");
        ::UnloadFont(*f);
        delete f;
    };
    std::shared_ptr<Font> sp(new Font(fnt), deleter);
    fonts_[np] = sp;
    Utils::LogInfo("Loaded font with UTF-8 support: " + np);
    return sp;
}

std::shared_ptr<Font> ResourceManager::LoadFontEx(const std::string& path, int fontSize, const std::vector<int>& codepoints)
{
    std::string np = NormalizePath(path);
    std::string key = np + "|" + std::to_string(fontSize);

    auto it = fontsEx_.find(key);
    if (it != fontsEx_.end()) {
        if (auto sp = it->second) return sp;
    }
    if (!Utils::FileExists(np)) {
        Utils::LogError("Font file not found: " + np);
        return nullptr;
    }
    // Prepare codepoints array
    int glyphCount = static_cast<int>(codepoints.size());
    auto cpData = std::make_unique<int[]>(glyphCount);
    std::copy(codepoints.begin(), codepoints.end(), cpData.get());
    Font fnt = ::LoadFontEx(np.c_str(), fontSize, cpData.get(), glyphCount);
    auto deleter = [](Font* f) {
        Utils::LogInfo("Unloading font Ex");
        ::UnloadFont(*f);
        delete f;
    };
    auto sp = std::shared_ptr<Font>(new Font(fnt), deleter);
    fontsEx_[key] = sp;
    Utils::LogInfo("Loaded font Ex: " + key);
    return sp;
}

std::shared_ptr<Font> ResourceManager::GetFont(const std::string& path) {
    std::string np = NormalizePath(path);
    auto it = fonts_.find(np);
    if (it != fonts_.end()) return it->second.lock();
    return nullptr;
}

std::shared_ptr<Font> ResourceManager::GetFontEx(const std::string& path, int fontSize) {
    std::string np = NormalizePath(path);
    std::string key = np + "|" + std::to_string(fontSize);
    auto it = fontsEx_.find(key);
    if (it != fontsEx_.end()) return it->second;
    return nullptr;
}

void ResourceManager::UnloadAll()
{
    // Clear all maps, shared_ptr deleters will unload resources
    {
        textures_.clear();
    }
    {
        sounds_.clear();
    }
    {
        fonts_.clear();
    }
    {
        fontsEx_.clear();
    }
    Utils::LogInfo("Unloaded all resources");
}
