#include "ScriptParser.h"
#include <sstream>

bool FOnlineScriptParser::ParseScript(const std::string& scriptContent, ScriptFile& scriptFile)
{
    // Очищаем данные от предыдущего парсинга
    functions.clear();
    knownFunctionNames.clear();
    nextNodeId = 1;

    // Шаг 1: Извлекаем импорты
    ExtractImports(scriptContent);

    // Шаг 2: Извлекаем функции
    ExtractFunctions(scriptContent);

    // Шаг 3: Создаем узлы
    CreateNodes(scriptFile);

    for (auto f : functions)
    {
        if (f.isImported)
        {
            Imports imp;
            imp.ID = nextNodeId++;           // выделяем уникальный ID
            imp.name = f.name;               // имя функции
            imp.sourceFile = f.sourceFile;   // откуда импорт
            int portId = 0;
            for (auto& param : f.parameters) {
                SPort p;
                p.name = param.first;
                p.ID = portId++;
                p.type = param.second;
                p.isArray = false;
                p.isInput = true;
                p.isHovered = false;
                imp.ports.push_back(p);
            }

            // Выходной порт (Result) — если возврат не void
            if (f.returnType != "void" && !f.returnType.empty()) {
                SPort out;
                out.name = "Result";
                out.ID = portId++;
                out.type = ParseVarType(f.returnType);
                out.isArray = false;
                out.isInput = false;
                out.isHovered = false;
                imp.ports.push_back(out);
            }
            scriptFile.Import.push_back(imp);
        }
    }


    // Шаг 4: Создаем соединения
    CreateConnections(scriptFile);

    return !functions.empty();
}

std::string FOnlineScriptParser::RemoveComments(const std::string& code)
{
    std::string result = code;

    try {
        std::regex singleLineComment("//.*");
        result = std::regex_replace(result, singleLineComment, "");

        std::regex multiLineComment(R"(/\*[\s\S]*?\*/)");
        result = std::regex_replace(result, multiLineComment, "");
    }
    catch (const std::regex_error& e) {
        // Логируй ошибку, можно пропустить очистку комментариев
        TraceLog(LOG_WARNING, "Regex error in RemoveComments: %s", e.what());
    }

    return result;
}

void FOnlineScriptParser::ExtractImports(const std::string& code)
{
    // Регулярное выражение для импортов (сырая строка)
    std::regex importRegex("import\\s+([\\w&]+)\\s+(\\w+)\\s*\\(([^)]*)\\)\\s+from\\s+\"([^\"]+)\"");

        std::smatch match;
    std::string::const_iterator searchStart(code.cbegin());

    while (std::regex_search(searchStart, code.cend(), match, importRegex))
    {
        FunctionInfo func;
        func.returnType = match[1].str();
        func.name = match[2].str();
        func.isImported = true;
        func.sourceFile = match[4].str();

        // Парсим параметры
        ParseFunctionParameters(match[3].str(), func.parameters);

        functions.push_back(func);
        knownFunctionNames.insert(func.name);

        searchStart = match.suffix().first;
    }
}

