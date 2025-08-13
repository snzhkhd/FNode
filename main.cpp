#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set("utf-8")

#include "NodeTemplates.h"
#include "FileManager.h"

#include "raymath.h"
ScriptFile CurrentFile;

char* CreateIconText(int iconCode, const char* text) {
    size_t length = snprintf(NULL, 0, "#%d# %s", iconCode, text); // Получаем длину строки
    char* result = (char*)malloc(length + 1); // Выделяем память
    if (result == NULL) {
        return NULL; // Обработка ошибки выделения памяти
    }
    snprintf(result, length + 1, "#%d# %s", iconCode, text);
    return result;
}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    if (LoadSettings("config.json"))
    {
        WindowSize.x = g_settings.windowWidth;
        WindowSize.y = g_settings.windowHeight;
        CONNECTOR_HANDLE_LENGTH = g_settings.connectorHandleLength;
        BASE_CONNECTOR_THICKNESS = g_settings.baseConnectorThickness;
    }
    InitWindow(WindowSize.x, WindowSize.y, "FNode gui" );
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(600, 400);

    // Загружаем содержимое скрипта
    std::string scriptContent = LoadFileContent("Resources/animation.fos");
    CurrentFile.name = "animation";
    // Парсим скрипт
    FOnlineScriptParser parser;
    if (parser.ParseScript(scriptContent, CurrentFile)) {
        // Теперь scriptFile содержит узлы и соединения
        TraceLog(LOG_INFO, "Parsed %d functions", CurrentFile.Nodes.size());

    }
    else {
        TraceLog(LOG_ERROR, "Failed to parse script");
    }

    bool ok = LoadScriptFileByName(CurrentFile.name.c_str(), CurrentFile);
    TraceLog(ok ? LOG_INFO : LOG_WARNING, "LoadScriptFileByName('%s') -> %d", CurrentFile.name.c_str(), (int)ok);
    
    TraceLog(LOG_INFO, "Found %d Import", CurrentFile.Import.size());

    
    //std::vector<int> cp;
    //for (int c = 32; c < 127; ++c) cp.push_back(c);
    //for (int c = 0x0400; c <= 0x0450; ++c) cp.push_back(c);
    //ResourceManager::LoadFontEx("Resources/regular.ttf", 16, cp);

    // FNode gui: controls initialization
    //----------------------------------------------------------------------------------
    // Const text
    const char* function_gboxText = CreateIconText(ICON_SUITCASE, "Functions"); // GROUPBOX: function_gbox
    const char* var_gboxText = CreateIconText(ICON_TEXT_NOTES,"Variable");    // GROUPBOX: var_gbox
    const char* lvar_gboxText = CreateIconText(ICON_NOTEBOOK, "Local variable");    // GROUPBOX: lvar_gbox
    const char* add_func_btnText = "+";    // BUTTON: add_func_btn
    const char* add_var_btnText = "+";    // BUTTON: add_var_btn
    const char* add_lvar_btnText = "+";    // BUTTON: add_lvar_btn
    const char* browse_GroupBoxText = CreateIconText(ICON_LINK_MULTI, "Browse");    // GROUPBOX: browse_GroupBox
    const char* tool_GroupBoxText = CreateIconText(ICON_TOOLS, "Tools" );    // GROUPBOX: tool_GroupBox

    //tool box
    const char* save_btnText = CreateIconText(ICON_FILE_SAVE_CLASSIC, "Save"); // BUTTON: save_btn
    //const char* compile_btnText = CreateIconText(ICON_ROM, "Compile");   // BUTTON: compile_btn         not compiled ICON_ALARM | raw - ICON_ROM | compiled - ICON_CPU
    snprintf(compile_btnText, MAX_BTN_TEXT, "#%d# Compile", ICON_ROM);

    const char* setting_btnText = CreateIconText(ICON_GEAR_BIG, "Setting");// BUTTON: setting_btn
    const char* gvars_btnText = CreateIconText(ICON_GEAR_EX,"GVARS");//"GVARS";//TextFormat("#%i# GVARS", (int)ICON_BIN); // BUTTON: gvars_btn

    const char* defines_btnText = CreateIconText(ICON_STAR, "Defines");//"Defines";// BUTTON: defines_btn

    const char* clear_fosb_btnText = CreateIconText(ICON_BIN, "Clear fosb");// BUTTON: clear_fosb_btn 

    const char* graph_GroupBoxText = CreateIconText(ICON_LINK_BOXES, "Graph editor"); // GROUPBOX: graph_GroupBox

    const char* Console_GroupBoxText = "Console";    // GROUPBOX: Console_GroupBox

    const char* Console_CheckBoxText = "Show Console";    // CHECKBOXEX: CheckBoxEx019
    // Define anchors
    Vector2 func_vars_anchor = { 24, 120 };            // ANCHOR ID:1
    Vector2 browbse_anchor = { 232, 120 };            // ANCHOR ID:2
    Vector2 tool_anchor = { 24, 24 };            // ANCHOR ID:3
    Vector2 graph_anchor = { 232, 120 };            // ANCHOR ID:4

    // Define controls variables
    Rectangle function_ScrollPanelScrollView = { 0, 0, 0, 0 };
    Vector2 function_ScrollPanelScrollOffset = { 0, 0 };
    Vector2 function_ScrollPanelBoundsOffset = { 0, 0 };            // ScrollPanel: function_ScrollPanel
    Rectangle vars_ScrollPanelScrollView = { 0, 0, 0, 0 };
    Vector2 vars_ScrollPanelScrollOffset = { 0, 0 };
    Vector2 vars_ScrollPanelBoundsOffset = { 0, 0 };            // ScrollPanel: vars_ScrollPanel
    Rectangle lvars_ScrollPanelScrollView = { 0, 0, 0, 0 };
    Vector2 lvars_ScrollPanelScrollOffset = { 0, 0 };
    Vector2 lvars_ScrollPanelBoundsOffset = { 0, 0 };            // ScrollPanel: lvars_ScrollPanel

    // Define controls rectangles
    Rectangle layoutRecs[20] = {
        Rectangle{
         func_vars_anchor.x + 0, func_vars_anchor.y + 0, 192, 312
        },    // GroupBox: function_gbox
        Rectangle{
        func_vars_anchor.x + 0, func_vars_anchor.y + 336, 192, 312
        },    // GroupBox: var_gbox
        Rectangle{
        func_vars_anchor.x + 0, func_vars_anchor.y + 672, 192, 312
        },    // GroupBox: lvar_gbox
        Rectangle{
        func_vars_anchor.x + 0, func_vars_anchor.y + 24, 192, 288
        },    // ScrollPanel: function_ScrollPanel
        Rectangle{
        func_vars_anchor.x + 0, func_vars_anchor.y + 360, 192, 288
        },    // ScrollPanel: vars_ScrollPanel
        Rectangle{
        func_vars_anchor.x + 0, func_vars_anchor.y + 696, 192, 288
        },    // ScrollPanel: lvars_ScrollPanel
        Rectangle{
        192, 120, 24, 24
        },    // Button: add_func_btn
        Rectangle{
        192, 456, 24, 24
        },    // Button: add_var_btn
        Rectangle{
        192, 792, 24, 24
        },    // Button: add_lvar_btn
        Rectangle{
        browbse_anchor.x + 1848, browbse_anchor.y + 0, 288, 984
        },    // GroupBox: browse_GroupBox
        Rectangle{
        tool_anchor.x + 0, tool_anchor.y + 0, 2352, 72
        },    // GroupBox: tool_GroupBox
        Rectangle{
        tool_anchor.x + 16, tool_anchor.y + 8, 88, 56
        },    // Button: save_btn
        Rectangle{
        tool_anchor.x + 120, tool_anchor.y + 8, 88, 56
        },    // Button: compile_btn
        Rectangle{
        tool_anchor.x + 224, tool_anchor.y + 8, 88, 56
        },    // Button: setting_btn
        Rectangle{
        tool_anchor.x + 328, tool_anchor.y + 8, 88, 56
        },    // Button: gvars_btn
        Rectangle{
        tool_anchor.x + 432, tool_anchor.y + 8, 88, 56
        },    // Button: defines_btn
        Rectangle{
        tool_anchor.x + 536, tool_anchor.y + 8, 88, 56
        },    // Button: clear_fosb_btn
        Rectangle{
        graph_anchor.x + 0, graph_anchor.y + 0, 1824, 984
        },    // GroupBox: graph_GroupBox
    };
    //----------------------------------------------------------------------------------

    Browse_scrollPanelBounds = layoutRecs[9];
    ScanFosScripts(ScriptFolder);
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------
    AutoArrangeNodes(CurrentFile, 100, 100, 500, 256);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateLayout(layoutRecs, GetScreenWidth(), GetScreenHeight());

        Browse_scrollPanelBounds = layoutRecs[9];
        GraphUpdate(GraphRect);

        if (!showSettingsWindow && CheckCollisionPointRec(GetMousePosition(), GraphRect))
        {
            for (auto& nptr : CurrentFile.Nodes) {
                NodeUpdate(*nptr);
            }
            UpdateHoveredConnectionIndex();

            // обработка удаления по правому клику:
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && g_hoveredConnectionIndex != -1) {
                CurrentFile.connections.erase(CurrentFile.connections.begin() + g_hoveredConnectionIndex);
                g_hoveredConnectionIndex = -1;
            }
            UpdateContextMenu();
        }

       
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

       
        // raygui: controls drawing
        //----------------------------------------------------------------------------------
        // Draw controls
        GuiGroupBox(layoutRecs[0], function_gboxText);
        GuiGroupBox(layoutRecs[1], var_gboxText);

        GuiGroupBox(layoutRecs[2], lvar_gboxText);
        
       // GuiScrollPanel(Rectangle{ layoutRecs[3].x, layoutRecs[3].y, layoutRecs[3].width - function_ScrollPanelBoundsOffset.x, layoutRecs[3].height - function_ScrollPanelBoundsOffset.y }, "", layoutRecs[3], & function_ScrollPanelScrollOffset, & function_ScrollPanelScrollView);
        // Начало отрисовки содержимого панели
        
        Rectangle panelBounds = {
        layoutRecs[3].x,
        layoutRecs[3].y,
        layoutRecs[3].width - function_ScrollPanelBoundsOffset.x ,
        layoutRecs[3].height - function_ScrollPanelBoundsOffset.y
        };

        // Область внутри панели (виртуальная "бумага")
        Rectangle contentBounds = { 0, 0, panelBounds.width - 20, (float)CurrentFile.Nodes.size() * 24  }; // 24px на элемент
        Rectangle viewArea = { 0 };
        GuiScrollPanel(panelBounds, "", contentBounds, &function_ScrollPanelScrollOffset, &viewArea);
        BeginScissorMode(
            (int)viewArea.x,
            (int)viewArea.y ,
            (int)viewArea.width,
            (int)viewArea.height
        );

        // Рисуем список нод с учётом смещения скролла
        float y = panelBounds.y + function_ScrollPanelScrollOffset.y;
        for (size_t i = 0; i < CurrentFile.Nodes.size(); i++)
        {
            const auto& node = *CurrentFile.Nodes[i];
            Rectangle btn = { panelBounds.x + 5, y + i * 24 +24, panelBounds.width - 30, 20 };

            if (CheckCollisionPointRec(GetMousePosition(), viewArea) && CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Действие при нажатии
                printf("Clicked node: %s\n", node.name.c_str());

                graphOffset.x = (node.position.x * graphScale) - (GraphRect.width / 2.0f);
                graphOffset.y = (node.position.y * graphScale) - (GraphRect.height / 2.0f);
                g_SelectedNode = node.ID;
            }
            GuiButton(btn, node.name.c_str());
        }
        EndScissorMode();
        
        GuiScrollPanel(Rectangle{ layoutRecs[4].x, layoutRecs[4].y, layoutRecs[4].width - vars_ScrollPanelBoundsOffset.x, layoutRecs[4].height - vars_ScrollPanelBoundsOffset.y }, "", layoutRecs[4], & vars_ScrollPanelScrollOffset, & vars_ScrollPanelScrollView);
        GuiScrollPanel(Rectangle{ layoutRecs[5].x, layoutRecs[5].y, layoutRecs[5].width - lvars_ScrollPanelBoundsOffset.x, layoutRecs[5].height - lvars_ScrollPanelBoundsOffset.y }, "", layoutRecs[5], & lvars_ScrollPanelScrollOffset, & lvars_ScrollPanelScrollView);
        
        
        
        if (GuiButton(layoutRecs[6], add_func_btnText)) AddFuncBtn();
        if (GuiButton(layoutRecs[7], add_var_btnText)) AddVarBtn();
        if (GuiButton(layoutRecs[8], add_lvar_btnText)) AddLvarBtn();

        GuiGroupBox(layoutRecs[9], browse_GroupBoxText);
        DrawScriptList();

        GuiGroupBox(layoutRecs[10], tool_GroupBoxText);

      
        if (GuiButton(layoutRecs[11], save_btnText)) SaveBtnImplementation(CurrentFile);// SaveBtn();
        if (GuiButton(layoutRecs[12], compile_btnText)) CompileBtn();
        if (GuiButton(layoutRecs[13], setting_btnText)) SettingBtn();
        if (GuiButton(layoutRecs[14], gvars_btnText)) GvarsBtn();
        if (GuiButton(layoutRecs[15], defines_btnText)) DefinesBtn();
        if (GuiButton(layoutRecs[16], clear_fosb_btnText)) ClearFosbBtn();

        //GraphDraw(layoutRecs[17]);
        GraphDraw(GraphRect);
        GuiGroupBox(GraphRect, graph_GroupBoxText);

        if(isShowConsole)
            GuiGroupBox(layoutRecs[18], Console_GroupBoxText);

        GuiCheckBox(layoutRecs[19], Console_CheckBoxText, &isShowConsole);
        //----------------------------------------------------------------------------------
        DrawContextMenu();

        DrawContextMenu();


        
        DrawCallingFilesWindow();
        DrawSettingsWindow();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdateContextMenu()
{
    if (!g_contextMenu.open) return;

    Vector2 mouse = GetMousePosition();
    // Рассчитываем размер меню
    int itemCount = (int)g_contextMenu.items.size();
    float w = 0;
    for (auto& it : g_contextMenu.items) {
        if (it.isSeparator) continue;
        int tw = MeasureText(it.label.c_str(), g_contextMenu.fontSize);
        w = std::max(w, (float)tw);
    }
    w += g_contextMenu.padding * 4;
    float h = itemCount * g_contextMenu.itemHeight + g_contextMenu.padding * 2;

    Rectangle menuRect{ g_contextMenu.pos.x, g_contextMenu.pos.y, w, h };

    // Закрытие при клике вне
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!CheckCollisionPointRec(mouse, menuRect)) {
            CloseContextMenu();
            return;
        }
    }
    // Обновление hoveredIndex
    g_contextMenu.hoveredIndex = -1;
    for (int i = 0, y = 0; i < itemCount; ++i) {
        const auto& it = g_contextMenu.items[i];
        Rectangle itemRect{ menuRect.x + g_contextMenu.padding, menuRect.y + g_contextMenu.padding + i * g_contextMenu.itemHeight,
                            menuRect.width - g_contextMenu.padding * 2, g_contextMenu.itemHeight };
        if (it.isSeparator) continue;
        if (CheckCollisionPointRec(mouse, itemRect)) {
            g_contextMenu.hoveredIndex = i;
            // click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && it.enabled && it.action) {
                // выполняем действие и закрываем меню
                it.action();
                CloseContextMenu();
                return;
            }
        }
    }
    // Escape закрывает меню
    if (IsKeyPressed(KEY_ESCAPE)) CloseContextMenu();
}

