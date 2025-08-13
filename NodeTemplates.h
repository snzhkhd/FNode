#pragma once
#include "main.h"

static std::shared_ptr<NodeBase> CreateNodeFromTemplate(const NodeTemplate& tpl, const ScriptFile& scriptFile, Vector2 position)
{
    auto node = std::make_shared<NodeBase>();
    node->ID = GetNextNodeId(scriptFile);
    node->name = tpl.name;
    node->position = position;
    node->isPure = tpl.isPure;
    node->sourceFile = ""; // по желанию
    node->ToolTip = tpl.ToolTip;
    int portId = 0;

    // Если нода не pure — добавляем Exec вход и выход (как у тебя ранее)
    if (!tpl.isPure) {
        SPort execIn;
        execIn.name = "ExecIn";
        execIn.ID = portId++;
        execIn.type = EVarType::Int; // Exec не имеет семантики — положим Int или Bool
        execIn.isArray = false;
        execIn.isInput = true;
        node->ports.push_back(execIn);
    }

    // добавляем порты из шаблона (в том порядке, как задано)
    for (const auto& pt : tpl.ports) {
        SPort p;
        p.name = std::get<0>(pt);
        p.ID = portId++;
        p.type = std::get<1>(pt);
        p.isArray = false;
        p.isInput = std::get<2>(pt);
        p.isHovered = false;
        node->ports.push_back(p);
    }

    // если не pure — добавим ExecOut в конце (после обычных выходов)
    /*if (!tpl.isPure) {
        SPort execOut;
        execOut.name = "ExecOut";
        execOut.ID = portId++;
        execOut.type = EVarType::Int;
        execOut.isArray = false;
        execOut.isInput = false;
        node->ports.push_back(execOut);
    }*/

    return node;
}


// Добавление ноды по индексу шаблона
static std::shared_ptr<NodeBase> AddNodeFromTemplate(ScriptFile& file, const NodeTemplate& tpl, Vector2 pos)
{
    auto node = CreateNodeFromTemplate(tpl, file, pos);
    file.Nodes.push_back(node);
    return node;
}

static inline std::string ToLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
    return out;
}

static bool IsPointOverAnyNode(const ScriptFile& file, Vector2 screenPoint, const Rectangle& graphArea) {
    // проверяем, попадает ли экранная точка в прямоугольник какой-либо ноды
    for (auto& nptr : file.Nodes) {
        Rectangle nr = CalculateNodeRect(*nptr); // твоя функция, возвращающая экранный rect
        if (CheckCollisionPointRec(screenPoint, nr)) return true;
    }
    return false;
}
struct ActionPalette {
    bool open = false;
    Vector2 screenPos = { 0,0 };      // экранная позиция открытия
    Rectangle rect = { 0,0,450,380 }; // размер палитры (можно менять)
    std::string search;             // строка поиска
    bool contextSensitive = true;
    Vector2 scrollOffset = { 0,0 };
    Rectangle scrollView = { 0,0,0,0 };
    int hoveredIndex = -1;
    int selectedIndex = -1;

    // двойной клик
    float lastClickTime = 0.0f;
    Vector2 lastClickPos = { 0,0 };
} g_actionPalette;

