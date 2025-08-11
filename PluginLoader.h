#pragma once
#include "PluginAPI.h"
#include <string>

struct Host;
class PluginLoader {
    std::string folder;
public:
    explicit PluginLoader(const std::string& pluginsFolder) : folder(pluginsFolder) {}
    void load(INodeRegistry& registry);
};