#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <queue>
#include <limits>
#include <map>
#include <filesystem>
#include "PluginAPI.h"
#include "Node.h"
#include "PluginLoader.h"  // abstracts platform-specific includes

#include "ResourceManager.h"

#include "ScriptParser.h"

#include <thread>
#include <atomic>
namespace fs = std::filesystem;
// Состояние меню
struct ContextMenu {
    bool open = false;
    Vector2 pos = { 0,0 };             // экранная позиция открытия
    int targetNodeId = -1;           // если меню привязано к ноде
    std::vector<ContextMenuItem> items;
    int hoveredIndex = -1;
    float padding = 6.0f;
    float itemHeight = 20.0f;
    int fontSize = 12;
} g_contextMenu;

void UpdateContextMenu();
void DrawContextMenu();
void AutoArrangeNodes(ScriptFile& scriptFile, float startX, float startY, float xSpacing, float ySpacing);
// Открыть меню для ноды: устанавливаем позицию и targetNodeId, и наполняем items через BuildNodeContextMenu
void OpenNodeContextMenu(int nodeId, Vector2 screenPos);
void CloseContextMenu() { g_contextMenu.open = false; g_contextMenu.items.clear(); g_contextMenu.hoveredIndex = -1; }
void UpdateAndDrawActionPalette(const Rectangle& graphArea);

void ScanFosScripts(const std::string& folderPath);
void DrawScriptList();

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

void UpdateHoveredConnectionIndex();
void UpdateLayout(Rectangle* recs, int screenWidth, int screenHeight);
void GraphUpdate(Rectangle graphArea);
void GraphDraw(Rectangle graphArea);
Vector2 SnapToGrid(Vector2 pos);
static bool IsExecPort(const SPort& p);
void DrawNode(const NodeBase& node);
void NodeUpdate(NodeBase& node);
static float Vector2DistanceSqrLine(const Vector2& p, const Vector2& a, const Vector2& b);
static bool AreTypesCompatible(EVarType outType, EVarType inType);
static bool IsNumericType(EVarType t);

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


static std::vector<std::string> callingFiles;  // список файлов с вызовами текущей ноды
static Vector2 callFilesScroll = { 0, 0 };
static bool showCallersWindow = false;
static int selectedCallerIndex = -1;

// Drag state для нод
static int g_draggingNodeId = -1;
static Vector2 g_dragStartMouseWorld = { 0,0 };
static Vector2 g_dragStartNodePos = { 0,0 };
static int g_hoveredConnectionIndex = -1;
static int g_SelectedNode = -1;


static Vector2 WindowSize = { 1600, 900 };

const Vector2 WindowMinSize = { 600, 400 };

static float CONNECTOR_HANDLE_LENGTH = 150.0f;
static float BASE_CONNECTOR_THICKNESS = 4.f;
Settings g_settings;
// Хранилище найденных скриптов
static std::string ScriptFolder = "E:\\_work\\379\\sdk\\Server\\scripts";
static std::vector<std::string> fosFiles;
static int selectedFileIndex = -1;

// Область скролла
static Rectangle Browse_scrollPanelBounds = { 200, 100, 300, 300 }; // где рисуем панель
static Rectangle Browse_contentBounds = { 0, 0, 280, 0 }; // изменяемая по высоте
static Vector2 Browse_scroll = { 0, 0 };

// где-то на уровне модуля
static std::unordered_map<std::string, bool> g_categoryExpanded;


// Вспомогательные переменные для окна настроек
static bool showSettingsWindow = false;
static char folderInput[512] = {};
static float inputWindowWidth = 0, inputWindowHeight = 0;
static float inputConnectorHandleLength = 0, inputBaseConnectorThickness = 0;


std::atomic<bool> isFindingCallingFiles = false;
std::atomic<float> findProgress = 0.0f; // 0.0 - 1.0
std::vector<std::string> foundCallingFiles; // результат поиска
std::mutex foundFilesMutex; // для потокобезопасного доступа

Rectangle GraphRect;
static std::string lastFind_nodeName;

