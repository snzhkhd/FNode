#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "FileManager.h"

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
    int screenWidth = 1600;
    int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "FNode gui" );
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

        for (auto f : CurrentFile.Nodes)
        {
            TraceLog(LOG_INFO, "%s", f->name.c_str());
        }
       
        TraceLog(LOG_INFO, "Found %d connections", CurrentFile.connections.size());

        // Теперь можно рисовать узлы и соединения
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

   

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateLayout(layoutRecs, GetScreenWidth(), GetScreenHeight());
        GraphUpdate(layoutRecs[17]);

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
        layoutRecs[3].width - function_ScrollPanelBoundsOffset.x,
        layoutRecs[3].height - function_ScrollPanelBoundsOffset.y
        };

        // Область внутри панели (виртуальная "бумага")
        Rectangle contentBounds = { 0, 0, panelBounds.width - 20, (float)CurrentFile.Nodes.size() * 24 }; // 24px на элемент
        Rectangle viewArea = { 0 };
        GuiScrollPanel(panelBounds, "", contentBounds, &function_ScrollPanelScrollOffset, &viewArea);
        BeginScissorMode(
            (int)viewArea.x,
            (int)viewArea.y,
            (int)viewArea.width,
            (int)viewArea.height
        );

        // Рисуем список нод с учётом смещения скролла
        float y = panelBounds.y + function_ScrollPanelScrollOffset.y;
        for (size_t i = 0; i < CurrentFile.Nodes.size(); i++)
        {
            const auto& node = *CurrentFile.Nodes[i];
            Rectangle btn = { panelBounds.x + 5, y + i * 24, panelBounds.width - 30, 20 };

            if (CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Действие при нажатии
                printf("Clicked node: %s\n", node.name.c_str());

                graphOffset.x = (node.position.x * graphScale) - (layoutRecs[17].width / 2.0f);
                graphOffset.y = (node.position.y * graphScale) - (layoutRecs[17].height / 2.0f);
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
        GuiGroupBox(layoutRecs[10], tool_GroupBoxText);

      
        if (GuiButton(layoutRecs[11], save_btnText)) SaveBtnImplementation(CurrentFile);// SaveBtn();
        if (GuiButton(layoutRecs[12], compile_btnText)) CompileBtn();
        if (GuiButton(layoutRecs[13], setting_btnText)) SettingBtn();
        if (GuiButton(layoutRecs[14], gvars_btnText)) GvarsBtn();
        if (GuiButton(layoutRecs[15], defines_btnText)) DefinesBtn();
        if (GuiButton(layoutRecs[16], clear_fosb_btnText)) ClearFosbBtn();

        GraphDraw(layoutRecs[17]);
        GuiGroupBox(layoutRecs[17], graph_GroupBoxText);

        if(isShowConsole)
            GuiGroupBox(layoutRecs[18], Console_GroupBoxText);

        GuiCheckBox(layoutRecs[19], Console_CheckBoxText, &isShowConsole);
        //----------------------------------------------------------------------------------
        DrawContextMenu();
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
    // TODO: Implement control logic
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

    // Граф редактор занимает большую часть правой области
    recs[17] = Rectangle{
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

    if (!mouseInGraphArea)
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
    if (mouseInGraphArea) {
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
            graphOffset.x += (afterZoom.x - beforeZoom.x) * graphScale;
            graphOffset.y += (afterZoom.y - beforeZoom.y) * graphScale;
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
    static std::shared_ptr<Texture2D> Node_title_Texture;
    static std::shared_ptr<Texture2D> ExecTexture;
    //g_SelectedNode
    static std::shared_ptr<Texture2D> NodeSelectTexture;
    if (!NodeTexture) {
        NodeTexture = ResourceManager::LoadTexture("Resources/RegularNode_body.png");
        Node_title_Texture = ResourceManager::LoadTexture("Resources/RegularNode_title_gloss.png");
        ExecTexture = ResourceManager::LoadTexture("Resources/ExecPin_Connected.png");

        NodeSelectTexture = ResourceManager::LoadTexture("Resources/RegularNode_shadow_selected.png");
    }
    if (!NodeTexture) return;

    Rectangle dst = CalculateNodeRect(node); // если есть такая функция — используй, иначе скопируй логику
    DrawTexturePro(*NodeTexture, { 0,0,(float)NodeTexture->width,(float)NodeTexture->height }, dst, { 0,0 }, 0, Fade(node.isHovered ? LIGHTGRAY : WHITE, 0.5f));

    float titleHeight = 18.0f * graphScale; // высота полоски заголовка (настраиваемая)
    Rectangle titleDst = { dst.x, dst.y, dst.width, titleHeight }; // экранные координаты

    // Полный src по текстуре
    Rectangle titleSrc = { 0.0f, 0.0f, (float)Node_title_Texture->width, (float)Node_title_Texture->height };

    // Отрисовать фон заголовка (растянет текстуру по width/height)
    DrawTexturePro(*Node_title_Texture, titleSrc, titleDst, { 0.0f, 0.0f }, 0.0f, SKYBLUE);

 //   DrawText(node.name.c_str(), dst.x + 10 * graphScale, dst.y + 5 * graphScale, 14 * graphScale, WHITE);

    if(g_SelectedNode == node.ID)
        DrawRectangleLines(dst.x, dst.y, dst.width, dst.height, ORANGE);// DrawTexturePro(*NodeSelectTexture, { 0,0,(float)NodeTexture->width,(float)NodeTexture->height }, dst, { 0,0 }, 0, WHITE);

    

    DrawTextEx(*font, node.name.c_str(), Vector2{ dst.x + 10 * graphScale, dst.y + 5 * graphScale }, 14 * graphScale,1, WHITE);
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

    for (const auto& port : node.ports) {
        Vector2 ppos = GetPortScreenPosition(node, port.ID);
        float pinRadius = 5.0f * graphScale;

        // Если это exec-иконка — рисуем текстуру, иначе — кружок
        if (!node.isPure && (port.name == "ExecIn" || port.name == "ExecOut")) {
            // Центруем текстуру по порту
            float texScale = graphScale; // можно регулировать
            Texture2D& tex = *ExecTexture;
            Vector2 texPos = { ppos.x - (tex.width * 0.5f) * texScale, ppos.y - (tex.height * 0.5f) * texScale };
            DrawTextureEx(tex, texPos, 0.0f, texScale, WHITE);
        }
        else {
            DrawCircleV(ppos, pinRadius, port.isInput ? PURPLE : BLUE);
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
                        // разрешаем только ExecOut -> ExecIn
                        if (!srcPort.isInput && port.isInput &&
                            srcPort.name == "ExecOut" && port.name == "ExecIn") {
                            // запрет соединять ноду саму с собой, если нужно:
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

    const float HANDLE_LENGTH = 150.0f * graphScale; // Длина "ручек" изгиба
    const float BASE_LINE_THICKNESS = 4.0f * graphScale;

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
    float scaledNode = NodeSize * graphScale;
    float minWidth = scaledNode * 2;
    float minHeight = scaledNode;
    float nameWidth = node.name.size() * 8.0f * graphScale + 30.0f * graphScale;
    float width = std::max(minWidth, nameWidth);
    int portsCount = (int)node.ports.size();
    float height = std::max(minHeight, portsCount * 30.0f * graphScale);
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