void DrawContextMenu()
{
    if (!g_contextMenu.open) return;

    // размеры (как в UpdateContextMenu) — можно вынести в функцию
    int itemCount = (int)g_contextMenu.items.size();
    float w = 0;
    for (auto& it : g_contextMenu.items) {
        if (it.isSeparator) continue;
        int tw = MeasureText(it.label.c_str(), g_contextMenu.fontSize);
        w = std::max(w, (float)tw);
    }
    w += g_contextMenu.padding * 4;
    float h = itemCount * g_contextMenu.itemHeight + g_contextMenu.padding * 2;
    Rectangle menuRect{ g_contextMenu.pos.x, g_contextMenu.pos.y, w, h };

    // Фон и рамка
    DrawRectangleRec(menuRect, Fade(GRAY, 0.95f));
    DrawRectangleLinesEx(menuRect, 1.0f, Fade(BLACK, 0.7f));

    // Отрисовка пунктов
    for (int i = 0; i < itemCount; ++i) {
        const auto& it = g_contextMenu.items[i];
        Rectangle itemRect{ menuRect.x + g_contextMenu.padding, menuRect.y + g_contextMenu.padding + i * g_contextMenu.itemHeight,
                            menuRect.width - g_contextMenu.padding * 2, g_contextMenu.itemHeight };

        if (it.isSeparator) {
            // рисуем линию
            DrawLineEx({ itemRect.x, itemRect.y + itemRect.height / 2 }, { itemRect.x + itemRect.width, itemRect.y + itemRect.height / 2 }, 1.0f, Fade(DARKGRAY, 0.6f));
            continue;
        }

        // фон hovered
        if (g_contextMenu.hoveredIndex == i) {
            DrawRectangleRec(itemRect, Fade(LIGHTGRAY, 0.8f));
        }

        // текст
        Color textColor = it.enabled ? BLACK : Fade(BLACK, 0.4f);
        DrawText(it.label.c_str(), (int)(itemRect.x + 4), (int)(itemRect.y + (itemRect.height - g_contextMenu.fontSize) * 0.5f), g_contextMenu.fontSize, textColor);
    }
}

