#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <filesystem>
#include "PluginAPI.h"
#include "Node.h"
#include "PluginLoader.h"  // abstracts platform-specific includes

#include "ResourceManager.h"

#include "ScriptParser.h"

struct ConnectionDrag {
    bool active = false;
    int nodeId = -1;
    int portId = -1;
    bool isOutput = false; // true если начали с выходного пина
} g_connDrag;
//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void AddFuncBtn();                // Button: add_func_btn logic
static void AddVarBtn();                // Button: add_var_btn logic
static void AddLvarBtn();                // Button: add_lvar_btn logic
static void SaveBtn();                // Button: save_btn logic
static void CompileBtn();                // Button: compile_btn logic
static void SettingBtn();                // Button: setting_btn logic
static void GvarsBtn();                // Button: gvars_btn logic
static void DefinesBtn();                // Button: defines_btn logic
static void ClearFosbBtn();                // Button: clear_fosb_btn logic

std::string LoadFileContent(const std::string& filePath);

void UpdateLayout(Rectangle* recs, int screenWidth, int screenHeight);
void GraphUpdate(Rectangle graphArea);
void GraphDraw(Rectangle graphArea);
Vector2 SnapToGrid(Vector2 pos);

void DrawNode(const NodeBase& node);
void NodeUpdate(NodeBase& node);


Vector2 GetPortScreenPosition(const NodeBase& node, int portId);
void DrawConection();

Vector2 graphOffset = { 0, 0 };       // Смещение камеры
bool isGraphDragging = false;       // Флаг перетаскивания
Vector2 lastGraphMousePos = { 0, 0 }; // Последняя позиция мыши
float graphScale = 1.0f;  // масштаб (1.0 = 100%)
const float MIN_SCALE = 0.25f;
const float MAX_SCALE = 4.0f;

#define MAX_BTN_TEXT 64
char compile_btnText[MAX_BTN_TEXT] = { 0 };  // Буфер, который можно менять
bool isCompiled = false;                     // Состояние компиляции
bool isShowConsole = false;


// Drag state для нод
static int g_draggingNodeId = -1;
static Vector2 g_dragStartMouseWorld = { 0,0 };
static Vector2 g_dragStartNodePos = { 0,0 };


// Получение следующего ID ноды (простая реализация)
static int GetNextNodeId(const ScriptFile& file) {
    int maxId = 0;
    for (auto& n : file.Nodes) if (n->ID > maxId) maxId = n->ID;
    return maxId + 1;
}

// screen -> world (учитываем graphScale и graphOffset)
static Vector2 ScreenToWorld(const Vector2& screen) {
    return { (screen.x + graphOffset.x) / graphScale, (screen.y + graphOffset.y) / graphScale };
}
static Vector2 WorldToScreen(const Vector2& world) {
    return { world.x * graphScale - graphOffset.x, world.y * graphScale - graphOffset.y };
}

static Rectangle CalculateNodeRect(const NodeBase& node)
{
    int NodeSize = 64;
    float scaledNode = NodeSize * graphScale;
    float minWidth = scaledNode;
    float minHeight = scaledNode;
    float nameWidth = node.name.size() * 8.0f * graphScale + 30.0f * graphScale;
    float width = std::max(minWidth, nameWidth);
    int portsCount = (int)node.ports.size();
    float height = std::max(minHeight, portsCount * 30.0f * graphScale);
    Vector2 screenPos = WorldToScreen(node.position);
    return Rectangle{ screenPos.x, screenPos.y, width, height };
}

static NodeBase* FindNodeById(ScriptFile& file, int id) {
    for (auto& n : file.Nodes) if (n->ID == id) return n.get();
    return nullptr;
}





void BeginConnectionDrag(int nodeId, int portId, bool isOutput) {
    g_connDrag.active = true;
    g_connDrag.nodeId = nodeId;
    g_connDrag.portId = portId;
    g_connDrag.isOutput = isOutput;
}