// Примеры шаблонов (можно расширять)
static std::vector<NodeTemplate> g_NodeTemplates = {

    { "Add (Float)", true,"Math | Float",
        {
            {"A", EVarType::Float, true},
            {"B", EVarType::Float, true},
            {"Result", EVarType::Float, false},
        }
    },

    // Subtract (float)
    { "Subtract (Float)", true,"Math | Float",
        {
            {"A", EVarType::Float, true},
            {"B", EVarType::Float, true},
            {"Result", EVarType::Float, false},
        }
    },

    // Multiply (float)
    { "Multiply (Float)", true,"Math | Float",
        {
            {"A", EVarType::Float, true},
            {"B", EVarType::Float, true},
            {"Result", EVarType::Float, false},
        }
    },

    // Integer Add
    { "Add (Int)", true,"Math | Int",
        {
            {"A", EVarType::Int, true},
            {"B", EVarType::Int, true},
            {"Result", EVarType::Int, false},
        }
    },

    // Compare Greater (int) -> returns bool
    { "Greater (Int)", false,"Math | Int",
        {
            {"A", EVarType::Int, true},
            {"B", EVarType::Int, true},
            {"Result", EVarType::Bool, false},
        }
    },

    // Boolean AND
    { "AND", true,"Math | Bool",
        {
            {"A", EVarType::Bool, true},
            {"B", EVarType::Bool, true},
            {"Result", EVarType::Bool, false},
        }
    },

    // NOT
    { "NOT", true,"Math | Bool",
        {
            {"Value", EVarType::Bool, true},
            {"Result", EVarType::Bool, false},
        }
    },

    // Branch (if) — делает exec ports + вход condition
    { "Branch", false,"Math | Bool",
        {
            {"Condition", EVarType::Bool, true},
            {"True", EVarType::EXEC, false},   // используем Int как "placeholder" для exec
            {"False", EVarType::EXEC, false}
        }
    },

    // Pure Function example (no Exec) - FormatNumber like
    { "valid (Critter)", true, "Server | Critter",
        {
            {"Critter", EVarType::Critter, true},
            {"Result", EVarType::Bool, false},
        }
    },
    { "valid (CritterCl)", true, "Client | Critter",
        {
            {"CritterCl", EVarType::CritterCl, true},
            {"Result", EVarType::Bool, false},
        }
    },
    { "valid (Item)", true,"Server | Item",
        {
            {"Item", EVarType::Item, true},
            {"Result", EVarType::Bool, false},
        }
    },
     { "valid (ItemCl)", true, "Client",
        {
            {"ItemCl", EVarType::ItemCl, true},
            {"Result", EVarType::Bool, false},
        }
    },
    { "valid (Map)", true, "Map",
        {
            {"Map", EVarType::Map, true},
            {"Result", EVarType::Bool, false},
        }
    },
    { "GetProtoId (Map)", true, "Client",
        {
            {"Map", EVarType::Map, true},
            {"Result", EVarType::UInt, false},
        }
    },
    { "GetProtoId (Item)", true,"Server | Item",
    {
        {"Item", EVarType::Item, true},
        {"Result", EVarType::UInt, false},
    }
    },
    { "GetProtoId (ItemCl)", true, "Client",
    {
        {"ItemCl", EVarType::ItemCl, true},
        {"Result", EVarType::UInt, false},
    }
    },
    { "GetProtoId (Critter)", true,
        {
            {"Critter", EVarType::Critter, true},
            {"Result", EVarType::UInt, false},
        }
    },
    { "GetProtoId (CritterCl)", true, "Client | Critter",
        {
            {"CritterCl", EVarType::CritterCl, true},
            {"Result", EVarType::UInt, false},
        }
    },
    { "GetChosen", true, "Client | Critter",
        {
            {"Result", EVarType::CritterCl, false},
        },
        "Позволяет получить указатель на криттера-игрока, для которого запущен клиент"
    },
    { "GetCurrentMapPid", true,"Client | Critter",
        {
            {"Result", EVarType::UInt, false},
        }
    },
    { "Log", false,
        {
            {"Out", EVarType::EXEC, false},
            {"text", EVarType::String, true},
        },
        "Заносит запись в лог-файл."
    },
    { "Random", true,
    {
        {"Value", EVarType::Int, false},
        {"Min", EVarType::Int, true},
        {"Max", EVarType::Int, true},
    },
    "Возвращает рандомное значение."
    },
    { "Message", false,
    {
        {"Out", EVarType::EXEC, false},
        {"msg", EVarType::String, true},
    }, "Выводит текст в окно сообщений клиента. "
    },
    { "GetItem", true, "Client | Item",
           {
                
                {"itemId", EVarType::UInt, true},
                {"ItemCl", EVarType::ItemCl, false},
           },
         "Ищет предмет с указанным уникальным идентификатором.\n"
            "Аргументы:\n"
            "itemId Уникальный идентификатор, по которому осуществляется поиск.\n"
            "Возвращает указатель на экземпляр класса ItemCl с указанным идентификатором.\n"
            "В случае провала – null."
    },
    { "GetProtoItem", true,"Client | Item",
       {

            {"protoId", EVarType::UInt, true},
            {"ProtoItem", EVarType::ProtoItem, false},
       }, "Ищет прототип предмета, имеющий указанный идентификатор"
    },
    { "WaitPing", false,
       {
            {"Out", EVarType::EXEC, false},
       }, "Для включения часов ожидания на время сигнала серверу и его ответа (т.е. пинг)."
    },
    { "GetDirection", false,
       {
            {"Out", EVarType::EXEC, false},
            {"Dir", EVarType::UInt8, false},
            {"fromX", EVarType::UInt, true},
            {"fromY", EVarType::UInt, true},
            {"toX", EVarType::UInt, true},
            {"toY ", EVarType::UInt, true},
       }, 
       "Определяет направление, в котором находится гексагон 2 относительно гексагона 1.\n" 
        "Аргументы:\n"
        "fromX,fromY Координаты 1 - го гексагона.\n"
        "toX,toY Координаты 2 - го гексагона.\n"
        "Возвращает : \nНаправление.Возможные значения : 0 – верх - право, 1 – право, 2 – низ - право, 3 – низ - лево и т.д. "
    },
    { "GetDistantion", false,
       {
            {"Out", EVarType::EXEC, false},
            {"Dist", EVarType::UInt, false},
            {"hexX1", EVarType::UInt, true},
            {"hexY1", EVarType::UInt, true},
            {"hexX2", EVarType::UInt, true},
            {"hexY2  ", EVarType::UInt, true},
       }
    },
    { "GetCrittersDistantion", false,
       {
            {"Out", EVarType::EXEC, false},
            {"Dist", EVarType::UInt, false},
            {"cr1", EVarType::CritterCl , true},
            {"cr2", EVarType::CritterCl , true},
       }
    },
    { "MoveHexByDir", false,
       {
            {"Out", EVarType::EXEC, false},
            {"hexX&", EVarType::UInt16, false},
            {"hexY&", EVarType::UInt16, false},
            {"dir", EVarType::UInt8, true},
            {"steps ", EVarType::UInt , true},
       }
    },
    { "GetHexCoord", false,
       {
            {"Out", EVarType::EXEC, false},
            {"fromHx", EVarType::UInt16, true},
            {"fromHy", EVarType::UInt16, true},
            {"toHx&", EVarType::UInt16, false},
            {"toHy&", EVarType::UInt16, false},
            {"angle ", EVarType::Float, true},
            {"dist ", EVarType::UInt, true},
       }
    },
    { "PlaySound", false, "Client | Sound",
       {
            {"Out", EVarType::EXEC, false},
            {"soundName", EVarType::String, true},
       }
    },
    { "PlaySound", false,"Client | Sound",
      {
           {"Out", EVarType::EXEC, false},
           {"soundType", EVarType::String, true},
           {"soundTypeExt", EVarType::String, true},
           {"soundId", EVarType::String, true},
           {"soundIdExt ", EVarType::String, true},
      }
    },
    { "PlayMusic", false,"Client | Sound",
      {
           {"Out", EVarType::EXEC, false},
           {"musicName", EVarType::String, true},
           {"pos", EVarType::UInt, true},
           {"repeat ", EVarType::UInt, true},
      }
    },
    { "GetFullSecond", false,"Client | Time",
      {
           {"Out", EVarType::EXEC, false},
           {"Full Minute", EVarType::UInt, false},
           {"year", EVarType::UInt16, true},
           {"month", EVarType::UInt16, true},
           {"day", EVarType::UInt16, true},
           {"hour", EVarType::UInt16, true},
           {"minute", EVarType::UInt16, true},
           {"second", EVarType::UInt16, true},

      }
    },
    { "GetTime", false,"Client | Time",
      {
           {"Out", EVarType::EXEC, false},
           {"year", EVarType::UInt16, true},
           {"month", EVarType::UInt16, true},
           {"day_of_week", EVarType::UInt16, true},
           {"day", EVarType::UInt16, true},
           {"hour", EVarType::UInt16, true},
           {"minute", EVarType::UInt16, true},
           {"second", EVarType::UInt16, true},
           {"milliseconds ", EVarType::UInt16, true},

      }
    },
    { "GetGameTime", false,"Client | Time",
      {
           {"Out", EVarType::EXEC, false},
           {"year", EVarType::UInt16, true},
           {"month", EVarType::UInt16, true},
           {"day_of_week", EVarType::UInt16, true},
           {"day", EVarType::UInt16, true},
           {"hour", EVarType::UInt16, true},
           {"minute", EVarType::UInt16, true},
           {"second", EVarType::UInt16, true},
           {"milliseconds ", EVarType::UInt16, true},

      }
    },
    { "LoadSprite", false, "Client | Drawing",
      {
           {"Out", EVarType::EXEC, false},
           {"ID", EVarType::UInt, false},
           {"name", EVarType::String, true},
           {"pathIndex ", EVarType::Defines, true},
      }
    },
    { "GetSpriteWidth", true,"Client | Drawing",
         {
              {"Width", EVarType::Int, false},
              {"sprId", EVarType::UInt, true},
              {"sprIndex  ", EVarType::Int, true},
         }
    },
    { "GetSpriteCount", true,"Client | Drawing",
         {
              {"Frame count", EVarType::UInt, false},
              {"sprId", EVarType::UInt, true},
         }
    },
    { "DrawSprite", false,"Client | Drawing",
     {
          {"Out", EVarType::EXEC, false},
          {"sprId", EVarType::UInt, true},
          {"sprIndex", EVarType::Int, true},
          {"x", EVarType::Int, true},
          {"y", EVarType::Int, true},
          {"color", EVarType::Colors, true},
     }
    },
    { "DrawSpriteExt", false,"Client | Drawing",
     {
          {"Out", EVarType::EXEC, false},
          {"sprId", EVarType::UInt, true},
          {"sprIndex", EVarType::Int, true},
          {"x", EVarType::Int, true},
          {"y", EVarType::Int, true},
          {"w", EVarType::Int, true},
          {"h", EVarType::Int, true},
          {"scratch", EVarType::Bool, true},
          {"center", EVarType::Bool, true},
          {"color", EVarType::Colors, true},
     }
    },
    { "DrawText", false,"Client | Drawing",
     {
          {"Out", EVarType::EXEC, false},
          {"text", EVarType::String, true},
          {"x", EVarType::Int, true},
          {"y", EVarType::Int, true},
          {"w", EVarType::Int, true},
          {"h", EVarType::Int, true},
          {"color", EVarType::Colors, true},
          {"font", EVarType::Int, true},
          {"flags", EVarType::Int, true},
     }
    },
    { "DrawPrimitive", false,"Client | Drawing",
     {
          {"Out", EVarType::EXEC, false},
          {"primitiveType", EVarType::Int, true},
          {"data&[]", EVarType::Int, true},
     }
    },
    { "DrawMapSprite", false,"Client | Drawing",
    {
         {"Out", EVarType::EXEC, false},
         {"hx", EVarType::UInt16, true},
         {"hy", EVarType::UInt16, true},
         {"effectPid", EVarType::UInt16, true},
         {"sprId", EVarType::UInt, true},
         {"sprIndex", EVarType::Int, true},
         {"offsX", EVarType::Int, true},
         {"offsY ", EVarType::Int, true},
    }
    },
    { "GetHexPos", true,"Client | Getters",
    {
         {"Result", EVarType::Bool, false},
         {"hx", EVarType::UInt16, true},
         {"hy", EVarType::UInt16, true},
         {"x", EVarType::Int, false},
         {"y", EVarType::Int, false},
    }
    },
    { "GetMonitorHex", true,"Client | Getters",
       {
            {"Result", EVarType::Bool, false},
            {"hx", EVarType::UInt16, false},
            {"hy", EVarType::UInt16, false},
            {"x", EVarType::Int, true},
            {"y", EVarType::Int, true},
       }
    },
    { "GetMonitorCritter", true,"Client | Getters",
       {
            {"CritterCl", EVarType::CritterCl, false},
            {"x", EVarType::Int, true},
            {"y", EVarType::Int, true},
       }
    },
    { "GetMonitorItem", true,"Client | Getters",
      {
           {"ItemCl", EVarType::ItemCl, false},
           {"x", EVarType::Int, true},
           {"y", EVarType::Int, true},
      }
    },
    { "GetCurrentCursor", true,"Client | Getters",
     {
          {"Cursor", EVarType::Defines, false},
     }
    },
};