void FOnlineScriptParser::ExtractFunctions(const std::string& code)
{
    // Удаляем комментарии перед парсингом
    std::string cleanCode = RemoveComments(code);

    const std::unordered_set<std::string> keywords = {
    "#","#if","if", "for", "while", "switch", "catch", "return", "else", "do", "try"
    };
    // Более гибкая регулярка для сигнатуры:
    std::regex funcRegex(R"(([\w:\<\>\*\&\s]+?)\s+([A-Za-z_]\w*)\s*\(([^)]*)\)\s*(?:const|noexcept|override|final)?\s*\{)",
        std::regex::ECMAScript);
    std::smatch match;
    std::string::const_iterator searchStart(cleanCode.cbegin());

    while (std::regex_search(searchStart, cleanCode.cend(), match, funcRegex)) {
        FunctionInfo func;
        func.returnType = match[1].str();
        func.returnType.erase(0, func.returnType.find_first_not_of(" \t\n\r"));
        func.returnType.erase(func.returnType.find_last_not_of(" \t\n\r") + 1);
        func.name = match[2].str();

        if (keywords.find(func.name) != keywords.end()) {
            // Если это ключевое слово, пропускаем и идём дальше
            size_t offset = static_cast<size_t>(std::distance(cleanCode.cbegin(), searchStart));
            size_t relMatchPos = static_cast<size_t>(match.position(0));
            size_t startPos = offset + relMatchPos + static_cast<size_t>(match.length(0));
            searchStart = cleanCode.cbegin() + startPos;
            continue;
        }

        // Парсим параметры
        ParseFunctionParameters(match[3].str(), func.parameters);

        // ВНИМАНИЕ: match.position() тут относительное по отношению к searchStart.
        // Надо вычислить абсолютный старт в cleanCode.
        size_t offset = static_cast<size_t>(std::distance(cleanCode.cbegin(), searchStart));
        size_t relMatchPos = static_cast<size_t>(match.position(0)); // позиция текущего матча относительно searchStart
        size_t startPos = offset + relMatchPos + static_cast<size_t>(match.length(0));

        // Ищем тело функции (учитываем вложенные скобки)
        int braceLevel = 1;
        size_t i = startPos;
        while (i < cleanCode.length() && braceLevel > 0) {
            if (cleanCode[i] == '{') braceLevel++;
            else if (cleanCode[i] == '}') braceLevel--;
            i++;
        }

        if (braceLevel == 0 && i > startPos) {
            func.body = cleanCode.substr(startPos, (i - 1) - startPos);
            functions.push_back(func);
            knownFunctionNames.insert(func.name);
        }

        // Продолжаем поиск с позиции i (абсолютной)
        searchStart = cleanCode.cbegin() + i;
    }
}

void FOnlineScriptParser::CreateNodes(ScriptFile& scriptFile)
{
    scriptFile.Nodes.clear();

    float startX = 100.0f;
    float startY = 100.0f;
    float ySpacing = 150.0f;
    float xSpacing = 250.0f;

    for (size_t i = 0; i < functions.size(); i++) {
        auto node = std::make_shared<NodeBase>();
        node->ID = nextNodeId++;
        node->name = functions[i].name;
        /*if (!functions[i].sourceFile.empty())
            node->name += " ( from \"" + functions[i].sourceFile + "\" )";*/
        node->sourceFile = functions[i].sourceFile;
        node->position = {
            startX + (i % 4) * xSpacing,
            startY + (i / 4) * ySpacing
        };

        int portId = 0;

        // Если нода не pure — даём оба exec порта: вход (ID=0, слева) и выход (ID=1, справа)
        if (!node->isPure) {
            SPort execIn;
            execIn.name = "ExecIn";
            execIn.ID = portId++;
            execIn.type = EVarType::EXEC;
            //TraceLog(LOG_INFO, "Set execIn.type = %d for node %s", (int)execIn.type, node->name.c_str());
            execIn.isArray = false;
            execIn.isInput = true;
            node->ports.push_back(execIn);

            SPort execOut;
            execOut.name = "ExecOut";
            execOut.ID = portId++;
            execOut.type = EVarType::EXEC;

            execOut.isArray = false;
            execOut.isInput = false;
            node->ports.push_back(execOut);
        }
        else {
            // Если pure — пропускаем exec порты и начинаем с ID=0 для параметров
            portId = 0;
        }

        // Входные порты для параметров (они — входы)
        for (auto& param : functions[i].parameters) {
            SPort port;
            port.name = param.first;
            port.ID = portId++;
            port.type = param.second;
            port.isArray = false;
            port.isInput = true;
            node->ports.push_back(port);
        }

        // Добавляем выходной порт для возвращаемого значения (если не void)
        if (functions[i].returnType != "void") {
            SPort port;
            port.name = "Result ( " + functions[i].returnType + " )";
            port.ID = portId++;
            port.type = ParseVarType(functions[i].returnType);
            port.isArray = false;
            port.isInput = false;
            node->ports.push_back(port);
        }

        scriptFile.Nodes.push_back(node);
    }
}