void LoadScript(std::string file);
// Функция поиска файлов, где вызывается nodeName
void FindCallingFiles(const std::string& nodeName);
void DrawCallingFilesWindow();

// Сохраняем настройки в файл
void SaveSettings(const std::string& filename) {
    json j = g_settings.to_json();
    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4); // с отступами для удобства
    }
}

// Загружаем настройки из файла
bool LoadSettings(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        json j;
        file >> j;
        g_settings.from_json(j);
        return true;
    }
    return false;
}

static void DrawSettingsWindow()
{
    if (!showSettingsWindow) return;

    // Задаём позицию и размер окна (можно настроить)
    Rectangle winBounds = { 400, 200, 600, 600 };
    DrawRectangle(winBounds.x, winBounds.y, winBounds.width, winBounds.height, WHITE);
    GuiGroupBox(winBounds, "Settings");

    // Отступы и позиции внутри окна
    int marginX = 20;
    int marginY = 30;
    float lineHeight = 25;
    float labelWidth = 140;
    float inputWidth = 300;

    float y = winBounds.y + marginY;

    // ScriptFolder input
    GuiLabel({ winBounds.x + marginX, y, labelWidth, lineHeight }, "Script Folder:");
    GuiTextBox({ winBounds.x + marginX + labelWidth, y, inputWidth, lineHeight }, folderInput, sizeof(folderInput), true);
    y += lineHeight + 10;

    static int mainMonitor = GetCurrentMonitor();
    static int monitorWidth = GetMonitorWidth(mainMonitor);
    static int monitorHeight = GetMonitorHeight(mainMonitor);

    int textHeight = 12; // размер шрифта
    Rectangle sliderRect1{ winBounds.x + marginX + labelWidth, y, inputWidth / 2 - 10, lineHeight };
    GuiSlider(sliderRect1, "Window Width", NULL, &inputWindowWidth, WindowMinSize.x, monitorWidth);
    const char* valText1 = TextFormat("%i", (int)inputWindowWidth);
    int textWidth1 = MeasureText(valText1, 12);
    float textX1 = sliderRect1.x + sliderRect1.width / 2 - textWidth1 / 2;
    float textY1 = sliderRect1.y + sliderRect1.height / 2 - textHeight / 2;
    DrawText(valText1, (int)textX1, (int)textY1, 12, GRAY);


    Rectangle sliderRect2 = { winBounds.x + marginX + labelWidth + inputWidth / 2 + labelWidth / 2 + 10, y, inputWidth / 2 - 10, lineHeight };
    GuiSlider(sliderRect2, "Window Height", NULL, &inputWindowHeight, WindowMinSize.y, monitorHeight);
    const char* valText2 = TextFormat("%i", (int)inputWindowHeight);
    int textWidth2 = MeasureText(valText2, 12);
    float textX2 = sliderRect2.x + sliderRect2.width / 2 - textWidth2 / 2;
    float textY2 = sliderRect2.y + sliderRect2.height / 2 - textHeight / 2;
    DrawText(valText2, (int)textX2, (int)textY2, 12, GRAY);
    y += lineHeight + 20;

    // CONNECTOR_HANDLE_LENGTH input
    GuiSlider(Rectangle{ winBounds.x + marginX + labelWidth, y, inputWidth, lineHeight }, "Connector Length", NULL, &inputConnectorHandleLength, 1, 500);
    DrawText(TextFormat("%.2f",inputConnectorHandleLength), winBounds.x + marginX + labelWidth + (inputWidth * 0.5), y + 8 , 12, GRAY);
    /*GuiLabel({ winBounds.x + marginX, y, labelWidth, lineHeight }, "Connector Length:");
    GuiValueBox({ winBounds.x + marginX + labelWidth, y, inputWidth, lineHeight }, "", &inputConnectorHandleLength, 10, 1000, true);*/
    y += lineHeight + 20;

    // BASE_CONNECTOR_THICKNESS input
    GuiSlider(Rectangle{ winBounds.x + marginX + labelWidth, y, inputWidth, lineHeight }, "Connector Thickness:", NULL, &inputBaseConnectorThickness, 1, 10);
    DrawText(TextFormat("%.2f", inputBaseConnectorThickness), winBounds.x + marginX + labelWidth + (inputWidth * 0.5), y + 8, 12, GRAY);
    //GuiLabel({ winBounds.x + marginX, y, labelWidth, lineHeight }, "Connector Thickness:");
    //GuiValueBox({ winBounds.x + marginX + labelWidth, y, inputWidth, lineHeight }, "", &inputBaseConnectorThickness, 1, 100, true);
    y += lineHeight + 20;

    // Кнопки внизу окна
    float btnWidth = 100;
    float btnHeight = 30;
    int btnSpacing = 20;
    float btnY = (winBounds.y + winBounds.height - marginY - btnHeight);

    // Apply
    if (GuiButton({ winBounds.x + marginX, btnY, btnWidth, btnHeight }, "Apply"))
    {
        // Применяем значения без закрытия окна
        ScriptFolder = folderInput;
        WindowSize.x = inputWindowWidth;
        WindowSize.y = inputWindowHeight;
        CONNECTOR_HANDLE_LENGTH = inputConnectorHandleLength;
        BASE_CONNECTOR_THICKNESS = inputBaseConnectorThickness;
    }

    // Ok
    if (GuiButton({ winBounds.x + marginX + btnWidth + btnSpacing, btnY, btnWidth, btnHeight }, "Ok"))
    {
        // Применяем и закрываем окно
        ScriptFolder = folderInput;
        WindowSize.x = inputWindowWidth;
        WindowSize.y = inputWindowHeight;
        CONNECTOR_HANDLE_LENGTH = inputConnectorHandleLength;
        BASE_CONNECTOR_THICKNESS = inputBaseConnectorThickness;
        showSettingsWindow = false;

        g_settings.baseConnectorThickness = BASE_CONNECTOR_THICKNESS;
        g_settings.connectorHandleLength = CONNECTOR_HANDLE_LENGTH;
        g_settings.scriptFolder = ScriptFolder;
        g_settings.windowWidth = WindowSize.x;
        g_settings.windowHeight = WindowSize.y;
        SaveSettings("config.json");
    }

    // Cancel
    if (GuiButton({ winBounds.x + marginX + 2 * (btnWidth + btnSpacing), btnY, btnWidth, btnHeight }, "Cancel"))
    {
        // Закрываем окно без сохранения изменений
        showSettingsWindow = false;
    }
}


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
    int NodeSize = 128;

    if (node.isPure)
        NodeSize = 64;
    float scaledNode = NodeSize * graphScale;
    float minWidth = scaledNode * 2;
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

