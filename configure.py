#!/usr/bin/env python3
"""Splits the executable with splat (config/SCUS_941.54.yaml) and writes build.ninja, which assembles the split, links it with
splat's linker script into build/SCUS_941.54.exe and checks that file's SHA-1 against the retail one.

    .venv/bin/python configure.py
    .venv/bin/ninja

build.ninja runs this again when the splat configuration or this file change. Needs orig/USA/SCUS_941.54
(tools/extract_disc.py) and binutils for little-endian MIPS (CONTRIBUTING.md).
"""
import re
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
NAME = "SCUS_941.54"
SPLAT_CONFIG = HERE / "config" / f"{NAME}.yaml"
TARGET = HERE / "orig" / "USA" / NAME
BUILD = Path("build")
LINKER_SCRIPT = BUILD / f"{NAME}.ld"
UNDEFINED_SYMBOLS = (BUILD / "undefined_syms_auto.txt", BUILD / "undefined_funcs_auto.txt")
ELF = BUILD / f"{NAME}.elf"
EXE = BUILD / f"{NAME}.exe"
CHECKSUM = Path("config") / f"{NAME}.sha1"
BINUTILS_PREFIX = "mipsel-linux-gnu-"
# The PS-X EXE's header (2048 bytes) and its .text size, 0x4F800: the retail file is that long, its tail zeros
EXE_SIZE = 0x50000
ASFLAGS = "-EL -march=r3000 -mtune=r3000 -no-pad-sections -O1 -G0 -Iinclude"
# An object of the linker script, build/<source path without its extension>.o
LINKED_OBJECT = re.compile(r"(build/\S+?)\.o\(")


def split():
    """Runs splat, which writes asm/, include/'s macros and the linker script"""
    if not TARGET.exists():
        sys.exit(f"{TARGET} is missing: tools/extract_disc.py takes it out of your disc image (CONTRIBUTING.md)")
    subprocess.run([sys.executable, "-m", "splat", "split", str(SPLAT_CONFIG)], cwd=HERE, check=True)


def objects():
    """(object, source) of every object the linker script names, in its order"""
    found = []
    for name in dict.fromkeys(LINKED_OBJECT.findall((HERE / LINKER_SCRIPT).read_text())):
        # Not with_suffix: the stems have dots of their own (asm/data/rodata.rodata)
        source = Path(f"{Path(name).relative_to(BUILD)}.s")
        if not (HERE / source).exists():
            sys.exit(f"{LINKER_SCRIPT} links {name}.o, but there's no {source}")
        found.append((Path(f"{name}.o"), source))
    return found


def write_ninja(linked):
    binutils = {tool: BINUTILS_PREFIX + tool for tool in ("as", "ld", "objcopy")}
    missing = [command for command in binutils.values() if shutil.which(command) is None]
    if missing:
        sys.exit(f"missing {', '.join(missing)}: sudo apt install binutils-mipsel-linux-gnu")
    lines = [
        "# Written by configure.py",
        f"asflags = {ASFLAGS}",
        "",
        "rule configure",
        f"  command = {sys.executable} configure.py",
        "  generator = 1",
        "  description = configure",
        "",
        "rule as",
        f"  command = {binutils['as']} $asflags -o $out $in",
        "  description = as $in",
        "",
        "rule ld",
        f"  command = {binutils['ld']} -nostdlib --no-check-sections --no-warn-rwx-segments -T {LINKER_SCRIPT} "
        + " ".join(f"-T {path}" for path in UNDEFINED_SYMBOLS)
        + f" -Map {BUILD / NAME}.map -o $out $in",
        "  description = ld $out",
        "",
        "rule exe",
        f"  command = {binutils['objcopy']} -O binary --gap-fill 0 --pad-to {EXE_SIZE:#x} $in $out",
        "  description = exe $out",
        "",
        "rule check",
        "  command = sha1sum --quiet -c $in && touch $out",
        "  description = check $out",
        "",
        f"build build.ninja: configure | configure.py {SPLAT_CONFIG.relative_to(HERE)}",
        "",
    ]
    for obj, source in linked:
        lines.append(f"build {obj}: as {source}")
    lines += [
        "",
        f"build {ELF}: ld {' '.join(str(obj) for obj, _ in linked)} | {LINKER_SCRIPT} "
        + " ".join(str(path) for path in UNDEFINED_SYMBOLS),
        f"build {EXE}: exe {ELF}",
        f"build {BUILD / 'check.stamp'}: check {CHECKSUM} | {EXE}",
        f"build check: phony {BUILD / 'check.stamp'}",
        "default check",
        "",
    ]
    (HERE / "build.ninja").write_text("\n".join(lines))


def main():
    split()
    linked = objects()
    write_ninja(linked)
    print(f"build.ninja: {len(linked)} objects; ninja builds and checks {EXE}")


if __name__ == "__main__":
    main()
