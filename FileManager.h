#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "structs.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

static fs::path FindSavedFileForScriptName(const std::string& scriptName, const std::string& dir = "saves");



// === вспомогательные мапы для типов ===
static std::string VarTypeToString(EVarType t) {
    switch (t) {
    case EVarType::Int: return "Int";
    case EVarType::UInt: return "Uint";
    case EVarType::UInt16: return "UInt16";
    case EVarType::UInt8: return "UInt8";
    case EVarType::Float: return "Dloat";
    case EVarType::Bool: return "Bool";
    case EVarType::String: return "String";
    case EVarType::Critter: return "Critter";
    case EVarType::CritterCl: return "CritterCL";
    case EVarType::ProtoItem: return "ProtoItem";
    case EVarType::Item: return "Item";
    case EVarType::ItemCl: return "ItemCL";

    case EVarType::Enums: return "Enums";
    case EVarType::GameVar: return "GameVar";
    case EVarType::Map: return "Map";
    case EVarType::Defines: return "Defines";
    case EVarType::Colors: return "Colors";
    case EVarType::EXEC: return "EXEC";
    default: return "String";
    }
}
static EVarType StringToVarType(const std::string& s) {
    if (s == "Int") return EVarType::Int;
    if (s == "UInt") return EVarType::UInt;
    if (s == "UInt16") return EVarType::UInt16;
    if (s == "UInt8") return EVarType::UInt8;
    if (s == "Float") return EVarType::Float;
    if (s == "Bool") return EVarType::Bool;
    if (s == "String") return EVarType::String;
    if (s == "Critter") return EVarType::Critter;
    if (s == "CritterCl") return EVarType::CritterCl;
    if (s == "ProtoItem") return EVarType::ProtoItem;
    if (s == "Item") return EVarType::Item;
    if (s == "ItemCl") return EVarType::ItemCl;

    if (s == "Enums") return EVarType::Enums;
    if (s == "GameVar") return EVarType::GameVar;
    if (s == "Map") return EVarType::Map;
    if (s == "Defines") return EVarType::Defines;
    if (s == "Colors") return EVarType::Colors;
    if (s == "EXEC") return EVarType::EXEC;
    return EVarType::String;
}

// === JSON serialization helpers ===
static json PortToJson(const SPort& p) {
    json j;
    j["name"] = p.name;
    j["id"] = p.ID;
    j["type"] = VarTypeToString(p.type);
    j["isArray"] = p.isArray;
    j["isInput"] = p.isInput;
    return j;
}
static SPort JsonToPort(const json& j) {
    SPort p;
    p.name = j.value("name", std::string());
    p.ID = j.value("id", 0);
    p.type = StringToVarType(j.value("type", std::string("string")));
    p.isArray = j.value("isArray", false);
    p.isInput = j.value("isInput", true);
    p.isHovered = false;
    return p;
}

static json NodeToJson(const NodeBase& n) {
    json j;
    j["id"] = n.ID;
    j["name"] = n.name;
    j["pos"] = { n.position.x, n.position.y };
    j["isPure"] = n.isPure;
    // icon: если у тебя есть поле с путём к иконке, положи его в node.iconPath; иначе пропусти
    // Здесь попытаемся сохранить имя текстуры в node.name (OPTIONAL)
    // j["icon"] = n.iconPath;

    // ports
    j["ports"] = json::array();
    for (const auto& p : n.ports) j["ports"].push_back(PortToJson(p));

    // variables (если есть)
    j["variables"] = json::array();
    for (const auto& v : n.Variables) {
        json vv;
        vv["name"] = v.name;
        vv["id"] = v.ID;
        vv["type"] = VarTypeToString(v.type);
        j["variables"].push_back(vv);
    }

    // вложенные функции/children (если используешь)
    // можно сериализовать node.Function ... опционально
    return j;
}

static std::shared_ptr<NodeBase> JsonToNode(const json& j) {
    auto node = std::make_shared<NodeBase>();
    node->ID = j.value("id", 0);
    node->name = j.value("name", std::string());
    auto posArr = j.value("pos", std::vector<float>{0.0f, 0.0f});
    if (posArr.size() >= 2) node->position = Vector2{ posArr[0], posArr[1] };
    node->isPure = j.value("isPure", false);

    // ports
    node->ports.clear();
    for (auto& pj : j.value("ports", json::array())) {
        node->ports.push_back(JsonToPort(pj));
    }

    // variables
    node->Variables.clear();
    for (auto& vv : j.value("variables", json::array())) {
        SVariables sv;
        sv.name = vv.value("name", std::string());
        sv.ID = vv.value("id", 0);
        sv.type = StringToVarType(vv.value("type", std::string("string")));
        node->Variables.push_back(sv);
    }

    return node;
}