static bool ConnectionExists(const ScriptFile& file, int outNodeId, int outPortId, int inNodeId, int inPortId)
{
    for (const auto& c : file.connections) {
        if (c.outputNodeId == outNodeId && c.outputPortID == outPortId &&
            c.inputNodeId == inNodeId && c.inputPortID == inPortId) return true;
    }
    return false;
}

void TryFinishConnectionDrag(ScriptFile& scriptFile) {
    if (!g_connDrag.active) return;
    Vector2 mouse = GetMousePosition();

    // Найдём исходную ноду/порт (откуда тянем)
    NodeBase* srcNode = FindNodeById(scriptFile, g_connDrag.nodeId);
    if (!srcNode) { CancelConnectionDrag(); return; }

    SPort srcPort{};
    bool foundSrc = false;
    for (const auto& p : srcNode->ports) {
        if (p.ID == g_connDrag.portId) { srcPort = p; foundSrc = true; break; }
    }
    if (!foundSrc) { CancelConnectionDrag(); return; }

    // Ищем цель под мышью
    for (auto& nodePtr : scriptFile.Nodes) {
        NodeBase* node = nodePtr.get();
        for (const auto& t : node->ports) {
            Vector2 ppos = GetPortScreenPosition(*node, t.ID);
            float radius = 6.0f * graphScale;
            if (!CheckCollisionPointCircle(mouse, ppos, radius)) continue;

            // избегаем соединения самого с собой (node+port)
            if (g_connDrag.nodeId == node->ID && g_connDrag.portId == t.ID) {
                CancelConnectionDrag();
                return;
            }

            // запрещаем соединять порт с портом той же ноды (опционально можно разрешить)
            if (srcNode->ID == node->ID) {
                // если хочешь разрешить внутринодовые соединения - убери этот блок
                CancelConnectionDrag();
                return;
            }

            // target port is 't'
            const SPort targetPort = t;

            bool srcIsExec = IsExecPort(srcPort);
            bool tgtIsExec = IsExecPort(targetPort);

            bool allowed = false;
            bool swapped = false;
            int outNodeId = -1, outPortId = -1, inNodeId = -1, inPortId = -1;

            // --- EXEC logic ---
            if (srcIsExec || tgtIsExec) {
                // оба должны быть EXEC
                if (srcIsExec && tgtIsExec) {
                    // стандарт: output -> input
                    if (!srcPort.isInput && targetPort.isInput) {
                        allowed = true;
                        outNodeId = srcNode->ID; outPortId = srcPort.ID;
                        inNodeId = node->ID;      inPortId = targetPort.ID;
                    }
                    // если начали drag с входа и кончаем на выходе — переворачиваем
                    else if (srcPort.isInput && !targetPort.isInput) {
                        allowed = true;
                        // swap ends: источник становится целью
                        outNodeId = node->ID;         outPortId = targetPort.ID;
                        inNodeId = srcNode->ID;       inPortId = srcPort.ID;
                    }
                }
                else {
                    // один из портов EXEC, другой нет — запрещаем
                    allowed = false;
                }
            }
            // --- value ports logic ---
            else {
                // output -> input
                if (!srcPort.isInput && targetPort.isInput) {
                    if (AreTypesCompatible(srcPort.type, targetPort.type)) {
                        allowed = true;
                        outNodeId = srcNode->ID; outPortId = srcPort.ID;
                        inNodeId = node->ID;      inPortId = targetPort.ID;
                    }
                }
                // started from input and dropped on output -> flip
                else if (srcPort.isInput && !targetPort.isInput) {
                    if (AreTypesCompatible(targetPort.type, srcPort.type)) {
                        allowed = true;
                        outNodeId = node->ID;    outPortId = targetPort.ID;
                        inNodeId = srcNode->ID;  inPortId = srcPort.ID;
                    }
                }
            }

            // Если разрешено — добавляем соединение (но проверим дубль)
            if (allowed) {
                if (!ConnectionExists(scriptFile, outNodeId, outPortId, inNodeId, inPortId)) {
                    ConnectPorts(scriptFile, outNodeId, outPortId, inNodeId, inPortId);
                }
                else {
                    // уже есть - можно проигнорировать или обновить
                }
            }
            else {
                // можно дать фидбек (звук/ошибка)
            }

            CancelConnectionDrag();
            return;
        }
    }

    // если цель не найдена — отменяем drag
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

    // --- Сброс глобальных состояний ---
    if (g_draggingNodeId == nodeId) {
        g_draggingNodeId = -1;
    }
    if (g_connDrag.active && g_connDrag.nodeId == nodeId) {
        g_connDrag.active = false;
    }
}



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