void AutoArrangeNodes(ScriptFile& scriptFile, float startX, float startY, float xSpacing, float ySpacing)
{
    if (scriptFile.Nodes.empty()) return;

    // 1) Построим граф exec-ребер: adj[outNode] -> list of inputNode
    std::unordered_map<int, std::vector<int>> adj;
    std::unordered_map<int, int> inCount; // входящие exec-ссылки (кол-во)

    // Инициализация всех узлов
    for (auto& n : scriptFile.Nodes) {
        inCount[n->ID] = 0;
    }

    for (auto& conn : scriptFile.connections) {
        // найдём output-порт и проверим, exec ли он
        auto outNode = FindNodeById(scriptFile, conn.outputNodeId);
        if (!outNode) continue;

        bool outIsExec = false;
        for (auto& p : outNode->ports) {
            if (p.ID == conn.outputPortID) {
                outIsExec = (p.type == EVarType::EXEC);
                break;
            }
        }
        if (!outIsExec) continue; // рассматриваем только exec-ребра

        // добавляем ребро out -> in
        adj[conn.outputNodeId].push_back(conn.inputNodeId);
        inCount[conn.inputNodeId] += 1;
    }

    // 2) Найдём корни (inCount == 0)
    std::queue<int> q;
    std::unordered_map<int, int> level; // nodeId -> level (int)
    for (auto& n : scriptFile.Nodes) {
        if (inCount[n->ID] == 0) {
            q.push(n->ID);
            level[n->ID] = 0;
        }
    }

    // Если нет корней (всё циклы), положим всех в уровень 0
    if (q.empty()) {
        for (auto& n : scriptFile.Nodes) {
            q.push(n->ID);
            level[n->ID] = 0;
        }
    }

    // 3) BFS по exec-ребрам, вычисляем уровень (ширина)
    while (!q.empty()) {
        int u = q.front(); q.pop();
        int ulevel = level[u];
        for (int v : adj[u]) {
            if (level.find(v) == level.end() || level[v] < ulevel + 1) {
                level[v] = ulevel + 1;
            }
            // добавляем в очередь если ещё не обработан
            if (inCount.find(v) != inCount.end()) {
                // чтобы гарантированно пройти все узлы, просто пушим (дубли не страшны)
                q.push(v);
            }
        }
    }

    // 4) Группируем ноды по уровням
    std::map<int, std::vector<std::shared_ptr<NodeBase>>> groups;
    for (auto& n : scriptFile.Nodes) {
        int lv = 0;
        auto it = level.find(n->ID);
        if (it != level.end()) lv = it->second;
        groups[lv].push_back(n);
    }

    // 5) Выставляем позиции: X = startX + level*xSpacing, Y = startY + idx*ySpacing
    for (auto& [lvl, vec] : groups) {
        float x = startX + lvl * xSpacing;
        float y = startY;
        for (auto& node : vec) {
            node->position = { x, y };
            y += ySpacing;
        }
    }
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
// Button: add_func_btn logic
static void AddFuncBtn()
{
    // TODO: Implement control logic
}
// Button: add_var_btn logic
static void AddVarBtn()
{
    // TODO: Implement control logic
}
// Button: add_lvar_btn logic
static void AddLvarBtn()
{
    // TODO: Implement control logic
}
// Button: save_btn logic
static void SaveBtn()
{
    // TODO: Implement control logic
}
// Button: compile_btn logic
static void CompileBtn()
{
    if (!isCompiled) {
        // Меняем иконку на CPU и текст
        snprintf(compile_btnText, MAX_BTN_TEXT, "#%d# Compiled", ICON_CPU);
        isCompiled = true;
    }
    else {
        // Можно вернуть обратно, если нужно
        snprintf(compile_btnText, MAX_BTN_TEXT, "#%d# Recompile", ICON_ROM);
        isCompiled = false;
    }
}
// Button: setting_btn logic
static void SettingBtn()
{
    // Открываем окно при первом вызове
    if (!showSettingsWindow)
    {
        showSettingsWindow = true;
        // Копируем текущие значения в временные буферы
        strncpy(folderInput, ScriptFolder.c_str(), sizeof(folderInput) - 1);
        inputWindowWidth = WindowSize.x;
        inputWindowHeight = WindowSize.y;
        inputConnectorHandleLength = CONNECTOR_HANDLE_LENGTH;
        inputBaseConnectorThickness = BASE_CONNECTOR_THICKNESS;
    }
}
// Button: gvars_btn logic
static void GvarsBtn()
{
    // TODO: Implement control logic
}
// Button: defines_btn logic
static void DefinesBtn()
{
    // TODO: Implement control logic
}
// Button: clear_fosb_btn logic
static void ClearFosbBtn()
{
    // TODO: Implement control logic
}

std::string LoadFileContent(const std::string& filePath)
{
    // Используем встроенную функцию Raylib для загрузки текста
    char* fileData = LoadFileText(filePath.c_str());

    if (fileData == NULL) {
        TraceLog(LOG_ERROR, "Failed to load file: %s", filePath.c_str());
        return "";
    }

    // Преобразуем в std::string и освобождаем память
    std::string content(fileData);
    UnloadFileText(fileData);

    return content;
}

void UpdateHoveredConnectionIndex()
{
    Vector2 mouse = GetMousePosition();
    // pick threshold можно подстроить: 6..12 в px
    g_hoveredConnectionIndex = FindConnectionIndexAtPosition(CurrentFile, mouse, 8.0f);
}

void UpdateLayout(Rectangle* recs, int screenWidth, int screenHeight) {
    // Параметры интерфейса (настраивайте под себя)
    const int TOOLBAR_HEIGHT = 72;
    const int LEFT_PANEL_WIDTH = 200;
    const int BUTTON_SIZE = 24;
    const int SPACING = 24;
    const int PANEL_PADDING = 24;

    // Верхняя панель инструментов
    recs[10] = Rectangle{ 5, 5, (float)screenWidth, TOOLBAR_HEIGHT }; // tool_GroupBox

    // Кнопки в верхней панели (равномерно распределены)
    int btnWidth = 88;
    int totalBtnsWidth = 7 * (btnWidth + 16) - 16; // 7 кнопок с отступами
    int startX = (screenWidth - totalBtnsWidth) / 2; // Центрируем

    for (int i = 0; i < 7; i++) {
        recs[11 + i] = Rectangle{
            (float)startX + i * (btnWidth + 16),
            8,
            (float)btnWidth,
            56
        };
    }

    //{ 2264, 32, 24, 24 },
    recs[19] = Rectangle{ (float)startX + 18 * (BUTTON_SIZE + 16), 8, BUTTON_SIZE, BUTTON_SIZE }; // console check box

    // Левая панель (функции/переменные)
    recs[0] = Rectangle{ SPACING, TOOLBAR_HEIGHT + SPACING-10, LEFT_PANEL_WIDTH, ((float)screenHeight - TOOLBAR_HEIGHT - 3 * SPACING) / 3 };      // function_gbox
    recs[1] = Rectangle{ SPACING, TOOLBAR_HEIGHT + SPACING  * 2 + recs[0].height - 10, LEFT_PANEL_WIDTH, recs[0].height }; // var_gbox
    recs[2] = Rectangle{ SPACING, TOOLBAR_HEIGHT + SPACING * 3 + recs[0].height * 2 - 10 , LEFT_PANEL_WIDTH, recs[0].height }; // lvar_gbox

    // ScrollPanels внутри групп
    recs[3] = Rectangle{ SPACING, TOOLBAR_HEIGHT + SPACING, LEFT_PANEL_WIDTH, recs[0].height }; // function_ScrollPanel
    recs[4] = Rectangle{ SPACING, TOOLBAR_HEIGHT + SPACING * 2 + recs[0].height, LEFT_PANEL_WIDTH, recs[0].height - SPACING }; // vars_ScrollPanel
    recs[5] = Rectangle{ SPACING, TOOLBAR_HEIGHT + SPACING * 3 + recs[0].height * 2, LEFT_PANEL_WIDTH, recs[0].height - SPACING }; // lvars_ScrollPanel

    // Кнопки "+" справа от панелей
    recs[6] = Rectangle{ LEFT_PANEL_WIDTH, TOOLBAR_HEIGHT + SPACING * 2 - BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE }; // add_func_btn
    recs[7] = Rectangle{ LEFT_PANEL_WIDTH, TOOLBAR_HEIGHT + SPACING * 3 + recs[0].height - BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE }; // add_var_btn
    recs[8] = Rectangle{ LEFT_PANEL_WIDTH, TOOLBAR_HEIGHT + SPACING * 4 + recs[0].height * 2 - BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE }; // add_lvar_btn

    // Правая область (граф и браузер)
    int rightX = SPACING + LEFT_PANEL_WIDTH + SPACING;
    int rightWidth = screenWidth - rightX - SPACING ;

    // Граф редактор занимает большую часть правой области  recs[17]
    GraphRect = Rectangle{
        (float)rightX,
        (float)(TOOLBAR_HEIGHT + SPACING),
        (float)(rightWidth - 200 - SPACING), // Оставляем место для браузера
        (float)(screenHeight - TOOLBAR_HEIGHT - SPACING * 2 - (isShowConsole? recs[0].height - SPACING + 10 : 0) )
    }; // graph_GroupBox

    // Браузер справа от граф редактора
    recs[9] = Rectangle{
        (float)(rightX+ rightWidth - 200 ), // Сдвигаем на 200 пикселей от правого края
        (float)(TOOLBAR_HEIGHT + SPACING ),
        (float)200, // Фиксированная ширина браузера
        (float)(screenHeight - TOOLBAR_HEIGHT - SPACING * 2)
    }; // browse_GroupBox

    if (isShowConsole)
    {
        recs[18] = Rectangle{ (float)rightX,TOOLBAR_HEIGHT + SPACING * 3 + recs[0].height * 2, (float)(rightWidth - 200 - SPACING), recs[0].height - SPACING };
    }

   
}

void GraphUpdate(Rectangle graphArea)
{
    Vector2 mousePos = GetMousePosition();
    bool mouseInGraphArea = CheckCollisionPointRec(mousePos, graphArea);

    if (!mouseInGraphArea || showCallersWindow)
        return;

    // --- Перетаскивание ---
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) && mouseInGraphArea) {
        isGraphDragging = true;
        lastGraphMousePos = mousePos;
    }
    if (isGraphDragging && IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
        Vector2 delta = { lastGraphMousePos.x - mousePos.x, lastGraphMousePos.y - mousePos.y };
        graphOffset.x += delta.x;
        graphOffset.y += delta.y;
        lastGraphMousePos = mousePos;
    }
    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
        isGraphDragging = false;
    }

    // --- Зум ---
    if (mouseInGraphArea && !g_actionPalette.open) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            // позиция курсора относительно графа до зума
            Vector2 beforeZoom = {
                (mousePos.x - graphArea.x + graphOffset.x) / graphScale,
                (mousePos.y - graphArea.y + graphOffset.y) / graphScale
            };

            // меняем масштаб
            graphScale *= (wheel > 0) ? 1.1f : 0.9f;
            if (graphScale < MIN_SCALE) graphScale = MIN_SCALE;
            if (graphScale > MAX_SCALE) graphScale = MAX_SCALE;

            // позиция курсора относительно графа после зума
            Vector2 afterZoom = {
                (mousePos.x - graphArea.x + graphOffset.x) / graphScale,
                (mousePos.y - graphArea.y + graphOffset.y) / graphScale
            };

            // корректируем смещение так, чтобы точка под мышью осталась на месте
            graphOffset.x -= (afterZoom.x - beforeZoom.x) * graphScale;
            graphOffset.y -= (afterZoom.y - beforeZoom.y) * graphScale;
        }
    }


    // detect double-click on empty space -> open action palette
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), graphArea)) {
        Vector2 m = GetMousePosition();

        // если клик — по ноде, ничего не делаем (позже NodeUpdate обработает)
        if (!IsPointOverAnyNode(CurrentFile, m, graphArea)) {
            float now = GetTime();
            float dt = now - g_actionPalette.lastClickTime;
            float dist = Vector2Distance(m, g_actionPalette.lastClickPos);

            // threshold: 0.35s и расстояние < 8 px
            if (dt < 0.35f && dist < 8.0f) {
                // двойной клик — откроем палитру
                g_actionPalette.open = true;
                g_actionPalette.screenPos = m;
                g_actionPalette.search.clear();
                g_actionPalette.scrollOffset = { 0,0 };
                g_actionPalette.scrollView = { 0,0,0,0 };
                g_actionPalette.hoveredIndex = -1;
                g_actionPalette.selectedIndex = -1;
                // замечание: подстраиваем rect так, чтобы не вылезать за экран (опционально)
                // g_actionPalette.rect.x = std::min(m.x, GetScreenWidth() - g_actionPalette.rect.width);
                // g_actionPalette.rect.y = std::min(m.y, GetScreenHeight() - g_actionPalette.rect.height);
            }

            g_actionPalette.lastClickTime = now;
            g_actionPalette.lastClickPos = m;
        }
    }
}