static json ConnectionToJson(const SConnection& c) {
    json j;
    j["inNode"] = c.inputNodeId;
    j["inPort"] = c.inputPortID;
    j["outNode"] = c.outputNodeId;
    j["outPort"] = c.outputPortID;
    return j;
}
static SConnection JsonToConnection(const json& j) {
    SConnection c;
    c.inputNodeId = j.value("inNode", 0);
    c.inputPortID = j.value("inPort", 0);
    c.outputNodeId = j.value("outNode", 0);
    c.outputPortID = j.value("outPort", 0);
    return c;
}

// === основная сериализация ScriptFile -> json ===
static json ScriptFileToJson(const ScriptFile& file) {
    json j;
    j["id"] = file.ID;
    j["name"] = file.name;
    j["nodes"] = json::array();
    for (const auto& nptr : file.Nodes) {
        j["nodes"].push_back(NodeToJson(*nptr));
    }
    j["connections"] = json::array();
    for (const auto& c : file.connections) j["connections"].push_back(ConnectionToJson(c));

    // глобальные переменные
    j["variables"] = json::array();
    for (const auto& v : file.Variables) {
        json vv;
        vv["name"] = v.name;
        vv["id"] = v.ID;
        vv["type"] = VarTypeToString(v.type);
        j["variables"].push_back(vv);
    }

    //// imports (если нужно)
    //j["imports"] = json::array();
    //for (const auto& imp : file.Importes) {
    //    json impj;
    //    impj["name"] = imp.name;
    //    impj["id"] = imp.ID;
    //    impj["from"] = imp.from;
    //    // imp ports
    //    impj["ports"] = json::array();
    //    for (const auto& p : imp.ports) impj["ports"].push_back(PortToJson(p));
    //    j["imports"].push_back(impj);
    //}

    return j;
}

// === Десериализация json -> ScriptFile ===
static bool JsonToScriptFile(const json& j, ScriptFile& out) {
    try {
        out.ID = j.value("id", 0);
        out.name = j.value("name", std::string());

        out.Nodes.clear();
        for (auto& nj : j.value("nodes", json::array())) {
            auto node = JsonToNode(nj);
            out.Nodes.push_back(node);
        }

        out.connections.clear();
        for (auto& cj : j.value("connections", json::array())) {
            out.connections.push_back(JsonToConnection(cj));
        }

        out.Variables.clear();
        for (auto& vv : j.value("variables", json::array())) {
            SVariables sv;
            sv.name = vv.value("name", std::string());
            sv.ID = vv.value("id", 0);
            sv.type = StringToVarType(vv.value("type", std::string("string")));
            out.Variables.push_back(sv);
        }

       /* out.Importes.clear();
        for (auto& ij : j.value("imports", json::array())) {
            Imports imp;
            imp.name = ij.value("name", std::string());
            imp.ID = ij.value("id", 0);
            imp.from = ij.value("from", 0);
            imp.ports.clear();
            for (auto& pj : ij.value("ports", json::array())) imp.ports.push_back(JsonToPort(pj));
            out.Importes.push_back(imp);
        }*/
    }
    catch (...) {
        return false;
    }
    return true;
}

// === файлописывающие функции ===
static std::string SanitizeFileName(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (std::isalnum((unsigned char)c) || c == '_' || c == '-') out.push_back(c);
        else if (c == ' ') out.push_back('_');
    }
    if (out.empty()) out = "script";
    return out;
}

static fs::path MakeSavePath(const ScriptFile& file, const std::string& dir = "saves") {
    fs::path d(dir);
    if (!fs::exists(d)) fs::create_directories(d);
    std::string fname = SanitizeFileName(file.name) + ".json"; //std::to_string(file.ID) + "_" + 
    return d / fname;
}

static bool SaveScriptFile(const ScriptFile& file, const std::string& dir = "saves") {
    try {
        json j = ScriptFileToJson(file);
        fs::path p = MakeSavePath(file, dir);
        std::ofstream ofs(p, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) {
            std::cerr << "Can't open file for write: " << p.string() << std::endl;
            return false;
        }
        ofs << std::setw(2) << j << std::endl;
        ofs.close();
        std::cout << "Saved: " << p.string() << std::endl;
        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Save error: " << e.what() << std::endl;
        return false;
    }
}

static bool LoadScriptFileFromPath(const fs::path& path, ScriptFile& out) {
    try {
        if (!fs::exists(path))  return false;

        std::ifstream ifs(path);
        if (!ifs.is_open()) return false;

        json j; ifs >> j; ifs.close();
        return JsonToScriptFile(j, out);
    }
    catch (std::exception& e) {
        std::cerr << "Load error: " << e.what() << std::endl;
        return false;
    }
}

