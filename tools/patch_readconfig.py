# -*- coding: utf-8 -*-
from pathlib import Path

p = Path(__file__).resolve().parents[1] / "GlobalVariate.cpp"
t = p.read_text(encoding="utf-8")
start = t.index("    ///////////////////////////////////////////开始读取")
e = t.index("    ////////////////////////////////////////////////", start)
e = t.find("\n", e) + 1
t = t[:start] + "    ApplyRuntimeConfigFromJson(config);\n\n" + t[e:]
p.write_text(t, encoding="utf-8")
print("patched ReadConfig")
