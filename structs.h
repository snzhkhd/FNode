#pragma once
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include "raylib.h"

enum class EVarType {
    Int,
    UInt,
    UInt8,
    Float,
    Bool,
    String,
    Critter,
    CritterCl,
    ProtoItem,
    Item,
    ItemCl,
};

struct SPort {
    std::string name;
    int ID;
    EVarType type; // enum: INT, FLOAT, BOOL, OBJECT, etc.
    bool isArray;
    bool isInput;
    bool isHovered = false;
};


struct SConnection 
{
    int inputNodeId;   // ID целевого узла
    int inputPortID;
    int outputNodeId;  // ID исходного узла
    int outputPortID;// »м€ выходного порта
};

struct sBase
{
public:
    std::string name;
    int ID;
    Vector2 position;
    std::shared_ptr<Texture2D> icon;
    bool isPure = false;
    bool isHovered = false;
};

struct SVariables : public sBase
{
    EVarType type;
};

struct NodeBase : public sBase
{
    std::vector<SPort> ports;

    std::vector<SConnection> connections;

    std::vector<SVariables> Variables;
    std::vector<NodeBase> Function;
};

struct Imports : public sBase
{
    std::vector<SPort> ports;

    int from; //ID;
};

class ScriptFile 
{
public:
    int ID;
    std::vector<std::shared_ptr<NodeBase>> Nodes;
    std::vector<SConnection> connections;
    std::vector<SVariables> Variables;

    std::vector<Imports> Importes; // пока импорты не будут выводитс€ не куда, но заполнить их нужно


    void AddNode(std::shared_ptr<NodeBase> node) { Nodes.push_back(node); }
    void RemoveNode(const std::string& nodeId);
    void AddConnection(SConnection conn) { connections.push_back(conn); }

    void Execute(); // ¬ыполн€ет весь граф

};



//struct PinTypeInfo {
//    const char* name;
//    PinBaseType baseType;
//    const char* color; // дл€ рисовани€ в нодах
//};
//
//inline static PinTypeInfo PinTypes[] = {
//    {"int",       PinBaseType::Int,       "#ffaaaa"},
//    {"uint",      PinBaseType::UInt,      "#aaffaa"},
//    {"uint8",     PinBaseType::UInt8,     "#aaaaff"},
//    {"float",     PinBaseType::Float,     "#ffffaa"},
//    {"bool",      PinBaseType::Bool,      "#ffccff"},
//    {"string",    PinBaseType::String,    "#cccccc"},
//    {"Critter",   PinBaseType::Critter,   "#ffaa88"},
//    {"CritterCl", PinBaseType::CritterCl, "#88aaff"},
//    {"ProtoItem", PinBaseType::ProtoItem, "#88ffaa"},
//    {"Item",      PinBaseType::Item,      "#ffaaff"},
//    {"ItemCl",    PinBaseType::ItemCl,    "#aaffff"},
//};
