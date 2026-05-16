# -*- coding: utf-8 -*-
"""Generate RuntimeConfig.h / RuntimeConfig.cpp from GlobalVariate.cpp ReadConfig()."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GV_CPP = ROOT / "GlobalVariate.cpp"

READ_START = "void ReadConfig()"
MARKER_END = "///////////////////////////////////////////////"

TYPE_MAP = {
    "Int": ("int", "toInt()", "0"),
    "Double": ("double", "toDouble()", "0.0"),
    "Bool": ("bool", "toBool()", "false"),
    "QString": ("QString", "toString()", "QString()"),
    "String": ("std::string", "toString().toStdString()", "std::string()"),
}


def extract_json_assignments(text: str):
    start = text.find(READ_START)
    if start < 0:
        raise SystemExit("ReadConfig not found")
    end = text.find(MARKER_END, start)
    if end < 0:
        raise SystemExit("end marker not found")
    segment = text[start:end]
    # 排除 #define jsonInt(x) 等宏定义行
    seg_lines = [ln for ln in segment.splitlines() if not ln.strip().startswith("#define")]
    segment = "\n".join(seg_lines)
    pat = re.compile(r"json(Int|Double|Bool|QString|String)\(([A-Za-z_]\w*)\)")
    return pat.findall(segment)


def main():
    text = GV_CPP.read_text(encoding="utf-8", errors="replace")
    pairs = extract_json_assignments(text)
    seen = set()
    ordered = []
    for kind, name in pairs:
        if name in seen:
            continue
        seen.add(name)
        ordered.append((kind, name))

    # 已从 ReadConfig 宏块移除但仍由 json 提供的键
    if "GAME_LOSE_SEC" not in seen:
        ordered.insert(0, ("Int", "GAME_LOSE_SEC"))
        seen.add("GAME_LOSE_SEC")

    h_lines = [
        "#ifndef RUNTIMECONFIG_H",
        "#define RUNTIMECONFIG_H",
        "",
        "#include <QString>",
        "#include <string>",
        "#include <QJsonObject>",
        "",
        "/** 由 config.json 填充；存储在 RuntimeConfig.cpp，对外只读（getter + 宏）。",
        " *  命令行覆盖见 RuntimeConfig_private.h（仅 GlobalVariate.cpp 可包含）。 */",
        "",
    ]

    cpp_lines = [
        '#include "RuntimeConfig.h"',
        "#include <QJsonObject>",
        "",
        "namespace {",
        "",
    ]

    for kind, name in ordered:
        ctype, _, default = TYPE_MAP[kind]
        cpp_lines.append(f"{ctype} g_{name} = {default};")

    cpp_lines += ["", "} // namespace", ""]

    for kind, name in ordered:
        ctype = TYPE_MAP[kind][0]
        h_lines.append(f"{ctype} RuntimeConfig_{name}();")
        h_lines.append(f"#define {name} (RuntimeConfig_{name}())")
        h_lines.append("")

        cpp_lines.append(f"{ctype} RuntimeConfig_{name}()")
        cpp_lines.append("{")
        cpp_lines.append(f"    return g_{name};")
        cpp_lines.append("}")
        cpp_lines.append("")

    h_lines.append("void ApplyRuntimeConfigFromJson(const QJsonObject& config);")
    h_lines.append("")
    h_lines.append("#endif // RUNTIMECONFIG_H")
    h_lines.append("")

    cpp_lines.append("void ApplyRuntimeConfigFromJson(const QJsonObject& config)")
    cpp_lines.append("{")
    for kind, name in ordered:
        conv = TYPE_MAP[kind][1]
        cpp_lines.append(
            f'    g_{name} = config.value(QStringLiteral("{name}")).{conv};'
        )
    cpp_lines.append("}")
    cpp_lines.append("")

    cpp_lines.append("void RuntimeConfig_setIsExamining(bool v) { g_IsExamining = v; }")
    cpp_lines.append("void RuntimeConfig_setOffScreen(bool v) { g_OffScreen = v; }")
    cpp_lines.append("void RuntimeConfig_setINITIAL_FREQUENCY(int v) { g_INITIAL_FREQUENCY = v; }")
    cpp_lines.append("")

    private_h = """#ifndef RUNTIMECONFIG_PRIVATE_H
#define RUNTIMECONFIG_PRIVATE_H

/** 仅由 GlobalVariate.cpp 包含：命令行覆盖 json 中的值。 */
void RuntimeConfig_setIsExamining(bool v);
void RuntimeConfig_setOffScreen(bool v);
void RuntimeConfig_setINITIAL_FREQUENCY(int v);

#endif
"""

    (ROOT / "RuntimeConfig.h").write_text("\n".join(h_lines), encoding="utf-8")
    (ROOT / "RuntimeConfig.cpp").write_text("\n".join(cpp_lines), encoding="utf-8")
    (ROOT / "RuntimeConfig_private.h").write_text(private_h, encoding="utf-8")
    (ROOT / "tools" / "runtime_config_names.txt").write_text(
        "\n".join(n for _, n in ordered), encoding="utf-8"
    )

    print("Wrote", len(ordered), "variables")


if __name__ == "__main__":
    main()
