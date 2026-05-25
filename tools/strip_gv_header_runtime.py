# -*- coding: utf-8 -*-
"""Remove extern lines from GlobalVariate.h for names listed in runtime_config_names.txt."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
NAMES = frozenset(
    (ROOT / "tools" / "runtime_config_names.txt").read_text(encoding="utf-8").split()
)


def should_remove(ln: str) -> bool:
    s = ln.strip()
    if not s.startswith("extern"):
        return False
    if "**" in s:
        return False
    if "(" in s and ")" not in s.split(";")[0]:
        return False
    before_semi = s.split(";")[0].strip()
    for n in NAMES:
        if before_semi.endswith(n) or before_semi.endswith("*" + n):
            return True
    return False


def main():
    h = ROOT / "GlobalVariate.h"
    lines = h.read_text(encoding="utf-8", errors="replace").splitlines(True)
    out = []
    for ln in lines:
        if should_remove(ln):
            continue
        out.append(ln)
    h.write_text("".join(out), encoding="utf-8")
    print("stripped header")


if __name__ == "__main__":
    main()
