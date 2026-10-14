#!/usr/bin/env python3
"""How much of the game's code is in C: for each C unit of the splat configuration, the bytes of its functions written in C
(their sizes in the built object, build/src/<unit>.o) against the unit's code bytes.

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
    total = written = 0
    for name, size in c_units():
        source = HERE / "src" / f"{name}.c"
        obj = HERE / "build" / "src" / f"{name}.o"
        if not source.exists() or not obj.exists():
            sys.exit(f"{source} or {obj} is missing: configure.py, then ninja")
        included = set(INCLUDED.findall(source.read_text()))
        symbols = subprocess.run([NM, "-S", "--defined-only", str(obj)], capture_output=True, text=True, check=True).stdout
        in_c = [(function, int(length, 16)) for length, function in SIZED_SYMBOL.findall(symbols)
                if function not in included]
        done = sum(length for _, length in in_c)
        print(f"{name}: {done} of {size} bytes in C ({done * PERCENT / size:.2f}%), {len(in_c)} functions in C, "
              f"{len(included)} still in assembly")
        total += size
        written += done
    print(f"game code: {written} of {total} bytes in C ({written * PERCENT / total:.2f}%)")


if __name__ == "__main__":
    main()
