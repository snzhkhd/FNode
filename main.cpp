#define _CRT_SECURE_NO_WARNINGS
#include "main.h"


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
    std::string scriptContent = LoadFileContent("Resources/magic_ball.fos");

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
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------
        UpdateLayout(layoutRecs, GetScreenWidth(), GetScreenHeight());
        GraphUpdate(layoutRecs[17]);

        for (auto& nptr : CurrentFile.Nodes) {
            NodeUpdate(*nptr);
        }

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
        GuiScrollPanel(Rectangle{ layoutRecs[3].x, layoutRecs[3].y, layoutRecs[3].width - function_ScrollPanelBoundsOffset.x, layoutRecs[3].height - function_ScrollPanelBoundsOffset.y }, "", layoutRecs[3], & function_ScrollPanelScrollOffset, & function_ScrollPanelScrollView);
        GuiScrollPanel(Rectangle{ layoutRecs[4].x, layoutRecs[4].y, layoutRecs[4].width - vars_ScrollPanelBoundsOffset.x, layoutRecs[4].height - vars_ScrollPanelBoundsOffset.y }, "", layoutRecs[4], & vars_ScrollPanelScrollOffset, & vars_ScrollPanelScrollView);
        GuiScrollPanel(Rectangle{ layoutRecs[5].x, layoutRecs[5].y, layoutRecs[5].width - lvars_ScrollPanelBoundsOffset.x, layoutRecs[5].height - lvars_ScrollPanelBoundsOffset.y }, "", layoutRecs[5], & lvars_ScrollPanelScrollOffset, & lvars_ScrollPanelScrollView);
        if (GuiButton(layoutRecs[6], add_func_btnText)) AddFuncBtn();
        if (GuiButton(layoutRecs[7], add_var_btnText)) AddVarBtn();
        if (GuiButton(layoutRecs[8], add_lvar_btnText)) AddLvarBtn();

        GuiGroupBox(layoutRecs[9], browse_GroupBoxText);
        GuiGroupBox(layoutRecs[10], tool_GroupBoxText);

      
        if (GuiButton(layoutRecs[11], save_btnText)) SaveBtn();
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

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
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
        if (n) {
            Vector2 start = GetPortScreenPosition(*n, g_connDrag.portId);
            Vector2 mouse = GetMousePosition();
            // нарисуй кривую/линию между start и mouse
            DrawLineEx(start, mouse, 2.0f, BLUE);
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
    static std::shared_ptr<Texture2D> NodeTexture;
    static std::shared_ptr<Texture2D> ExecTexture;
    if (!NodeTexture) {
        NodeTexture = ResourceManager::LoadTexture("Resources/NodeTitleBackground.png");
        ExecTexture = ResourceManager::LoadTexture("Resources/ExecPin_Connected.png");
    }
    if (!NodeTexture) return;

    int NodeSize = 64;
    float scaledNode = NodeSize * graphScale;

    // Минимальные размеры
    float minWidth = scaledNode;
    float minHeight = scaledNode;

    // Вычисляем ширину по имени (пример, 8 пикселей на символ * масштаб)
    float nameWidth = node.name.size() * 8.0f * graphScale + 30.0f * graphScale; // + паддинг

    float width = std::max(minWidth, nameWidth);

    // Высота зависит от количества портов
    int portsCount = (int)node.ports.size();
    float height = std::max(minHeight, portsCount * 30.0f * graphScale);  // 20 пикс на порт, масштабируем

    // Позиция ноды на экране (с минусом graphOffset!)
    float x = node.position.x * graphScale - graphOffset.x;
    float y = node.position.y * graphScale - graphOffset.y;

    // Рисуем фон ноды растянутый по ширине и высоте
    Rectangle src = { 0, 0, (float)NodeTexture->width, (float)NodeTexture->height };
    Rectangle dst = { x, y, width, height };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(*NodeTexture, src, dst, origin, 0, node.isHovered ? LIGHTGRAY : WHITE);

    // Рисуем название сверху с паддингом
    DrawText(node.name.c_str(), x + 10 * graphScale, y + 5 * graphScale, 14 * graphScale, BLACK);

    // Рисуем порты
    float portYStart = y + 30 * graphScale; // старт по Y для портов (с отступом от заголовка)
    float portSpacing = 20.0f * graphScale;

    int inputIndex = 0;
    int outputIndex = 0;

    for (auto& port : node.ports)
    {
        bool isInput = port.isInput; // предполагается, что у порта есть поле isInput или подобное

       

        float portX, textX;
        float portY;

        float pin_offset = 5;

        if (isInput)
        {
            // Входы слева
            portX = x + 5 * graphScale;
            portY = portYStart + inputIndex * portSpacing;

            textX = portX + 12 * graphScale; // текст справа от кружка

            inputIndex++;
        }
        else
        {
            

            // Выходы справа
            portX = x + width - 5 * graphScale;
            portY = portYStart + outputIndex * portSpacing;

            textX = portX - 5 * graphScale - MeasureText(port.name.c_str(), 12 * graphScale); // текст слева от кружка
            if (!node.isPure)
            {
                //выход exec
                DrawTextureEx(*ExecTexture, Vector2{ portX - 10 ,y + 20 }, 0, 1, port.isHovered? MAGENTA : WHITE);
                outputIndex++;
            }
           

            portY = portYStart + outputIndex * portSpacing;

            outputIndex++;
        }

        if ( (!port.isArray && (port.name == "ExecIn" || port.name == "ExecOut")) && !node.isPure)
        {
            DrawTextureEx(*ExecTexture, Vector2{ dst.x + pin_offset,dst.y + 20 }, 0, 1, port.isHovered ? MAGENTA : WHITE);
        }
        else        
        {
            // Рисуем кружок порта
            DrawCircle((isInput ? (int)portX + pin_offset : (int)portX -pin_offset), (int)portY, 5 * graphScale , isInput ? PURPLE : BLUE);

            // Рисуем название порта
            DrawText(port.name.c_str(), (isInput ? (int)textX + pin_offset: (int)textX - pin_offset), (int)(portY - 6 * graphScale), 12 * graphScale, DARKGRAY);
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
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (node.isHovered && !anyPortUnderMouse) {
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
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && CheckCollisionPointRec(mouse, r)) {
        // TODO: здесь вызвать твой UI/меню
        // Example: OpenNodeContextMenu(node.ID);
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
}


void DrawConection()
{

    const float HANDLE_LENGTH = 50.0f * graphScale; // Длина "ручек" изгиба
    const float LINE_THICKNESS = 2.0f;
    Color connectionColor = BLUE;

    for (const auto& conn : CurrentFile.connections) {
        // Находим узлы по ID
        NodeBase* outputNode = nullptr;
        NodeBase* inputNode = nullptr;

        for (const auto& n : CurrentFile.Nodes) {
            if (n->ID == conn.outputNodeId) outputNode = n.get();
            if (n->ID == conn.inputNodeId) inputNode = n.get();
        }

        if (!outputNode || !inputNode) continue;

        // Получаем позиции портов
        Vector2 outputPos = GetPortScreenPosition(*outputNode, conn.outputPortID);
        Vector2 inputPos = GetPortScreenPosition(*inputNode, conn.inputPortID);

        // Рассчитываем контрольные точки для плавной кривой
        Vector2 controlPoint1 = { outputPos.x + HANDLE_LENGTH, outputPos.y };
        Vector2 controlPoint2 = { inputPos.x - HANDLE_LENGTH, inputPos.y };

        // Рисуем кривую Безье
        DrawSplineSegmentBezierCubic(
            outputPos,
            controlPoint1,
            controlPoint2,
            inputPos,
            LINE_THICKNESS,
            connectionColor
        );

        // Опционально: рисуем точку подключения на входном порту
        DrawCircleV(inputPos, LINE_THICKNESS * 2, Fade(connectionColor, 0.7f));
    }
}

Vector2 GetPortScreenPosition(const NodeBase& node, int portId) 
{
    int NodeSize = 64;
    float scaledNode = NodeSize * graphScale;

    float minWidth = scaledNode;
    float minHeight = scaledNode;

    float nameWidth = node.name.size() * 8.0f * graphScale + 30.0f * graphScale;
    float width = std::max(minWidth, nameWidth);

    int portsCount = (int)node.ports.size();
    float height = std::max(minHeight, portsCount * 30.0f * graphScale);

    float x = node.position.x * graphScale - graphOffset.x;
    float y = node.position.y * graphScale - graphOffset.y;

    float portYStart = y + 30 * graphScale;
    float portSpacing = 20.0f * graphScale;

    int inputIndex = 0;
    int outputIndex = 0;

    for (const auto& port : node.ports)
    {
        if (port.ID == portId)
        {
            bool isInput = port.isInput;

            float portX, portY;

            if (isInput)
            {
                portX = x + 5 * graphScale + 5;  // смещение для круга (pin_offset)
                portY = portYStart + inputIndex * portSpacing;
                return { portX, portY };
            }
            else
            {
                portX = x + width - 5 * graphScale - 5;
                portY = portYStart + outputIndex * portSpacing;
                return { portX, portY };
            }
        }
        if (port.isInput) inputIndex++;
        else outputIndex++;
    }
    // если порт не найден — возвращаем позицию ноды
    return { x, y };
}