void FOnlineScriptParser::CreateConnections(ScriptFile& scriptFile)
{
    scriptFile.connections.clear();

    std::unordered_map<std::string, int> funcIdMap;
    for (auto& node : scriptFile.Nodes) {
        funcIdMap[node->name] = node->ID;
    }

    for (auto& node : scriptFile.Nodes) {
        auto funcIt = std::find_if(functions.begin(), functions.end(),
            [&](const FunctionInfo& f) { return f.name == node->name; });

        if (funcIt == functions.end() || funcIt->body.empty()) continue;

        std::string body = funcIt->body;
        // удаляем литералы чтобы не цеплять вызовы внутри строк
        try {
            body = std::regex_replace(body, std::regex(R"("([^"\\]|\\.)*")"), std::string(""));
            body = std::regex_replace(body, std::regex(R"('([^'\\]|\\.)*')"), std::string(""));
        }
        catch (...) {}

        for (const auto& targetFunc : functions) {
            if (targetFunc.name == node->name) continue;

            std::string pattern = "\\b" + targetFunc.name + "\\s*\\(";
            std::regex callRegex(pattern);
            std::sregex_iterator it(body.begin(), body.end(), callRegex);
            std::sregex_iterator end;
            for (; it != end; ++it) {
                SConnection conn;
                conn.outputNodeId = node->ID;
                // выходной exec порт у вызывающей ноды — мы зарезервировали ID=1
                conn.outputPortID = 1;

                auto targetIt = funcIdMap.find(targetFunc.name);
                if (targetIt != funcIdMap.end()) {
                    conn.inputNodeId = targetIt->second;
                    // входной exec порт у цели — ID=0
                    conn.inputPortID = 0;
                    scriptFile.connections.push_back(conn);
                }
            }
        }
    }
}


void FOnlineScriptParser::ParseFunctionParameters(const std::string& paramsStr, std::vector<std::pair<std::string, EVarType>>& params)
{
    params.clear();
    if (paramsStr.empty()) return;

    std::string s = paramsStr;
    // Trim helper
    auto trim = [](std::string& t) {
        size_t a = t.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) { t.clear(); return; }
        size_t b = t.find_last_not_of(" \t\r\n");
        t = t.substr(a, b - a + 1);
    };

    // Если единственный параметр - void
    std::string tmp = s;
    trim(tmp);
    if (tmp == "void") return;

    // Разбиваем по запятым, игнорируя запятые внутри <> и ()
    std::vector<std::string> parts;
    int angleDepth = 0;
    int parenDepth = 0;
    std::string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '<') { angleDepth++; cur.push_back(c); continue; }
        if (c == '>') { if (angleDepth > 0) angleDepth--; cur.push_back(c); continue; }
        if (c == '(') { parenDepth++; cur.push_back(c); continue; }
        if (c == ')') { if (parenDepth > 0) parenDepth--; cur.push_back(c); continue; }
        if (c == ',' && angleDepth == 0 && parenDepth == 0) {
            trim(cur);
            if (!cur.empty()) parts.push_back(cur);
            cur.clear();
        }
        else cur.push_back(c);
    }
    trim(cur);
    if (!cur.empty()) parts.push_back(cur);

    for (auto& p : parts) {
        std::string param = p;
        trim(param);
        if (param.empty()) continue;

        // Удаляем значение по умолчанию (всё после '=' на верхнем уровне)
        size_t eqPos = std::string::npos;
        int depth = 0;
        for (size_t i = 0; i < param.size(); ++i) {
            char c = param[i];
            if (c == '<') depth++;
            else if (c == '>') if (depth > 0) depth--;
            else if (c == '(') depth++;
            else if (c == ')') if (depth > 0) depth--;
            else if (c == '=' && depth == 0) { eqPos = i; break; }
        }
        if (eqPos != std::string::npos) param = param.substr(0, eqPos);
        trim(param);
        if (param.empty()) continue;

        // --- Находим имя параметра как последний идентификатор в строке ---
        // (идентификатор: [A-Za-z_][A-Za-z0-9_]*)
        int i = (int)param.size() - 1;

        // сначала пропустим возможные символы, не входящие в идентификатор (например ; , ] и т.п.)
        while (i >= 0 && !((param[i] == '_') || isalnum((unsigned char)param[i]))) i--;

        // теперь конец идентификатора
        int endId = i;
        // идём назад пока буквы/цифры/_
        while (i >= 0 && (param[i] == '_' || isalnum((unsigned char)param[i]))) i--;
        int startId = i + 1;

        std::string name;
        std::string typeStr;
        if (endId >= startId) {
            name = param.substr(startId, endId - startId + 1);
            // тип — всё, что слева от имени
            if (startId > 0) typeStr = param.substr(0, startId);
            else typeStr.clear();
        }
        else {
            // fallback — попробуем найти последний пробел (как раньше)
            size_t splitPos = std::string::npos;
            int angleDepth2 = 0, parenDepth2 = 0;
            for (int k = (int)param.size() - 1; k >= 0; --k) {
                char c = param[k];
                if (c == '>') angleDepth2++;
                else if (c == '<') if (angleDepth2 > 0) angleDepth2--;
                else if (c == ')') parenDepth2++;
                else if (c == '(') if (parenDepth2 > 0) parenDepth2--;
                else if ((c == ' ' || c == '\t') && angleDepth2 == 0 && parenDepth2 == 0) {
                    splitPos = (size_t)k;
                    break;
                }
            }
            if (splitPos != std::string::npos) {
                typeStr = param.substr(0, splitPos);
                name = param.substr(splitPos + 1);
            }
            else {
                // совсем нет — считаем весь параметр типом, генерируем имя
                typeStr = param;
                name = "arg";
            }
        }

        // Очищаем имя от ведущих/концевых &/* (вроде "&player" или "*ptr")
        while (!name.empty() && (name.front() == '&' || name.front() == '*' || isspace((unsigned char)name.front()))) name.erase(name.begin());
        while (!name.empty() && (name.back() == '&' || name.back() == '*' || isspace((unsigned char)name.back()))) name.pop_back();

        trim(typeStr);
        trim(name);

        // Если имя всё ещё пустое — fallback
        if (name.empty()) name = "arg";

        EVarType type = ParseVarType(typeStr);
        params.push_back({ name, type });

        // debug: можно раскомментировать для логов конкретной строки
        // printf("Param parsed: raw='%s' type='%s' name='%s' -> enum=%d\n", p.c_str(), typeStr.c_str(), name.c_str(), (int)type);
    }
}

