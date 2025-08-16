import re
import json

def parse_defines(file_path):
    defines = []
    current_category = None
    enum_values = []

    with open(file_path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            # Новый блок — комментарий
            if line.startswith("//"):
                if current_category and enum_values:
                    defines.append({
                        "name": current_category,
                        "isPure": True,
                        "category": "Client | Define | Colors",
                        "ports": [
                            {
                                "name": "Type",
                                "type": "Defines",
                                "isInput": True,
                                "enumValues": enum_values
                            },
                            {
                                "name": "",
                                "type": "Int"
                            }
                        ]
                    })
                    enum_values = []

                current_category = line[2:].strip()
                continue

            # Парсим #define
            match = re.match(r"#define\s+([A-Z0-9_]+)\s*\(?.*?\)?", line)
            if match:
                name = match.group(1)
                # Обрезаем префикс до первой подчеркивания
                # if "_" in name:
                    # name = "_".join(name.split("_")[1:])
                enum_values.append(name)

    # Последний блок
    if current_category and enum_values:
        defines.append({
            "name": current_category,
            "isPure": True,
            "category": "Client | Define | Colors",
            "ports": [
                {
                    "name": "Type",
                    "type": "Defines",
                    "isInput": True,
                    "enumValues": enum_values
                },
                {
                    "name": "",
                    "type": "Int"
                }
            ]
        })

    return defines

if __name__ == "__main__":
    result = parse_defines("_colors.fos")  # путь к вашему файлу с #define
    # Сохраняем в JSON файл
    with open("_colors.json", "w", encoding="utf-8") as out_file:
        json.dump(result, out_file, indent=2, ensure_ascii=False)
