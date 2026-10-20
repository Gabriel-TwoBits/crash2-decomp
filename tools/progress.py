#!/usr/bin/env python3
"""How much of the game's code is in C: for each C unit of the splat configuration, the bytes of its functions written in C
(their sizes in the built object, build/src/<unit>.o) against the unit's code bytes. An equivalent function, C under
NON_MATCHING and assembly in the build (docs/WORKFLOW.md, "The equivalent track"), is counted apart, never as in C. Then
how readable the C is: the symbols named (docs/WORKFLOW.md, "Naming"), the structures defined, the fields still read at
a raw offset.

    python tools/progress.py

The build has to be current (ninja).
"""
import re
import subprocess
import sys
from pathlib import Path

import yaml

import handwritten
from symbols import ADDRESS_NAME, DATA, GAME_FUNCTION, table

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
SHARED_HEADERS = HERE / "include" / "game"
# A structure's definition, typedef'd or tagged
STRUCTURE = re.compile(r"^(?:typedef[ \t]+)?struct\b[^;{]*\{", re.MULTILINE)
# A field read at a raw offset from a pointer: *(s32 *)(p + 0x60)
RAW_OFFSET = re.compile(r"\*\s*\(\s*[A-Za-z_][\w ]*\*+\s*\)\s*\(\s*[A-Za-z_][\w.>-]*\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)\s*\)")


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


def handwritten_line():
    """The line on the handwritten code: its functions, and those the equivalent build takes from C"""
    functions = handwritten.functions()
    return (f"handwritten code: {len(handwritten.replaced() & functions)} of {len(functions)} functions equivalent, "
            "not matched")


def readability():
    """Lines on the names and types: game functions and data named, structures defined, fields read at a raw offset"""
    lines = []
    for kind, what in ((GAME_FUNCTION, "game functions"), (DATA, "data symbols")):
        names = [symbol.name for symbol in table().values() if symbol.kind == kind]
        named = [name for name in names if ADDRESS_NAME.fullmatch(name) is None]
        lines.append(f"names: {len(named)} of {len(names)} {what} named")
    sources = [path.read_text() for path in sorted((HERE / "src" / "game").glob("*.c"))]
    headers = [path.read_text() for path in sorted(SHARED_HEADERS.glob("*.h"))]
    structures = sum(len(STRUCTURE.findall(text)) for text in sources)
    shared = sum(len(STRUCTURE.findall(text)) for text in headers)
    raw = sum(len(RAW_OFFSET.findall(text)) for text in sources)
    lines.append(f"types: {structures} structures defined in src/game, {shared} in include/game; {raw} fields read at a "
                 "raw offset (*(type *)(p + offset))")
    return lines


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
    print(handwritten_line())
    for line in readability():
        print(line)


if __name__ == "__main__":
    main()