EVarType FOnlineScriptParser::ParseVarType(const std::string& typeStr)
{
    // Удаляем const, volatile, лишние пробелы
    std::string t = typeStr;
    // Удаляем ключевые слова const/volatile
    std::regex kw(R"(\b(const|volatile|static)\b)");
    t = std::regex_replace(t, kw, "");
    // Удаляем пробелы вокруг, затем приводим все пробелы к одному
    t.erase(0, t.find_first_not_of(" \t\r\n"));
    t.erase(t.find_last_not_of(" \t\r\n") + 1);
    // Запомним есть ли ссылка
    bool isRef = (t.find('&') != std::string::npos);
    bool isPtr = (t.find('*') != std::string::npos);

    // Удалим символы &, * для сопоставления с мапой
    std::string cleaned;
    for (char c : t) {
        if (c == '&' || c == '*') continue;
        if (!isspace((unsigned char)c)) cleaned.push_back(c);
    }

    // Попробуем найти сначала точное соответствие (с &), затем без &
    auto it = typeMap.find(cleaned);
    if (it != typeMap.end()) return it->second;

    // попробуем варианты (например, если в typeMap есть "Critter&")
    if (isRef) {
        std::string withRef = cleaned + "&";
        it = typeMap.find(withRef);
        if (it != typeMap.end()) return it->second;
    }

    // По умолчанию — string (как раньше)
    return EVarType::String;
}

bool FOnlineScriptParser::IsFunctionCall(const std::string& line, const std::string& functionName)
{
    // Проверяем, что функция не используется как строка
    if (line.find("\"" + functionName + "\"") != std::string::npos) {
        return false;
    }

    // Используем регулярное выражение для поиска вызова функции
    std::string pattern = "\\b" + functionName + "\\s*\\(";
    std::regex regex(pattern);
    return std::regex_search(line, regex);
}

void FOnlineScriptParser::RegisterBasicTypes()
{
    // Регистрируем базовые типы FOnline
    typeMap["void"] = EVarType::Int; // Для простоты, хотя void не должен быть типом
    typeMap["int"] = EVarType::Int;
    typeMap["uint"] = EVarType::UInt;
    typeMap["uint8"] = EVarType::UInt8;
    typeMap["float"] = EVarType::Float;
    typeMap["bool"] = EVarType::Bool;
    typeMap["string"] = EVarType::String;
    typeMap["Critter"] = EVarType::Critter;
    typeMap["Critter&"] = EVarType::CritterCl;
    typeMap["ProtoItem"] = EVarType::ProtoItem;
    typeMap["Item"] = EVarType::Item;
    typeMap["Item&"] = EVarType::ItemCl;
}
