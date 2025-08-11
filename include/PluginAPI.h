#pragma once
#include <functional>
struct Node;
struct INodeRegistry {
    virtual void registerFactory(std::function<Node* ()> f) = 0;
    virtual ~INodeRegistry() = default;
};
using RegisterPluginFn = void(*)(INodeRegistry*);