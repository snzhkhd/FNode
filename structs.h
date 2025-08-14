#pragma once
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include "raylib.h"
#include <functional>

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class EVarType {
    Int,
    UInt,
    UInt16,
    UInt8,
    Float,
    Bool,
    String,
    Critter,
    CritterCl,
    ProtoItem,
    Item,
    ItemCl,
    Enums,
    GameVar,
    Map,
    Defines,
    Colors,
    EXEC    //16
};

enum class RepType
{
    CLIENT  = 0,
    SERVER  = 1,
    MAP     = 2,
    CONST   = 3
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
    int outputPortID;// Имя выходного порта

    bool isTemp = false;
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

    std::string sourceFile;

    std::string ToolTip = "";
};

struct Imports : public sBase
{
    std::vector<SPort> ports; 
    std::string sourceFile;

    int from; //ID;
};


struct NodeTemplate {
    std::string name;
    bool isPure = false; // если true — без Exec портов

    std::string Category = "";
    int type = 0;
    // порты задаём как (name, type, isInput)
    std::vector<std::tuple<std::string, EVarType, bool>> ports;
    // можно добавить метаданные/иконку/описание при необходимости
    std::string ToolTip = "";

    NodeTemplate(const std::string& n, bool pure,
        const std::vector<std::tuple<std::string, EVarType, bool>>& p)
        : name(n), isPure(pure), ports(p) {}

    NodeTemplate(const std::string& n, bool pure, const std::string& cat, int t,
        const std::vector<std::tuple<std::string, EVarType, bool>>& p)
        : name(n), isPure(pure), Category(cat), type(t), ports(p) {}

    NodeTemplate(const std::string& n, bool pure, const std::vector<std::tuple<std::string, EVarType, bool>>& p, const std::string& tip)
        : name(n), isPure(pure), ports(p), ToolTip(tip) {}

    NodeTemplate(const std::string& n, bool pure, const std::string& cat, int t,
        const std::vector<std::tuple<std::string, EVarType, bool>>& p, const std::string& tip)
        : name(n), isPure(pure), Category(cat), type(t), ports(p), ToolTip(tip) {}

    NodeTemplate(const std::string& n, bool pure, const std::string& cat,
        const std::vector<std::tuple<std::string, EVarType, bool>>& p)
        : name(n), isPure(pure), Category(cat), ports(p) {}
    
    NodeTemplate(const std::string& n, bool pure, const std::string& cat,
        const std::vector<std::tuple<std::string, EVarType, bool>>& p,
        const std::string& tip)
        : name(n), isPure(pure), Category(cat), ports(p), ToolTip(tip) {}

};
// Node in category tree
struct CatNode {
    std::string name;
    std::string fullPath; // <-- полный путь, например "Server | Math"
    std::vector<int> templates; // indices
    std::vector<std::unique_ptr<CatNode>> children;
    bool expanded = false;     // default collapsed
    CatNode* parent = nullptr;

    CatNode(const std::string& n = "") : name(n) {}
};


// Visible entry for rendering
struct VisibleEntry {
    bool isCategory;
    CatNode* cat;        // valid if isCategory
    int templateIndex;   // valid if !isCategory
    int depth;           // nesting depth (0 = root)
};


class ScriptFile 
{
public:
    int ID;
    std::string name;
    std::vector<std::shared_ptr<NodeBase>> Nodes;
    std::vector<SConnection> connections;
    std::vector<SVariables> Variables;

    std::vector<Imports> Import;

    ScriptFile() {}

    void AddNode(std::shared_ptr<NodeBase> node) { Nodes.push_back(node); }
    void RemoveNode(const std::string& nodeId);
    void AddConnection(SConnection conn) { connections.push_back(conn); }

    void Execute(); // Выполняет весь граф

    void Clear()
    {
        ID = 0;
        name.clear();

        Nodes.clear();
        connections.clear();
        Variables.clear();
        Import.clear();
    }
};


// Один пункт меню
struct ContextMenuItem {
    std::string label;
    std::function<void()> action; // действие при клике
    bool enabled = true;          // если false - пункт не кликабелен
    bool isSeparator = false;     // разделитель
    // Можно добавить: std::vector<ContextMenuItem> children; // для submenus
    ContextMenuItem() = default;
    ContextMenuItem(std::string l, std::function<void()> a, bool e = true)
        : label(std::move(l)), action(std::move(a)), enabled(e), isSeparator(false) {}
    static ContextMenuItem Separator() { ContextMenuItem it; it.isSeparator = true; return it; }
};






// Структура для настроек
struct Settings {
    std::string scriptFolder;
    float connectorHandleLength;
    float baseConnectorThickness;
    int windowWidth;
    int windowHeight;

    // Сериализация в json
    json to_json() const {
        return json{
            {"scriptFolder", scriptFolder},
            {"connectorHandleLength", connectorHandleLength},
            {"baseConnectorThickness", baseConnectorThickness},
            {"windowWidth", windowWidth},
            {"windowHeight", windowHeight}
        };
    }

    // Десериализация из json
    void from_json(const json& j) {
        if (j.contains("scriptFolder")) j.at("scriptFolder").get_to(scriptFolder);
        if (j.contains("connectorHandleLength")) j.at("connectorHandleLength").get_to(connectorHandleLength);
        if (j.contains("baseConnectorThickness")) j.at("baseConnectorThickness").get_to(baseConnectorThickness);
        if (j.contains("windowWidth")) j.at("windowWidth").get_to(windowWidth);
        if (j.contains("windowHeight")) j.at("windowHeight").get_to(windowHeight);
    }
};