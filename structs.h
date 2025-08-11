#pragma once
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include "raylib.h"
#include <functional>
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






