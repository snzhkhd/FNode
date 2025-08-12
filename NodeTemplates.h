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

    int portId = 0;

    // ≈сли нода не pure Ч добавл€ем Exec вход и выход (как у теб€ ранее)
    if (!tpl.isPure) {
        SPort execIn;
        execIn.name = "ExecIn";
        execIn.ID = portId++;
        execIn.type = EVarType::Int; // Exec не имеет семантики Ч положим Int или Bool
        execIn.isArray = false;
        execIn.isInput = true;
        node->ports.push_back(execIn);
    }

    // добавл€ем порты из шаблона (в том пор€дке, как задано)
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

    // если не pure Ч добавим ExecOut в конце (после обычных выходов)
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


// ƒобавление ноды по индексу шаблона
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
    // провер€ем, попадает ли экранна€ точка в пр€моугольник какой-либо ноды
    for (auto& nptr : file.Nodes) {
        Rectangle nr = CalculateNodeRect(*nptr); // тво€ функци€, возвращающа€ экранный rect
        if (CheckCollisionPointRec(screenPoint, nr)) return true;
    }
    return false;
}
struct ActionPalette {
    bool open = false;
    Vector2 screenPos = { 0,0 };      // экранна€ позици€ открыти€
    Rectangle rect = { 0,0,450,380 }; // размер палитры (можно мен€ть)
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

// ѕримеры шаблонов (можно расшир€ть)
static std::vector<NodeTemplate> g_NodeTemplates = {

    { "Add (Float)", true,
        {
            {"A", EVarType::Float, true},
            {"B", EVarType::Float, true},
            {"Result", EVarType::Float, false},
        }
    },

    // Subtract (float)
    { "Subtract (Float)", true,
        {
            {"A", EVarType::Float, true},
            {"B", EVarType::Float, true},
            {"Result", EVarType::Float, false},
        }
    },

    // Multiply (float)
    { "Multiply (Float)", true,
        {
            {"A", EVarType::Float, true},
            {"B", EVarType::Float, true},
            {"Result", EVarType::Float, false},
        }
    },

    // Integer Add
    { "Add (Int)", true,
        {
            {"A", EVarType::Int, true},
            {"B", EVarType::Int, true},
            {"Result", EVarType::Int, false},
        }
    },

    // Compare Greater (int) -> returns bool
    { "Greater (Int)", false,
        {
            {"A", EVarType::Int, true},
            {"B", EVarType::Int, true},
            {"Result", EVarType::Bool, false},
        }
    },

    // Boolean AND
    { "AND", true,
        {
            {"A", EVarType::Bool, true},
            {"B", EVarType::Bool, true},
            {"Result", EVarType::Bool, false},
        }
    },

    // NOT
    { "NOT", true,
        {
            {"Value", EVarType::Bool, true},
            {"Result", EVarType::Bool, false},
        }
    },

    // Branch (if) Ч делает exec ports + вход condition
    { "Branch", false,
        {
            {"Condition", EVarType::Bool, true},
            {"True", EVarType::EXEC, false},   // используем Int как "placeholder" дл€ exec
            {"False", EVarType::EXEC, false}
        }
    },

    // Pure Function example (no Exec) - FormatNumber like
    { "FormatNumber", true,
        {
            {"Value", EVarType::Int, true},
            {"Result", EVarType::String, false},
        }
    },
};