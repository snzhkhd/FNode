#include "PluginLoader.h"
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>

void PluginLoader::load(INodeRegistry& registry) {
    if (!std::filesystem::exists(folder)) {
        std::cerr << "Plugins folder not found: " << folder << std::endl;
        return;
    }

    for (auto& e : std::filesystem::directory_iterator(folder)) {
        if (!e.is_regular_file()) continue;
        auto ext = e.path().extension().string();

#ifdef _WIN32
        if (ext != ".dll") continue;
        HMODULE lib = LoadLibraryA(e.path().string().c_str());
        if (!lib) continue;
        auto fn = reinterpret_cast<RegisterPluginFn>(GetProcAddress(lib, "RegisterPlugin"));
#else
        if (ext != ".so") continue;
        void* lib = dlopen(e.path().string().c_str(), RTLD_NOW);
        if (!lib) continue;
        auto fn = reinterpret_cast<RegisterPluginFn>(dlsym(lib, "RegisterPlugin"));
#endif

        if (fn) fn(&registry);
    }
}