static std::vector<fs::path> ListSavedFiles(const std::string& dir = "saves") {
    std::vector<fs::path> result;
    fs::path d(dir);
    if (!fs::exists(d)) return result;
    for (auto& entry : fs::directory_iterator(d)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") result.push_back(entry.path());
    }
    std::sort(result.begin(), result.end());
    return result;
}

static bool LoadScriptFileByName(const std::string& filename, ScriptFile& out, const std::string& dir = "saves") 
{
    // 1) direct path: saves/name
    fs::path p1 = fs::path(dir) / filename;
    if (LoadScriptFileFromPath(p1, out)) {
        return true;
    }

    // 2) try with .json
    fs::path p2 = p1;
    p2 += ".json";
    if (LoadScriptFileFromPath(p2, out)) {
        return true;
    }

    // 3) try exact sanitized file: <dir>/<name>.json (if name has no id-prefix)
    fs::path p3 = fs::path(dir) / (SanitizeFileName(filename) + ".json");
    if (LoadScriptFileFromPath(p3, out)) {
        return true;
    }

    // 4) try pattern search for *_<sanitized_name>.json
    fs::path found = FindSavedFileForScriptName(filename, dir);
    if (!found.empty()) {
        if (LoadScriptFileFromPath(found, out)) return true;
    }

    // not found
    return false;
}

static std::vector<ScriptFile> LoadAllSavedFiles(const std::string& dir = "saves") {
    std::vector<ScriptFile> out;
    for (auto& p : ListSavedFiles(dir)) {
        ScriptFile sf;
        if (LoadScriptFileFromPath(p, sf)) out.push_back(sf);
        else std::cerr << "Failed to load " << p << std::endl;
    }
    return out;
}

// === SaveBtn реализация (простой) ===
static void SaveBtnImplementation(const ScriptFile& current) {
    if (SaveScriptFile(current)) {
        // можно вызвать уведомление в UI
        printf("Saved current script (ID=%d, name=%s)\n", current.ID, current.name.c_str());
    }
    else {
        printf("Failed to save\n");
    }
}

static fs::path FindSavedFileForScriptName(const std::string& scriptName, const std::string& dir) {
    if (!fs::exists(dir)) return fs::path();
    std::string want = "_" + SanitizeFileName(scriptName) + ".json";
    for (auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        std::string fname = entry.path().filename().string();
        // содержит суффикс "_<sanitized_name>.json"
        if (fname.size() >= want.size() && fname.find(want) != std::string::npos) {
            return entry.path();
        }
    }
    return fs::path();
}


PortTemplate JsonToPortTemplate(const nlohmann::json& pj)
{
    std::string name = pj.value("name", "");
    EVarType type = StringToVarType(pj.value("type", "string"));

    bool isInput = pj.contains("isInput")
        ? pj.value("isInput", true)
        : !(name.empty() && type != EVarType::Defines); // логика по умолчанию

    if (pj.contains("enumValues")) {
        auto enums = pj["enumValues"].get<std::vector<std::string>>();
        return PortTemplate(name, type, isInput, enums);
    }

    return PortTemplate(name, type, isInput);
}


std::vector<NodeTemplate> LoadNodeTemplates(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return {};
    }

    nlohmann::json j;
    try {
        file >> j;
    }
    catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return {};
    }

    std::vector<NodeTemplate> templates;

    for (auto& nj : j) {
        NodeTemplate nt;
        nt.name = nj.value("name", std::string());
        nt.Category = nj.value("category", std::string());
        nt.isPure = nj.value("isPure", false);
        nt.type = nj.value("type", 0);
        nt.ToolTip = nj.value("toolTip", std::string());

        for (auto& pj : nj.value("ports", nlohmann::json::array())) {
            nt.ports.push_back(JsonToPortTemplate(pj));
        }


        templates.push_back(nt);
    }

    return templates;
}


bool LoadAllNodeTemplates(const std::string& folder)
{
   // static std::vector<NodeTemplate>& g_NodeTemplates = GetGlobalNodeTemplates(); // если у тебя глобальная

    if (!fs::exists(folder) || !fs::is_directory(folder)) {
        std::cerr << "Folder does not exist: " << folder << "\n";
        return false;
    }

    for (auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            auto templates = LoadNodeTemplates(entry.path().string());
            g_NodeTemplates.insert(g_NodeTemplates.end(), templates.begin(), templates.end());
            std::cout << "Loaded " << templates.size() << " templates from " << entry.path() << "\n";
        }
    }

    std::cout << "Total node templates loaded: " << g_NodeTemplates.size() << "\n";
    return true;
}