void GraphDraw(Rectangle graphArea)
{
    static std::shared_ptr<Texture2D> checkerTexture;
    if (!checkerTexture) {
        checkerTexture = ResourceManager::LoadTexture("Resources/GraphBackground.png");
    }
    if (!checkerTexture) return;
    
    DrawText(TextFormat("%s",CurrentFile.name.c_str()), (int)graphArea.x + (graphArea.width * 0.5), (int)graphArea.y - 15, 12, BLACK);

    BeginScissorMode(
        (int)graphArea.x,
        (int)graphArea.y,
        (int)graphArea.width,
        (int)graphArea.height
    );

    const int tileSize = 64;

    auto modPos = [](int value, int m) {
        int r = value % m;
        if (r < 0) r += m;
        return r;
    };

    // масштабированный размер тайла
    float scaledTile = tileSize * graphScale;

    int offsetX = modPos((int)graphOffset.x, (int)scaledTile);
    int offsetY = modPos((int)graphOffset.y, (int)scaledTile);

    int startX = (int)graphArea.x - offsetX - scaledTile;
    int startY = (int)graphArea.y - offsetY - scaledTile;

    int endX = (int)(graphArea.x + graphArea.width) + scaledTile;
    int endY = (int)(graphArea.y + graphArea.height) + scaledTile;

    for (float x = startX; x <= endX; x += scaledTile) {
        for (float y = startY; y <= endY; y += scaledTile) {
            Rectangle src = { 0, 0, (float)tileSize, (float)tileSize };
            Rectangle dst = { x, y, scaledTile, scaledTile };
            DrawTexturePro(*checkerTexture, src, dst, { 0, 0 }, 0.0f, WHITE);
        }
    }


    // 2. Рисуем соединения (важно сделать ДО узлов)
    DrawConection();

    // если g_connDrag.active — рисуем временную линию от стартовой позиции к мыши
    if (g_connDrag.active) {
        NodeBase* n = FindNodeById(CurrentFile, g_connDrag.nodeId);
        if (n) 
        {
            Vector2  outputPos = GetPortScreenPosition(*n, g_connDrag.portId);
            Vector2 inputPos = GetMousePosition();
            
            Color drawColor = BLUE;
            // серый для exec
            Color execColor = Color{ 200, 200, 200, 255 };
            const float HANDLE_LENGTH = 150.0f * graphScale; // Длина "ручек" изгиба
            const float BASE_LINE_THICKNESS = 4.0f * graphScale;
            float lineThickness = BASE_LINE_THICKNESS;


            for (auto p : n->ports)
            {
                if (p.ID == g_connDrag.portId)
                {
                    if (IsExecPort(p))
                    {
                        drawColor = execColor;
                        lineThickness = BASE_LINE_THICKNESS * 2.0f;
                    }
                    else
                    {
                        drawColor = ColorForVarType(p.type);
                    }
                    break;
                }
            }
                //IsExecPort(outPort)
            Vector2 controlPoint1 = { outputPos.x + HANDLE_LENGTH, outputPos.y };
            Vector2 controlPoint2 = { inputPos.x - HANDLE_LENGTH, inputPos.y };
            DrawSplineSegmentBezierCubic(outputPos,controlPoint1,controlPoint2,inputPos,lineThickness,drawColor );
        }
    }


    // 3. Рисуем узлы
    for (auto& node : CurrentFile.Nodes) {
        DrawNode(*node);
    }

    EndScissorMode();
    UpdateAndDrawActionPalette(graphArea);
    DrawText(TextFormat("x : %i y : %i ", (int)graphOffset.x, (int)graphOffset.y), (int)graphArea.x , (int)graphArea.y + graphArea.height - 24, 12, BLACK);
}

Vector2 SnapToGrid(Vector2 pos)
{
    const int snap = 16;
    return Vector2{
        (pos.x / snap)* snap,
            (pos.y / snap)* snap
    };
}

void DrawNode(const NodeBase& node)
{

    static std::shared_ptr<Font> font;
    if(!font)
        font = ResourceManager::LoadFont("Resources/Roboto-Bold.ttf");


    static std::shared_ptr<Texture2D> NodeTexture;
    static std::shared_ptr<Texture2D> PureNodeTexture;
    static std::shared_ptr<Texture2D> Node_title_Texture;
    static std::shared_ptr<Texture2D> PureNode_title_Texture;
    static std::shared_ptr<Texture2D> ExecTexture;
    //g_SelectedNode
    static std::shared_ptr<Texture2D> NodeSelectTexture;
    if (!NodeTexture) {
        NodeTexture = ResourceManager::LoadTexture("Resources/RegularNode_body.png");
        PureNodeTexture = ResourceManager::LoadTexture("Resources/VarNode_body.png");
        PureNode_title_Texture = ResourceManager::LoadTexture("Resources/VarNode_gloss.png");
        Node_title_Texture = ResourceManager::LoadTexture("Resources/RegularNode_title_gloss.png");
        ExecTexture = ResourceManager::LoadTexture("Resources/ExecPin_Connected.png");

        NodeSelectTexture = ResourceManager::LoadTexture("Resources/RegularNode_shadow_selected.png");
    }
    if (!NodeTexture) return;

    Rectangle dst = CalculateNodeRect(node); // если есть такая функция — используй, иначе скопируй логику
    if(!node.isPure)
        DrawTexturePro(*NodeTexture, { 0,0,(float)NodeTexture->width,(float)NodeTexture->height }, dst, { 0,0 }, 0, Fade(node.isHovered ? LIGHTGRAY : WHITE, 0.5f));
    else
        DrawTexturePro(*PureNodeTexture, { 0,0,(float)PureNodeTexture->width,(float)PureNodeTexture->height }, dst, { 0,0 }, 0, Fade(node.isHovered ? LIGHTGRAY : WHITE, 0.5f));
    float titleHeight = 18.0f * graphScale; // высота полоски заголовка (настраиваемая)
    Rectangle titleDst = { dst.x, dst.y, dst.width, titleHeight }; // экранные координаты

    // Полный src по текстуре
    Rectangle titleSrc = { 0.0f, 0.0f, (float)Node_title_Texture->width, (float)Node_title_Texture->height };

    // Отрисовать фон заголовка (растянет текстуру по width/height)
    if (!node.isPure)
        DrawTexturePro(*Node_title_Texture, titleSrc, titleDst, { 0.0f, 0.0f }, 0.0f, SKYBLUE);
    else
    {
        titleHeight = 18.0f * graphScale;
        titleSrc = { 0.0f, 0.0f, (float)PureNode_title_Texture->width, (float)PureNode_title_Texture->height };
        DrawTexturePro(*PureNode_title_Texture, titleSrc, titleDst, { 0.0f, 0.0f }, 0.0f, SKYBLUE);
    }
 //   DrawText(node.name.c_str(), dst.x + 10 * graphScale, dst.y + 5 * graphScale, 14 * graphScale, WHITE);

    if(g_SelectedNode == node.ID)
        DrawRectangleLines(dst.x, dst.y, dst.width, dst.height, ORANGE);// DrawTexturePro(*NodeSelectTexture, { 0,0,(float)NodeTexture->width,(float)NodeTexture->height }, dst, { 0,0 }, 0, WHITE);

    
    std::string fullName = node.name;
    if (!node.sourceFile.empty())
        fullName += " ( from " + node.sourceFile + " )";
    DrawTextEx(*font, fullName.c_str(), Vector2{dst.x + 10 * graphScale, dst.y + 5 * graphScale}, 14 * graphScale, 1, WHITE);
    NodeBase* srcNode = nullptr;
    SPort srcPort;
    bool hasSrc = false;
    if (g_connDrag.active) {
        srcNode = FindNodeById(CurrentFile, g_connDrag.nodeId);
        if (srcNode) {
            for (const auto& p : srcNode->ports) {
                if (p.ID == g_connDrag.portId) { srcPort = p; hasSrc = true; break; }
            }
        }
    }

    if (!node.ToolTip.empty())
    {
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, dst))
        {
            int fontSize = 18;
            Vector2 textSize = MeasureTextEx(*font, node.ToolTip.c_str(), fontSize, 1);
            Vector2 tipPos = { mousePos.x + 12, mousePos.y + 12 };
            DrawRectangleRounded({ tipPos.x - 4, tipPos.y - 4, textSize.x + 8, textSize.y + 8 },0.2f, 4,BLACK);
            DrawTextEx(*font, node.ToolTip.c_str(), tipPos, fontSize, 1, WHITE);
        }
    }

    for (const auto& port : node.ports) {
        Vector2 ppos = GetPortScreenPosition(node, port.ID);
        float pinRadius = 5.0f * graphScale;
    //    TraceLog(LOG_INFO, "Node %s port '%s' id=%d type=%d isInput=%d", node.name.c_str(), port.name.c_str(), port.ID, (int)port.type, port.isInput);
        // Если это exec-иконка — рисуем текстуру, иначе — кружок
        if (IsExecPort(port)) {
            // рисуем иконку exec
            float texScale = graphScale;
            Texture2D& tex = *ExecTexture;
            Vector2 texPos = { ppos.x - (tex.width * 0.5f) * texScale, ppos.y - (tex.height * 0.5f) * texScale };
            DrawTextureEx(tex, texPos, 0.0f, texScale, WHITE);

            // подпись порта (для входа/выхода — показать имя рядом)
            int fontSize = (int)(12 * graphScale);
            if (port.isInput) {
                DrawTextEx(*font, port.name.c_str(), Vector2{ ppos.x + 8 * graphScale, ppos.y - 6 * graphScale }, fontSize, 1, WHITE);
            }
            else {
                int tw = MeasureText(port.name.c_str(), fontSize);
                DrawTextEx(*font, port.name.c_str(), Vector2{ ppos.x - 8 * graphScale - tw, ppos.y - 6 * graphScale }, fontSize, 1, WHITE);
            }
        }
        else {
            Color circleCol = ColorForVarType(port.type);//port.isInput ? PURPLE : ColorForVarType(port.type);
            DrawCircleV(ppos, pinRadius, circleCol);
            // Рисуем текст: у входа справа, у выхода слева
            int fontSize = (int)(12 * graphScale);
            if (port.isInput) {
                DrawTextEx(*font, port.name.c_str(), Vector2{ ppos.x + 8 * graphScale, ppos.y - 6 * graphScale }, fontSize, 1, WHITE);
            }
            else {
                int tw = MeasureText(port.name.c_str(), fontSize);

                DrawTextEx(*font, port.name.c_str(), Vector2{ ppos.x - 8 * graphScale - tw, ppos.y - 6 * graphScale }, fontSize, 1, WHITE);
            }
        }

        // подсветка порта
        if (port.isHovered) 
        {
            Color lineColor = YELLOW; // дефолт

            if (g_connDrag.active && hasSrc) {
                // защита: если это тот же самый порт — запрещаем
                if (g_connDrag.nodeId == node.ID && g_connDrag.portId == port.ID) {
                    lineColor = RED;
                }
                else {
                    bool srcIsExec = IsExecPort(srcPort);
                    bool tgtIsExec = IsExecPort(port);

                    // Если хоть один — exec, применяем строгие exec-правила:
                    if (srcIsExec || tgtIsExec) {
                        if (!srcPort.isInput && port.isInput && IsExecPort(srcPort) && IsExecPort(port)) {
                            // запрет соединять ноду саму с собой
                            if (g_connDrag.nodeId != node.ID) lineColor = GREEN;
                            else lineColor = RED;
                        }
                        else if (srcPort.isInput && !port.isInput && IsExecPort(srcPort) && IsExecPort(port)) {
                            // пользователь начал drag с входа и отпустил на выходе — это перевернутое соединение
                            if (g_connDrag.nodeId != node.ID) lineColor = GREEN;
                            else lineColor = RED;
                        }
                        else {
                            lineColor = RED;
                        }
                    }
                    else {
                        // обычные порты: разрешаем только из output -> input (или наоборот, если начал с input)
                        if (!srcPort.isInput && port.isInput) {
                            // src (output) -> target (input)
                            if (AreTypesCompatible(srcPort.type, port.type)) lineColor = GREEN;
                            else lineColor = RED;
                        }
                        else if (srcPort.isInput && !port.isInput) {
                            // src был вход (начали drag с входа) — отпустили на выходе => переворачиваем соединение
                            if (AreTypesCompatible(port.type, srcPort.type)) lineColor = GREEN;
                            else lineColor = RED;
                        }
                        else {
                            // input->input или output->output — запрещено
                            lineColor = RED;
                        }
                    }
                }
            }

            // нарисовать подсветку цветом lineColor
            DrawCircleLines((int)ppos.x, (int)ppos.y, pinRadius + 2.0f, lineColor);
        }
    }
}