static Rectangle MakeBezierBounds(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3) {
    float minx = std::min(std::min(p0.x, p1.x), std::min(p2.x, p3.x));
    float maxx = std::max(std::max(p0.x, p1.x), std::max(p2.x, p3.x));
    float miny = std::min(std::min(p0.y, p1.y), std::min(p2.y, p3.y));
    float maxy = std::max(std::max(p0.y, p1.y), std::max(p2.y, p3.y));
    return Rectangle{ minx, miny, maxx - minx, maxy - miny };
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



static bool IsNumericType(EVarType t) {
    return t == EVarType::Int || t == EVarType::UInt || t == EVarType::UInt8 || t == EVarType::Float;
}

static bool IsExecPort(const SPort& p) {
    if (p.type == EVarType::EXEC) return true;

    // fallback: если тип почему-то не выставлен, попробуем по имени
    std::string lower = p.name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.find("exec") != std::string::npos) return true;
    if (lower == "true" || lower == "false") return true; // для Branch
    // можно ещё сопоставить с "ExecIn"/"ExecOut" или с префиксом
    return false;
}

static bool AreTypesCompatible(EVarType outType, EVarType inType) {
    if (outType == inType) return true;
    if (outType == EVarType::EXEC || inType == EVarType::EXEC) return (outType == EVarType::EXEC && inType == EVarType::EXEC);

    // разрешаем присваивание чисел между собой (инт->флоат и т.п.)
    if (IsNumericType(outType) && IsNumericType(inType)) return true;
    // можно расширить правила: Item->ItemCl, Critter& -> Critter и т.д.
    return false;
}


