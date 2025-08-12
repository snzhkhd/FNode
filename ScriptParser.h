#pragma once
#include "structs.h"
#include <regex>
#include <stack>
#include <unordered_set>
#include <unordered_map>

// Добавляем необходимые структуры
struct FunctionInfo {
    std::string name;
    std::string returnType;
    std::vector<std::pair<std::string, EVarType>> parameters;
    std::string body;
    bool isImported = false;
    std::string sourceFile; // Для импортированных функций
};

class FOnlineScriptParser {
public:
    FOnlineScriptParser() {
        // Регистрируем базовые типы
        RegisterBasicTypes();
    }

    bool ParseScript(const std::string& scriptContent, ScriptFile& scriptFile);

private:
    // Вспомогательные функции
    std::string RemoveComments(const std::string& code);
    void ExtractImports(const std::string& code);
    void ExtractFunctions(const std::string& code);
    void CreateNodes(ScriptFile& scriptFile);
    void CreateConnections(ScriptFile& scriptFile);


   

    // Обработка параметров и типов
    void ParseFunctionParameters(const std::string& paramsStr, std::vector<std::pair<std::string, EVarType>>& params);
    EVarType ParseVarType(const std::string& typeStr);
    bool IsFunctionCall(const std::string& line, const std::string& functionName);

    // Регистрация типов
    void RegisterBasicTypes();

    // Данные парсера
    std::vector<FunctionInfo> functions;

    std::unordered_set<std::string> knownFunctionNames;
    std::unordered_map<std::string, EVarType> typeMap;
    int nextNodeId = 1;
};