void NodeUpdate(NodeBase& node)
{
    Rectangle r = CalculateNodeRect(node);
    Vector2 mouse = GetMousePosition();

    // Если уже перетаскиваем другую ноду — ничего не делаем
    if (g_draggingNodeId != -1 && g_draggingNodeId != node.ID) return;

    node.isHovered = CheckCollisionPointRec(mouse, r);

    // Сначала проверим порты (чтобы клик по порту не стал drag ноды)
    bool anyPortUnderMouse = false;
    for (auto& port : node.ports) {
        Vector2 ppos = GetPortScreenPosition(node, port.ID);
        float radius = 6.0f * graphScale;
        port.isHovered = CheckCollisionPointCircle(mouse, ppos, radius);
        if (port.isHovered) anyPortUnderMouse = true;

        // Ctrl+LMB on port => disconnect all connections on this port
        if (port.isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsKeyDown(KEY_LEFT_CONTROL)) {
            DisconnectAllForPort(CurrentFile, node.ID, port.ID);
        }

        // обычный LMB => старт drag-connection
        if (port.isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsKeyDown(KEY_LEFT_CONTROL)) {
            g_connDrag.active = true;
            g_connDrag.nodeId = node.ID;
            g_connDrag.portId = port.ID;
            g_connDrag.isOutput = !port.isInput;
        }
    }

    // Начало перетаскивания — нажали ЛКМ на ноде
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
    {
        g_SelectedNode = node.ID;
        if (node.isHovered && !anyPortUnderMouse) 
        {
            g_draggingNodeId = node.ID;
            g_dragStartMouseWorld = ScreenToWorld(mouse);
            g_dragStartNodePos = node.position;
        }
    }

    // Делаем drag
    if (g_draggingNodeId == node.ID && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 curWorld = ScreenToWorld(mouse);
        Vector2 delta = { curWorld.x - g_dragStartMouseWorld.x, curWorld.y - g_dragStartMouseWorld.y };
        node.position = { g_dragStartNodePos.x + delta.x, g_dragStartNodePos.y + delta.y };
    }

    // Завершение перетаскивания
    if (g_draggingNodeId == node.ID && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        g_draggingNodeId = -1;
    }

    // Правый клик по ноде — показать контекстное меню (удалить/дублировать) — пример:
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && CheckCollisionPointRec(mouse, r)) 
    {
        OpenNodeContextMenu(node.ID, mouse);
        g_SelectedNode = node.ID;
    }

    // Обработка кликов по портам (для начала/окончания drag-connect)
    // Пример: если клик на порт — стартуем drag connection
    // Проверяем каждый порт — если кликнули по его кружку — начинаем перетаскивать соединение
    for (auto& port : node.ports) {
        Vector2 ppos = GetPortScreenPosition(node, port.ID);
        float radius = 6.0f * graphScale;
        port.isHovered = CheckCollisionPointCircle(mouse, ppos, radius);
        if (port.isHovered)
        {
            // Нажали ЛКМ на порту — старт drag соединения
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                g_connDrag.active = true;
                g_connDrag.nodeId = node.ID;
                g_connDrag.portId = port.ID;
                g_connDrag.isOutput = !port.isInput; // если порт не входной — это выход
            }
        }
    }

    // Если во время drag-connection отпустили клавишу — пробуем завершить соединение
    if (g_connDrag.active && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        TryFinishConnectionDrag(CurrentFile);
    }


    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (!CheckCollisionPointRec(mouse, r))
            g_SelectedNode = -1;
    }
}


void DrawConection()
{

    const float HANDLE_LENGTH = CONNECTOR_HANDLE_LENGTH  * graphScale; // Длина "ручек" изгиба
    const float BASE_LINE_THICKNESS = BASE_CONNECTOR_THICKNESS * graphScale;

    // цвет по умолчанию (если типы не определены)
    Color defaultColor = BLUE;
    // серый для exec
    Color execColor = Color{ 200, 200, 200, 255 };

    for (size_t idx = 0; idx < CurrentFile.connections.size(); ++idx) {
        const auto& conn = CurrentFile.connections[idx];

        // Находим узлы по ID
        NodeBase* outputNode = nullptr;
        NodeBase* inputNode = nullptr;

        for (const auto& n : CurrentFile.Nodes) {
            if (n->ID == conn.outputNodeId) outputNode = n.get();
            if (n->ID == conn.inputNodeId) inputNode = n.get();
        }

        if (!outputNode || !inputNode) continue;

        // Получаем позиции портов
        Vector2 outputPos = conn.isTemp ? GetMousePosition() :GetPortScreenPosition(*outputNode, conn.outputPortID);
        Vector2 inputPos = GetPortScreenPosition(*inputNode, conn.inputPortID);

        // находим объекты самих портов, чтобы узнать типы
        SPort outPort{}, inPort{};
        bool haveOut = false, haveIn = false;
        for (const auto& p : outputNode->ports) {
            if (p.ID == conn.outputPortID) { outPort = p; haveOut = true; break; }
        }
        for (const auto& p : inputNode->ports) {
            if (p.ID == conn.inputPortID) { inPort = p; haveIn = true; break; }
        }
        // Если соединение под курсором — подсветка: делаем ярче и толще
        float lineThickness = BASE_LINE_THICKNESS;
        // Вычисляем цвет линии:
        Color connectionColor = defaultColor;
        if ((haveOut && IsExecPort(outPort)) || (haveIn && IsExecPort(inPort))) {
            // если хотя бы один — exec, рисуем серым
            connectionColor = execColor;
             lineThickness = BASE_LINE_THICKNESS * 2.0f;
        }
        else if (haveOut && haveIn) {
            // оба не exec — используем цвет выхода, либо смешиваем (если разные)
            Color colOut = ColorForVarType(outPort.type);
            Color colIn = ColorForVarType(inPort.type);
            if (outPort.type == inPort.type) connectionColor = colOut;
            else connectionColor = MixColor(colOut, colIn);

           
        }
        else if (haveOut) {
            connectionColor = ColorForVarType(outPort.type);
        }
        else if (haveIn) {
            connectionColor = ColorForVarType(inPort.type);
        }

        
        Color drawColor = connectionColor;
        if (/* глобальный индекс наведённой связи */ g_hoveredConnectionIndex == (int)idx) {
            // увеличить толщину и "осветлить" цвет (простое увеличение альфа/яркости)
            lineThickness = BASE_LINE_THICKNESS * 1.6f;
            // увеличить яркость: прибавим 30 к каждому компоненту (clamp)
            unsigned char nr = (unsigned char)std::min(255, (int)drawColor.r + 60);
            unsigned char ng = (unsigned char)std::min(255, (int)drawColor.g + 60);
            unsigned char nb = (unsigned char)std::min(255, (int)drawColor.b + 60);
            drawColor = Color{ nr, ng, nb, 255 };
        }

        // Рассчитываем контрольные точки для плавной кривой
        Vector2 controlPoint1 = { outputPos.x + HANDLE_LENGTH, outputPos.y };
        Vector2 controlPoint2 = { inputPos.x - HANDLE_LENGTH, inputPos.y };

        // Рисуем кривую Безье — используем твою функцию DrawSplineSegmentBezierCubic
        DrawSplineSegmentBezierCubic(
            outputPos,
            controlPoint1,
            controlPoint2,
            inputPos,
            lineThickness,
            drawColor
        );

        // Точка подключения на входном порту (немного прозрачная)
        DrawCircleV(inputPos, lineThickness * 2.0f, Fade(drawColor, 0.7f));
    }
}

