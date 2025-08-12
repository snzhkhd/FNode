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
    int inputNodeId;   // ID �������� ����
    int inputPortID;
    int outputNodeId;  // ID ��������� ����
    int outputPortID;// ��� ��������� �����

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
};

struct Imports : public sBase
{
    std::vector<SPort> ports; 
    std::string sourceFile;

    int from; //ID;
};


struct NodeTemplate {
    std::string name;
    bool isPure = false; // ���� true � ��� Exec ������
    // ����� ����� ��� (name, type, isInput)
    std::vector<std::tuple<std::string, EVarType, bool>> ports;
    // ����� �������� ����������/������/�������� ��� �������������
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


    void AddNode(std::shared_ptr<NodeBase> node) { Nodes.push_back(node); }
    void RemoveNode(const std::string& nodeId);
    void AddConnection(SConnection conn) { connections.push_back(conn); }

    void Execute(); // ��������� ���� ����

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


// ���� ����� ����
struct ContextMenuItem {
    std::string label;
    std::function<void()> action; // �������� ��� �����
    bool enabled = true;          // ���� false - ����� �� ����������
    bool isSeparator = false;     // �����������
    // ����� ��������: std::vector<ContextMenuItem> children; // ��� submenus
    ContextMenuItem() = default;
    ContextMenuItem(std::string l, std::function<void()> a, bool e = true)
        : label(std::move(l)), action(std::move(a)), enabled(e), isSeparator(false) {}
    static ContextMenuItem Separator() { ContextMenuItem it; it.isSeparator = true; return it; }
};






// ��������� ��� ��������
struct Settings {
    std::string scriptFolder;
    float connectorHandleLength;
    float baseConnectorThickness;
    int windowWidth;
    int windowHeight;

    // ������������ � json
    json to_json() const {
        return json{
            {"scriptFolder", scriptFolder},
            {"connectorHandleLength", connectorHandleLength},
            {"baseConnectorThickness", baseConnectorThickness},
            {"windowWidth", windowWidth},
            {"windowHeight", windowHeight}
        };
    }

    // �������������� �� json
    void from_json(const json& j) {
        if (j.contains("scriptFolder")) j.at("scriptFolder").get_to(scriptFolder);
        if (j.contains("connectorHandleLength")) j.at("connectorHandleLength").get_to(connectorHandleLength);
        if (j.contains("baseConnectorThickness")) j.at("baseConnectorThickness").get_to(baseConnectorThickness);
        if (j.contains("windowWidth")) j.at("windowWidth").get_to(windowWidth);
        if (j.contains("windowHeight")) j.at("windowHeight").get_to(windowHeight);
    }
};