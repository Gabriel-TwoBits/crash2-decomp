#!/usr/bin/env python3
"""How much of the game's code is in C: for each C unit of the splat configuration, the bytes of its functions written in C
(their sizes in the built object, build/src/<unit>.o) against the unit's code bytes. An equivalent function, C under
NON_MATCHING and assembly in the build (docs/WORKFLOW.md, "The equivalent track"), is counted apart, never as in C.

    python tools/progress.py

The build has to be current (ninja).
"""
import re
import subprocess
import sys
from pathlib import Path

import yaml

HERE = Path(__file__).resolve().parents[1]
SPLAT_CONFIG = HERE / "config" / "SCUS_941.54.yaml"
NM = "mipsel-linux-gnu-nm"
INCLUDED = re.compile(r'INCLUDE_ASM\("[^"]+", (\w+)\);')
# An equivalent function: its C under #ifdef NON_MATCHING (no conditional of its own inside), its INCLUDE_ASM under #else
EQUIVALENT = re.compile(r'^#ifdef NON_MATCHING\n(?:(?!#ifdef NON_MATCHING\n|#else\n|#endif\n).*\n)*'
                        r'#else\nINCLUDE_ASM\("[^"]+", (\w+)\);\n#endif\n?', re.MULTILINE)
# nm -S: address, size, type (T: a function in .text), name
SIZED_SYMBOL = re.compile(r"^[0-9a-f]+ ([0-9a-f]+) [Tt] (\w+)$", re.MULTILINE)
PERCENT = 100


def c_units():
    """(name, code bytes) of the configuration's C subsegments: up to the next subsegment's start"""
    config = yaml.safe_load(SPLAT_CONFIG.read_text())
    units = []
    for segment in config["segments"]:
        if not isinstance(segment, dict):
            continue
        subsegments = segment.get("subsegments", [])
        for index, subsegment in enumerate(subsegments):
            if not isinstance(subsegment, list) or len(subsegment) < 3 or subsegment[1] != "c":
                continue
            following = subsegments[index + 1]
            end = following[0] if isinstance(following, list) else following["start"]
            units.append((subsegment[2], end - subsegment[0]))
    return units


def main():
    total = written = equivalent_total = 0
    for name, size in c_units():
        source = HERE / "src" / f"{name}.c"
        obj = HERE / "build" / "src" / f"{name}.o"
        if not source.exists() or not obj.exists():
            sys.exit(f"{source} or {obj} is missing: configure.py, then ninja")
        text = source.read_text()
        included = set(INCLUDED.findall(text))
        equivalent = set(EQUIVALENT.findall(text))
        symbols = subprocess.run([NM, "-S", "--defined-only", str(obj)], capture_output=True, text=True, check=True).stdout
        in_c = [(function, int(length, 16)) for length, function in SIZED_SYMBOL.findall(symbols)
                if function not in included]
        done = sum(length for _, length in in_c)
        line = (f"{name}: {done} of {size} bytes in C ({done * PERCENT / size:.2f}%), {len(in_c)} functions in C, "
                f"{len(included)} still in assembly")
        if equivalent:
            line += f" ({len(equivalent)} functions equivalent, not matched)"
        else:
            # No equivalent function in the unit: the line as it always was
            pass
        print(line)
        total += size
        written += done
        equivalent_total += len(equivalent)
    print(f"game code: {written} of {total} bytes in C ({written * PERCENT / total:.2f}%)")
    print(f"game code: {equivalent_total} functions equivalent, not matched")


if __name__ == "__main__":
    main()