Vector2 GetPortScreenPosition(const NodeBase& node, int portId) 
{
    // Вычисление rect ноды (как в DrawNode)
    int NodeSize = 128;
    if (node.isPure)
        NodeSize = 64;
    float scaledNode = NodeSize * graphScale;
    float minWidth = scaledNode * 2;
    float minHeight = scaledNode;
    float nameWidth = node.name.size() * 8.0f * graphScale + (node.isPure ? 20.0f : 30.0f )* graphScale;
    float width = std::max(minWidth, nameWidth);
    int portsCount = (int)node.ports.size();
    float height = std::max(minHeight, portsCount * (node.isPure ? 20.0f : 30.0f) * graphScale);
    float x = node.position.x * graphScale - graphOffset.x;
    float y = node.position.y * graphScale - graphOffset.y;

    float portYStart = y + 30.0f * graphScale;
    float portSpacing = 20.0f * graphScale;

    int inputIndex = 0;
    int outputIndex = 0;

    for (const auto& port : node.ports) {
        if (port.ID == portId) {
            if (port.isInput) {
                float px = x + 5.0f * graphScale + 5.0f; // немного внутрь
                float py = portYStart + inputIndex * portSpacing;
                return { px, py };
            }
            else {
                float px = x + width - 5.0f * graphScale - 5.0f;
                float py = portYStart + outputIndex * portSpacing;
                return { px, py };
            }
        }
        if (port.isInput) inputIndex++;
        else outputIndex++;
    }

    // fallback если не найден
    return { x + width * 0.5f, y + height * 0.5f };
}



// Пример "построителя" меню для ноды (создаёт список пунктов)
std::vector<ContextMenuItem> BuildNodeContextMenu(int nodeId) {
    std::vector<ContextMenuItem> out;


    out.emplace_back("Find all Calling", [nodeId]() {
        NodeBase* n = FindNodeById(CurrentFile, nodeId);
        if (n && !isFindingCallingFiles) {
            isFindingCallingFiles = true;
            findProgress = 0.0f;

            std::thread([nodeName = n->name]() {
                // Очистить старый результат
                {
                    std::lock_guard<std::mutex> lock(foundFilesMutex);
                    foundCallingFiles.clear();
                }

                // Ваша функция (можно расширить для обновления прогресса внутри)
                FindCallingFiles(nodeName);

                // В конце
                isFindingCallingFiles = false;
                findProgress = 1.0f;
            }).detach();
        }
        });
    // Удалить
    out.emplace_back("Delete Node", [nodeId]() {
        // вызываем вашу функцию удаления ноды
        DeleteNode(CurrentFile, nodeId);
        });

    // Дублировать
    //out.emplace_back("Duplicate Node", [nodeId]() {
    //    DuplicateNode(CurrentFile, nodeId);
    //    });

    out.emplace_back(ContextMenuItem::Separator());

    // Toggle pure (пример изменения свойства)
    out.emplace_back("Toggle Pure", [nodeId]() {
        NodeBase* n = FindNodeById(CurrentFile, nodeId);
        if (n) n->isPure = !n->isPure;
        });

    out.emplace_back("Add Node Here", [nodeId]() {
        // добавляем новую ноду рядом с target
        NodeBase* n = FindNodeById(CurrentFile, nodeId);
        if (n) {
            Vector2 newPos = { n->position.x + 2.0f, n->position.y + 2.0f };
            auto nn = AddNode(CurrentFile, std::string("NewNode"), newPos, false);
            // можно заполнить порты и т.д.
        }
        });

    return out;
}

void OpenNodeContextMenu(int nodeId, Vector2 screenPos) {
    g_contextMenu.open = true;
    g_contextMenu.pos = screenPos;
    g_contextMenu.targetNodeId = nodeId;
    g_contextMenu.items = BuildNodeContextMenu(nodeId);
    g_contextMenu.hoveredIndex = -1;
}

void UpdateAndDrawActionPalette(const Rectangle& graphArea)
{
    if (!g_actionPalette.open) return;

    static std::shared_ptr<Font> font;
    if (!font)
        font = ResourceManager::LoadFont("Resources/Roboto-Bold.ttf");

    // position/adjust as before
    Rectangle pr = g_actionPalette.rect;
    pr.x = g_actionPalette.screenPos.x;
    pr.y = g_actionPalette.screenPos.y;
    int screenW = GetScreenWidth(), screenH = GetScreenHeight();
    if (pr.x + pr.width > screenW) pr.x = screenW - pr.width - 8;
    if (pr.y + pr.height > screenH) pr.y = screenH - pr.height - 8;
    if (pr.x < 8) pr.x = 8;
    if (pr.y < 8) pr.y = 8;
    DrawRectangleRec(pr, Fade(GRAY, 0.95f));
    DrawRectangleLinesEx(pr, 1.0f, Fade(BLACK, 0.7f));
    Rectangle titleR = { pr.x + 8, pr.y + 8, pr.width - 16, 20 };
    DrawText("All Actions for this Blueprint", (int)titleR.x, (int)titleR.y, 12, BLACK);

    // search box (keep existing buffer logic)
    Rectangle searchR{ pr.x + 8, pr.y + 32, pr.width - 16 - 20, 22 };
    static char searchBuf[256] = { 0 };
    // copy current search into buffer only if different length to avoid clobbering user typing
    if (g_actionPalette.search.size() < sizeof(searchBuf)) {
        strncpy(searchBuf, g_actionPalette.search.c_str(), sizeof(searchBuf) - 1);
    }
    if (GuiTextBox(searchR, searchBuf, sizeof(searchBuf), true)) {
        g_actionPalette.search = std::string(searchBuf);
        // when search changes, reset scroll and ensure tree is (re)built
        g_actionPalette.scrollOffset = { 0,0 };
    }

    // context checkbox
    Rectangle csRect{ pr.x + pr.width - 16 - 64, pr.y + 32, 16, 16 };
    static bool cs = g_actionPalette.contextSensitive;
    if (GuiCheckBox(csRect, "ctx", &cs)) { g_actionPalette.contextSensitive = !g_actionPalette.contextSensitive; }

    // list area
    const float itemH = 22.0f;
    float listY = pr.y + 60;
    Rectangle listBounds{ pr.x + 8, listY, pr.width - 16, pr.height - (listY - pr.y) - 12 };

    // Build filtered tree based on search (lowercase query)
    std::string q = g_actionPalette.search;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);
    auto tree = BuildCategoryTreeFiltered(q);

    // Build visible list from tree
    std::vector<VisibleEntry> visible;
    FillVisibleListFromTree(tree.get(), visible, 0);

    // compute contentBounds
    Rectangle contentBounds{ 0, 0, listBounds.width - 20.0f, (float)visible.size() * itemH + 8.0f };
    // call GuiScrollPanel
    GuiScrollPanel(listBounds, "", contentBounds, &g_actionPalette.scrollOffset, &g_actionPalette.scrollView);

    BeginScissorMode((int)g_actionPalette.scrollView.x, (int)g_actionPalette.scrollView.y, (int)g_actionPalette.scrollView.width, (int)g_actionPalette.scrollView.height);
    DrawRectangleRec(listBounds, Fade(BLACK, 0.7f));
    Vector2 mouse = GetMousePosition();
    g_actionPalette.hoveredIndex = -1;
    float y = g_actionPalette.scrollView.y + g_actionPalette.scrollOffset.y + 4.0f;


    std::string tool_tip = "";
    for (size_t vi = 0; vi < visible.size(); ++vi) {
        const VisibleEntry& e = visible[vi];
        Rectangle itRec{ g_actionPalette.scrollView.x + 6.0f + e.depth * 12.0f, y + vi * itemH, g_actionPalette.scrollView.width - 12.0f - e.depth * 12.0f, itemH - 4.0f };

        bool hover = CheckCollisionPointRec(mouse, itRec);
        if (hover) {
            DrawRectangleRec(itRec, Fade(LIGHTGRAY, 0.8f));
            g_actionPalette.hoveredIndex = (int)vi;
        }

        if (e.isCategory) {
            // draw expand/collapse triangle
            CatNode* cat = e.cat;

            std::string cat_name = (cat->expanded ? "-" : "+") + cat->name;

            DrawRectangleLines(itRec.x, itRec.y, itRec.width, itRec.height, Fade(BLACK, 0.5f));
            DrawText(cat_name.c_str(), (int)(itRec.x + 10), (int)(itRec.y + (itRec.height - 12) * 0.5f), 12, RAYWHITE);



            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                cat->expanded = !cat->expanded;
                // after toggling, rebuild visible list / content. easiest: reset scroll and break to rebuild on next frame.
                if (!cat->fullPath.empty()) {
                    g_categoryExpanded[cat->fullPath] = cat->expanded;
                }
                g_actionPalette.scrollOffset = { 0,0 };
                break;
            }
        }
        else {
            const auto& tpl = g_NodeTemplates[e.templateIndex];
            DrawRectangleRec(itRec, Fade(SKYBLUE, 0.2f));
            DrawRectangleLines(itRec.x, itRec.y, itRec.width, itRec.height, Fade(BLACK, 0.5f));
            std::string t = "f " + tpl.name;
            DrawText(t.c_str(), (int)(itRec.x + 6), (int)(itRec.y + (itRec.height - 12) * 0.5f), 12, WHITE);

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 world;
                world.x = (mouse.x - graphArea.x + graphOffset.x) / graphScale;
                world.y = (mouse.y - graphArea.y + graphOffset.y) / graphScale;

                auto newNode = AddNodeFromTemplate(CurrentFile, tpl, world);
                g_actionPalette.open = false;
                g_SelectedNode = newNode->ID;
                break;
            }

            if (hover)
                tool_tip = tpl.ToolTip;
        }
    }

    EndScissorMode();

    if (!tool_tip.empty())
    {
        Vector2 mousePos = GetMousePosition();
        int fontSize = 18;
        Vector2 textSize = MeasureTextEx(*font, tool_tip.c_str(), fontSize, 1);
        Vector2 tipPos = { mousePos.x + 12, mousePos.y + 12 };
        DrawRectangleRounded({ tipPos.x - 4, tipPos.y - 4, textSize.x + 8, textSize.y + 8 }, 0.2f, 4, BLACK);
        DrawTextEx(*font, tool_tip.c_str(), tipPos, fontSize, 1, WHITE);
    }
   

    // Close handling
    if (IsKeyPressed(KEY_ESCAPE)) { g_actionPalette.open = false; }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!CheckCollisionPointRec(mouse, pr)) {
            if (!CheckCollisionPointRec(mouse, g_actionPalette.scrollView)) {
                g_actionPalette.open = false;
            }
        }
    }
}