static Color HexToColor(const std::string& hex) {
    std::string h = hex;
    if (!h.empty() && h[0] == '#') h = h.substr(1);
    // ожидаем 6 символов
    if (h.size() != 6) return WHITE;
    int r = 0, g = 0, b = 0;
    try {
        r = std::stoi(h.substr(0, 2), nullptr, 16);
        g = std::stoi(h.substr(2, 2), nullptr, 16);
        b = std::stoi(h.substr(4, 2), nullptr, 16);
    }
    catch (...) {
        return WHITE;
    }
    return Color{ (unsigned char)r, (unsigned char)g, (unsigned char)b, 255 };
}

// Хелпер: среднее/смешивание двух цветов
static Color MixColor(const Color& a, const Color& b) {
    unsigned char r = (unsigned char)(((int)a.r + (int)b.r) / 2);
    unsigned char g = (unsigned char)(((int)a.g + (int)b.g) / 2);
    unsigned char bl = (unsigned char)(((int)a.b + (int)b.b) / 2);
    return Color{ r, g, bl, 255 };
}

// Мапа: EVarType -> hex string (на основе данных, которые дал)
static Color ColorForVarType(EVarType t) {
    static const std::unordered_map<EVarType, std::string> mapHex = {
        { EVarType::Int,     "#00ff7b" },
        { EVarType::UInt,    "#aaffaa" },
        { EVarType::UInt16,    "#c8ffab" },
        { EVarType::UInt8,   "#256b62" },
        { EVarType::Float,   "#0fff37" },
        { EVarType::Bool,    "#ff0000" },
        { EVarType::String,  "#e600ff" },
        { EVarType::Critter, "#00c8ff" },
        { EVarType::CritterCl,"#0d99bf"},
        { EVarType::ProtoItem,"#7293d6"},
        { EVarType::Item,    "#0551ad" },
        { EVarType::ItemCl,  "#0073ff" }
    };

    auto it = mapHex.find(t);
    if (it != mapHex.end()) return HexToColor(it->second);
    return WHITE;
}



// Helper trim / split
static inline std::string Trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static inline std::vector<std::string> SplitCategoryPath(const std::string& path) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : path) {
        if (c == '|') {
            std::string t = Trim(cur);
            if (!t.empty()) out.push_back(t);
            cur.clear();
        }
        else cur.push_back(c);
    }
    std::string t = Trim(cur);
    if (!t.empty()) out.push_back(t);
    return out;
}

static std::unique_ptr<CatNode> BuildCategoryTree();
static bool BuildVisibleListRec(CatNode* node, const std::string& queryLower, std::vector<VisibleEntry>& out, int depth);
static bool BuildVisibleList_Final(CatNode* node, const std::string& queryLower, std::vector<VisibleEntry>& out, int depth);
static std::unique_ptr<CatNode> BuildCategoryTreeFiltered(const std::string& queryLower);
static void FillVisibleListFromTree(CatNode* node, std::vector<VisibleEntry>& out, int depth);