void CancelConnectionDrag() {
    g_connDrag = ConnectionDrag();
}

void ConnectPorts(ScriptFile& scriptFile, int outputNodeId, int outputPortID, int inputNodeId, int inputPortID) {
    // Проверки: не соединяем порт сам с собой и т.д.
    if (outputNodeId == inputNodeId && outputPortID == inputPortID) return;

    // Удалим дубликаты если есть
    for (const auto& c : scriptFile.connections) {
        if (c.outputNodeId == outputNodeId && c.outputPortID == outputPortID &&
            c.inputNodeId == inputNodeId && c.inputPortID == inputPortID) {
            return; // уже есть
        }
    }

    SConnection conn;
    conn.outputNodeId = outputNodeId;
    conn.outputPortID = outputPortID;
    conn.inputNodeId = inputNodeId;
    conn.inputPortID = inputPortID;
    scriptFile.connections.push_back(conn);
}

void DisconnectPorts(ScriptFile& scriptFile, int outputNodeId, int outputPortID, int inputNodeId, int inputPortID) {
    scriptFile.connections.erase(
        std::remove_if(scriptFile.connections.begin(), scriptFile.connections.end(),
            [&](const SConnection& c) {
                return c.outputNodeId == outputNodeId && c.outputPortID == outputPortID &&
                    c.inputNodeId == inputNodeId && c.inputPortID == inputPortID;
            }),
        scriptFile.connections.end());
}

void DisconnectAllForPort(ScriptFile& file, int nodeId, int portId) {
    file.connections.erase(
        std::remove_if(file.connections.begin(), file.connections.end(),
            [&](const SConnection& c) {
                return (c.inputNodeId == nodeId && c.inputPortID == portId) ||
                    (c.outputNodeId == nodeId && c.outputPortID == portId);
            }),
        file.connections.end());
}


// Попытка завершить drag соединения — если отпустили над портом — соединяем
void TryFinishConnectionDrag(ScriptFile& scriptFile) {
    if (!g_connDrag.active) return;
    Vector2 mouse = GetMousePosition();

    // Найдём порт под курсором
    for (auto& nodePtr : scriptFile.Nodes) {
        NodeBase* node = nodePtr.get();
        for (const auto& port : node->ports) {
            Vector2 ppos = GetPortScreenPosition(*node, port.ID);
            float radius = 6.0f * graphScale;
            if (CheckCollisionPointCircle(mouse, ppos, radius)) {
                // избежать соединения вход->вход или выход->выход
                if (g_connDrag.isOutput && port.isInput) {
                    // g_connDrag.nodeId:g_connDrag.portId -> node.ID:port.ID
                    ConnectPorts(scriptFile, g_connDrag.nodeId, g_connDrag.portId, node->ID, port.ID);
                }
                else if (!g_connDrag.isOutput && !port.isInput) {
                    // если начали с входа и отпустили на выходе — перевернём
                    ConnectPorts(scriptFile, node->ID, port.ID, g_connDrag.nodeId, g_connDrag.portId);
                }
                CancelConnectionDrag();
                return;
            }
        }
    }

    // Если не нашли порт — просто отменим drag
    CancelConnectionDrag();
}



std::shared_ptr<NodeBase> AddNode(ScriptFile& scriptFile, const std::string& name, Vector2 worldPos, bool isPure = false) {
    auto node = std::make_shared<NodeBase>();
    node->ID = GetNextNodeId(scriptFile);
    node->name = name;
    node->position = worldPos;
    node->isPure = isPure;

    // Добавляем стандартные порты (например ExecIn/ExecOut если !isPure)
    int portId = 0;
    if (!node->isPure) {
        SPort in; in.name = "ExecIn"; in.ID = portId++; in.isInput = true; in.isArray = false; in.type = EVarType::Bool;
        SPort out; out.name = "ExecOut"; out.ID = portId++; out.isInput = false; out.isArray = false; out.type = EVarType::Bool;
        node->ports.push_back(in);
        node->ports.push_back(out);
    }
    // можно добавить другие дефолтные порты тут

    scriptFile.Nodes.push_back(node);
    return node;
}