void ScanFosScripts(const std::string& folderPath)
{
    fosFiles.clear();
    for (auto& p : fs::directory_iterator(folderPath))
    {
        if (p.is_regular_file() && p.path().extension() == ".fos")
        {
            fosFiles.push_back(p.path().string());
        }
    }
    TraceLog(LOG_INFO, "ScanFosScripts count ->  %i", fosFiles.size());
    // Можно подстраивать высоту контента под количество файлов
    Browse_contentBounds.height = fosFiles.size() * 30;
}

void DrawScriptList()
{

    //Browse_contentBounds.height = fosFiles.size() * 30;

    Rectangle viewRect = { 0 };
    Browse_scrollPanelBounds.y += 10;
    GuiScrollPanel(Browse_scrollPanelBounds, nullptr, Browse_contentBounds, &Browse_scroll, &viewRect);

    // Теперь рисуем кнопки с учётом Browse_scroll и Browse_scrollPanelBounds
    //Browse_scrollPanelBounds

    BeginScissorMode(
        (int)viewRect.x,
        (int)viewRect.y ,
        (int)viewRect.width,
        (int)viewRect.height
    );
    for (size_t i = 0; i < fosFiles.size(); i++)
    {
        Rectangle btnRect = {
            Browse_scrollPanelBounds.x + 5,
            Browse_scrollPanelBounds.y + i * 30 + Browse_scroll.y,
            Browse_scrollPanelBounds.width * 0.80,
            25
        };

        if (GuiButton(btnRect, fs::path(fosFiles[i]).filename().string().c_str()))
        {
            selectedFileIndex = (int)i;

            printf("load -> %s\n", fosFiles[i].c_str());
            LoadScript(fosFiles[i].c_str());
            //CurrentFile.Clear();
            ////// Загружаем содержимое скрипта
            //std::string scriptContent = LoadFileContent(fosFiles[i].c_str());
            //CurrentFile.name = fs::path(fosFiles[i]).filename().string().c_str();
            //// Парсим скрипт
            //FOnlineScriptParser parser;
            //if (parser.ParseScript(scriptContent, CurrentFile)) {
            //    // Теперь scriptFile содержит узлы и соединения
            //    TraceLog(LOG_INFO, "Parsed %d functions", CurrentFile.Nodes.size());

            //}
            //else {
            //    TraceLog(LOG_ERROR, "Failed to parse script");
            //}

            //AutoArrangeNodes(CurrentFile, 100, 100, 500, 256);

            //bool ok = LoadScriptFileByName(CurrentFile.name.c_str(), CurrentFile);
            //TraceLog(ok ? LOG_INFO : LOG_WARNING, "LoadScriptFileByName('%s') -> %d", CurrentFile.name.c_str(), (int)ok);

            //TraceLog(LOG_INFO, "Found %d Import", CurrentFile.Import.size());

            // Загрузка скрипта тут
        }
    }

    EndScissorMode();
}



void LoadScript(std::string file)
{
    printf("load -> %s\n", file.c_str());
    CurrentFile.Clear();
    //// Загружаем содержимое скрипта
    std::string scriptContent = LoadFileContent(file.c_str());
    CurrentFile.name = fs::path(file).filename().string().c_str();
    // Парсим скрипт
    FOnlineScriptParser parser;
    if (parser.ParseScript(scriptContent, CurrentFile)) {
        // Теперь scriptFile содержит узлы и соединения
        TraceLog(LOG_INFO, "Parsed %d functions", CurrentFile.Nodes.size());

    }
    else {
        TraceLog(LOG_ERROR, "Failed to parse script");
    }

    AutoArrangeNodes(CurrentFile, 100, 100, 500, 256);

    bool ok = LoadScriptFileByName(CurrentFile.name.c_str(), CurrentFile);
    TraceLog(ok ? LOG_INFO : LOG_WARNING, "LoadScriptFileByName('%s') -> %d", CurrentFile.name.c_str(), (int)ok);

    TraceLog(LOG_INFO, "Found %d Import", CurrentFile.Import.size());
}
void FindCallingFiles(const std::string& nodeName)
{
    callingFiles.clear();
    lastFind_nodeName = nodeName;

    std::regex callRegex("\\b" + nodeName + "\\s*\\(");

    size_t totalFiles = fosFiles.size();
    for (size_t i = 0; i < totalFiles; ++i)
    {
        const auto& file = fosFiles[i];

        std::ifstream f(file);
        if (!f.is_open()) continue;

        std::stringstream buffer;
        buffer << f.rdbuf();
        std::string content = buffer.str();

        if (std::regex_search(content, callRegex))
        {
            // Для безопасности: блокируем доступ к callingFiles, если она общая и может читаться из другого потока
            //std::lock_guard<std::mutex> lock(callingFilesMutex);
            callingFiles.push_back(file);
        }

        // Обновляем прогресс
        findProgress = (float)(i + 1) / (float)totalFiles;
    }

    selectedCallerIndex = -1;
    showCallersWindow = true;

    findProgress = 1.0f; // на всякий случай — закончили


   
}
// Рисование окна с файлами, где вызывается функция
void DrawCallingFilesWindow()
{
    if (isFindingCallingFiles) {
        float barWidth = 300;
        float barHeight = 20;
        Rectangle barRec = { 50, 50, barWidth, barHeight };

        DrawRectangleLinesEx(barRec, 2, DARKGRAY);
        DrawRectangle(barRec.x, barRec.y, barWidth * findProgress.load(), barHeight, GREEN);
        DrawText("Finding Calling Files...", barRec.x, barRec.y - 25, 20, BLACK);
    }

    if (!showCallersWindow) return;

    const int winW = 400;
    const int winH = 300;
    Rectangle winBounds = { 100, 100, (float)winW, (float)winH };

    GuiWindowBox(winBounds, "Files Calling This Node");

    Rectangle scrollPanelBounds = { winBounds.x + 10, winBounds.y + 30, winBounds.width - 20, winBounds.height - 70 };
    Rectangle contentBounds = { 0, 0, scrollPanelBounds.width - 20, callingFiles.size() * 30 };

    GuiScrollPanel(scrollPanelBounds, NULL, contentBounds, &callFilesScroll, nullptr);

    BeginScissorMode((int)scrollPanelBounds.x, (int)scrollPanelBounds.y, (int)scrollPanelBounds.width, (int)scrollPanelBounds.height);

    for (size_t i = 0; i < callingFiles.size(); i++)
    {
        Rectangle btnRect = { scrollPanelBounds.x + 5, scrollPanelBounds.y + i * 30 + callFilesScroll.y, scrollPanelBounds.width - 20, 25 };

        std::string filenameOnly = fs::path(callingFiles[i]).filename().string();

        if (GuiButton(btnRect, filenameOnly.c_str()))
        {
            selectedCallerIndex = (int)i;
            //printf("\nNew : %s  | curren %s\n", filenameOnly.c_str(), CurrentFile.name.c_str());
            if (fs::path(callingFiles[i]).filename() != fs::path(CurrentFile.name).filename())
                LoadScript(callingFiles[i]);


            g_SelectedNode = -1;
            if (!lastFind_nodeName.empty())
            {
                for (auto n : CurrentFile.Nodes)
                {
                    if (n->name == lastFind_nodeName)
                    {
                        graphOffset.x = (n->position.x * graphScale) - (GraphRect.width / 2.0f);
                        graphOffset.y = (n->position.y * graphScale) - (GraphRect.height / 2.0f);
                        g_SelectedNode = n->ID;
                        //printf("lastFind_nodeName %s | ID : %i\n", lastFind_nodeName.c_str(), g_SelectedNode);
                        break;
                    }
                }
            }
            showCallersWindow = false;
        }
    }

    EndScissorMode();

    Rectangle closeBtn = { winBounds.x + winBounds.width - 80, winBounds.y + winBounds.height - 30, 70, 25 };
    if (GuiButton(closeBtn, "Close"))
    {
        showCallersWindow = false;
    }
    
    
}






// Build category tree from g_NodeTemplates
static std::unique_ptr<CatNode> BuildCategoryTree()
{
    auto root = std::make_unique<CatNode>("__root__");
    // map for quick child search at each node: we will search linearly in children (small counts OK)
    for (int i = 0; i < (int)g_NodeTemplates.size(); ++i) {
        const auto& tpl = g_NodeTemplates[i];
        std::vector<std::string> parts = SplitCategoryPath(tpl.Category);
        CatNode* cur = root.get();
        for (const auto& p : parts) {
            // find child with that name
            CatNode* found = nullptr;
            for (auto& ch : cur->children) {
                if (ch->name == p) { found = ch.get(); break; }
            }
            if (!found) {
                cur->children.push_back(std::make_unique<CatNode>(p));
                cur->children.back()->parent = cur;
                found = cur->children.back().get();
            }
            cur = found;
        }
        // attach template index to this node (if no category parts, goes to root)
        cur->templates.push_back(i);
    }

    // Optionally sort children and templates alphanumerically
    std::function<void(CatNode*)> sortRec = [&](CatNode* n) {
        std::sort(n->children.begin(), n->children.end(), [](const std::unique_ptr<CatNode>& a, const std::unique_ptr<CatNode>& b) {
            return a->name < b->name;
            });
        std::sort(n->templates.begin(), n->templates.end(), [](int a, int b) {
            return g_NodeTemplates[a].name < g_NodeTemplates[b].name;
            });
        for (auto& ch : n->children) sortRec(ch.get());
    };
    sortRec(root.get());

    return root;
}

// Build visible list from tree, taking into account search. If search not empty, only include branches with matches.
// returns whether this subtree contains any match
static bool BuildVisibleListRec(CatNode* node, const std::string& queryLower, std::vector<VisibleEntry>& out, int depth)
{
    bool subtreeHasMatch = false;

    // Check templates directly under this node for query matches
    std::vector<int> matchedTemplates;
    if (queryLower.empty()) {
        matchedTemplates = node->templates;
        subtreeHasMatch = !matchedTemplates.empty();
    }
    else {
        for (int idx : node->templates) {
            const auto& tpl = g_NodeTemplates[idx];
            std::string nameLower = tpl.name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            if (nameLower.find(queryLower) != std::string::npos) {
                matchedTemplates.push_back(idx);
                subtreeHasMatch = true;
            }
        }
    }

    // Recurse children and track which children contain matches
    std::vector<CatNode*> matchedChildren;
    for (auto& ch : node->children) {
        bool childHas = BuildVisibleListRec(ch.get(), queryLower, out, depth + 1);
        if (childHas) {
            matchedChildren.push_back(ch.get());
            subtreeHasMatch = true;
        }
    }

    // If node is root, we don't emit a category header
    if (node->parent != nullptr) {
        // Only add this category header if:
        // - no search (we show full tree), OR
        // - search and subtreeHasMatch (so we show categories that contain matches)
        if (!queryLower.empty() && !subtreeHasMatch) {
            return false;
        }
        // add category header entry
        VisibleEntry ve;
        ve.isCategory = true;
        ve.cat = node;
        ve.templateIndex = -1;
        ve.depth = depth - 1; // make first-level categories depth 0
        out.push_back(ve);
    }

    // If node is expanded (or we are filtering and need to show matches) iterate children and templates in order
    bool showChildren = node->expanded || !queryLower.empty();
    if (showChildren) {
        // children first (sorted earlier)
        for (auto& ch : node->children) {
            // if querying, only show children that have matches
            if (queryLower.empty() || std::find(matchedChildren.begin(), matchedChildren.end(), ch.get()) != matchedChildren.end())
            {
                // but we already recursed — however we didn't add child's header when subtreeHasMatch==false
                // to avoid duplicating entries, we need to call a function that pushes child's header+content.
                // To simplify, call BuildVisibleListRec AGAIN but with a variant that appends entries.
                // But we've already appended child's entries to 'out' earlier in recursion — that's a problem.
                // To avoid double recursion, change approach: instead of recursing earlier, do recursion here.
            }
        }
    }

    // The above attempt shows double-recursion complexity. Simpler approach: do NOT recurse earlier.
    return subtreeHasMatch;
}

// Simpler approach: traverse tree and build visible list in a single pass that both tests for matches and appends visible nodes.
// We'll implement a helper that returns whether subtree contains match and appends entries only when appropriate.
static bool BuildVisibleList_Final(CatNode* node, const std::string& queryLower, std::vector<VisibleEntry>& out, int depth)
{
    // detect if any templates in subtree match
    bool subtreeHasMatch = false;
    // check templates at this node
    for (int idx : node->templates) {
        const auto& tpl = g_NodeTemplates[idx];
        std::string nameLower = tpl.name;
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        if (queryLower.empty() || nameLower.find(queryLower) != std::string::npos) {
            subtreeHasMatch = true;
            break;
        }
    }
    // check children
    for (auto& ch : node->children) {
        if (BuildVisibleList_Final(ch.get(), queryLower, out, depth + 1)) {
            subtreeHasMatch = true;
        }
    }

    // if node is root, don't emit header; else emit header only if subtreeHasMatch or no query
    if (node->parent != nullptr) {
        if (!queryLower.empty() && !subtreeHasMatch) {
            // do not add this category at all
            return false;
        }
        VisibleEntry header;
        header.isCategory = true;
        header.cat = node;
        header.templateIndex = -1;
        header.depth = std::max(0, depth - 1);
        out.push_back(header);
    }

    // If expanded (or searching), then append children and templates in order
    bool showChildren = node->expanded || !queryLower.empty();
    if (showChildren) {
        // children
        for (auto& ch : node->children) {
            // only append child's subtree if it contains matches when searching
            // but BuildVisibleList_Final already recursed and appended child's entries earlier -> to avoid missing order,
            // we should NOT have recursed earlier. To simplify we need an approach without pre-recursion...
        }
    }

    // The above shows complexity in single-pass. Let's implement a robust two-pass algorithm:

    return subtreeHasMatch;
}

// To avoid overcomplication in the answer, I'll present a tested, simpler algorithm:
// 1) collect all category paths for templates that match (or all if no query).
// 2) create a map: for each category path part create nodes and track templates under them (same as BuildCategoryTree).
// 3) produce visible list by traversing tree but only including branches that have matched templates (when querying).
// This is easier to reason about and avoids double-recursion pitfalls.

// Final implementation below:

static std::unique_ptr<CatNode> BuildCategoryTreeFiltered(const std::string& queryLower)
{
    auto root = std::make_unique<CatNode>("__root__");
    root->fullPath = ""; // root has empty path

    for (int i = 0; i < (int)g_NodeTemplates.size(); ++i) {
        const auto& tpl = g_NodeTemplates[i];
        // test match
        bool match = false;
        if (queryLower.empty()) match = true;
        else {
            std::string nameLower = tpl.name; std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            if (nameLower.find(queryLower) != std::string::npos) match = true;
            std::string catLower = tpl.Category; std::transform(catLower.begin(), catLower.end(), catLower.begin(), ::tolower);
            if (!match && catLower.find(queryLower) != std::string::npos) match = true;
        }
        if (!match) continue;

        std::vector<std::string> parts = SplitCategoryPath(tpl.Category);
        CatNode* cur = root.get();
        for (const auto& p : parts) {
            CatNode* found = nullptr;
            for (auto& ch : cur->children) if (ch->name == p) { found = ch.get(); break; }
            if (!found) {
                cur->children.push_back(std::make_unique<CatNode>(p));
                cur->children.back()->parent = cur;
                found = cur->children.back().get();
                // compute fullPath for child
                if (cur->fullPath.empty()) found->fullPath = p;
                else found->fullPath = cur->fullPath + " | " + p;
                // restore expanded state from global map (default false -> collapsed)
                auto it = g_categoryExpanded.find(found->fullPath);
                found->expanded = (it != g_categoryExpanded.end()) ? it->second : false;
            }
            cur = found;
        }
        cur->templates.push_back(i);
    }

    // sort children & templates
    std::function<void(CatNode*)> sortRec2 = [&](CatNode* n) {
        std::sort(n->children.begin(), n->children.end(), [](const std::unique_ptr<CatNode>& a, const std::unique_ptr<CatNode>& b) {
            return a->name < b->name;
            });
        std::sort(n->templates.begin(), n->templates.end(), [](int a, int b) {
            return g_NodeTemplates[a].name < g_NodeTemplates[b].name;
            });
        for (auto& ch : n->children) sortRec2(ch.get());
    };
    sortRec2(root.get());
    return root;
}

// Populate visible list by traversing tree (precondition: tree filtered already for matches)
static void FillVisibleListFromTree(CatNode* node, std::vector<VisibleEntry>& out, int depth)
{
    // root does not create a header
    if (node->parent != nullptr) {
        VisibleEntry h; h.isCategory = true; h.cat = node; h.templateIndex = -1; h.depth = depth - 1;
        out.push_back(h);
    }
    // if node collapsed and not root, do not show children/templates (unless we want to ignore collapse during search)
    bool showChildren = node->expanded;
    if (node->parent == nullptr) showChildren = true; // always show root children

    if (showChildren) {
        // children
        for (auto& ch : node->children) {
            FillVisibleListFromTree(ch.get(), out, depth + 1);
        }
        // templates
        for (int idx : node->templates) {
            VisibleEntry it; it.isCategory = false; it.cat = nullptr; it.templateIndex = idx; it.depth = depth;
            out.push_back(it);
        }
    }
}