void DeleteNode(ScriptFile& scriptFile, int nodeId) {
    // Удаляем соединения с этой нодой
    scriptFile.connections.erase(
        std::remove_if(scriptFile.connections.begin(), scriptFile.connections.end(),
            [&](const SConnection& c) {
                return c.inputNodeId == nodeId || c.outputNodeId == nodeId;
            }),
        scriptFile.connections.end());

    // Удаляем саму ноду
    scriptFile.Nodes.erase(
        std::remove_if(scriptFile.Nodes.begin(), scriptFile.Nodes.end(),
            [&](const std::shared_ptr<NodeBase>& n) { return n->ID == nodeId; }),
        scriptFile.Nodes.end());
}


static float Vector2DistanceSqrLine(const Vector2& p, const Vector2& a, const Vector2& b);
// точка на кубическом Безьере для t in [0..1]
static Vector2 BezierCubicPoint(float t, const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    Vector2 p = { 0,0 };
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    return p;
}

// аппроксимируем расстояние от точки до кривой, дискретизируя N сегментов
static float DistancePointToBezierApprox(const Vector2& pt, const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, int samples = 30) {
    float best = FLT_MAX;
    Vector2 prev = p0;
    for (int i = 1; i <= samples; ++i) {
        float t = (float)i / (float)samples;
        Vector2 cur = BezierCubicPoint(t, p0, p1, p2, p3);
        // расстояние от pt до отрезка prev-cur
        float d = Vector2DistanceSqrLine(pt, prev, cur); // у тебя, возможно, нет этой функции — сделаем вручную ниже
        best = std::min(best, d);
        prev = cur;
    }
    return sqrt(best);
}

// Если нет Vector2DistanceSqrLine — реализация
static float DistPointToSegmentSquared(const Vector2& p, const Vector2& a, const Vector2& b) {
    Vector2 ab = { b.x - a.x, b.y - a.y };
    Vector2 ap = { p.x - a.x, p.y - a.y };
    float ab2 = ab.x * ab.x + ab.y * ab.y;
    if (ab2 == 0.0f) return (p.x - a.x) * (p.x - a.x) + (p.y - a.y) * (p.y - a.y);
    float t = (ap.x * ab.x + ap.y * ab.y) / ab2;
    if (t < 0.0f) t = 0.0f;
    else if (t > 1.0f) t = 1.0f;
    Vector2 proj = { a.x + ab.x * t, a.y + ab.y * t };
    float dx = p.x - proj.x, dy = p.y - proj.y;
    return dx * dx + dy * dy;
}
// adapter
static float Vector2DistanceSqrLine(const Vector2& p, const Vector2& a, const Vector2& b) {
    return DistPointToSegmentSquared(p, a, b);
}



int FindConnectionIndexAtPosition(const ScriptFile& file, const Vector2& mouse, float pickThreshold = 8.0f) {
    for (size_t i = 0; i < file.connections.size(); ++i) {
        const auto& c = file.connections[i];
        NodeBase* outNode = FindNodeById(const_cast<ScriptFile&>(file), c.outputNodeId);
        NodeBase* inNode = FindNodeById(const_cast<ScriptFile&>(file), c.inputNodeId);
        if (!outNode || !inNode) continue;
        Vector2 outPos = GetPortScreenPosition(*outNode, c.outputPortID);
        Vector2 inPos = GetPortScreenPosition(*inNode, c.inputPortID);
        float HANDLE_LENGTH = 50.0f * graphScale;
        Vector2 cp1 = { outPos.x + HANDLE_LENGTH, outPos.y };
        Vector2 cp2 = { inPos.x - HANDLE_LENGTH, inPos.y };
        float dist = DistancePointToBezierApprox(mouse, outPos, cp1, cp2, inPos, 24);
        if (dist <= pickThreshold) return (int)i;
    }
    return -1;
}
