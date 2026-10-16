#!/usr/bin/env python3
"""Splits the executable with splat (config/SCUS_941.54.yaml) and writes build.ninja, which compiles src/'s C
(tools/psx_cc.py: the original compiler, its INCLUDE_ASM functions taken from asm/nonmatchings/) and assembles the rest of the
split, links it with splat's linker script into build/SCUS_941.54.exe and checks that file's SHA-1 against the retail one.
`ninja nonmatching` builds build/nonmatching/SCUS_941.54.exe with NON_MATCHING defined, unchecked: the equivalent track
(docs/WORKFLOW.md).

    .venv/bin/python configure.py
    .venv/bin/ninja
    .venv/bin/ninja nonmatching

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
HEADER_OBJECT = BUILD / "asm" / "header.o"

# The equivalent build: the C compiled with NON_MATCHING defined, the assembly objects shared with the default build
NONMATCHING_MACRO = "NON_MATCHING"
NONMATCHING_BUILD = BUILD / "nonmatching"
NONMATCHING_HEADER = NONMATCHING_BUILD / "asm" / "header.s"
NONMATCHING_LINKER_SCRIPT = NONMATCHING_BUILD / f"{NAME}.ld"
NONMATCHING_ELF = NONMATCHING_BUILD / f"{NAME}.elf"
NONMATCHING_EXE = NONMATCHING_BUILD / f"{NAME}.exe"
# A PS-X EXE's header and its .text are whole CD sectors
SECTOR_SIZE = 0x800
# Equivalent C may change the size of what follows it, so the header's entry point and .text size come from the link: the
# fields by their comment in splat's header, and the symbol each becomes
HEADER_FIELD = re.compile(r"^\.word 0x[0-9A-Fa-f]+(?P<comment>\s+/\* (?P<field>Initial PC|\.text size) \*/)$", re.MULTILINE)
HEADER_SYMBOLS = {"Initial PC": "__SN_ENTRY_POINT", ".text size": "nonmatching_TEXT_SIZE"}
TEXT_SIZE = f"nonmatching_TEXT_SIZE = ALIGN(main_ROM_END - main_ROM_START, {SECTOR_SIZE:#x});"
LINKER_SCRIPT_END = "    /DISCARD/ :"
# The image's addresses: .text's start (config/SCUS_941.54.yaml) to the end of .bss
IMAGE_START = 0x80010000
IMAGE_END = 0x8006F1F0
# splat pins an address its split references to its retail value, though an object defines most of them with a label;
# PROVIDE leaves those to the label. The startup code clears .bss up to this one, which nothing defines
UNDEFINED_SYMBOL = re.compile(r"^(\w+) = (0x[0-9A-Fa-f]+);$")
BSS_END_SYMBOL = f"D_{IMAGE_END:08X}"
BSS_END = "main_BSS_END"
# A word of the split written as a number, not as a symbol
NUMERIC_WORD = re.compile(r"\.word (0x[0-9A-Fa-f]+)\b")
# Not checked for numbers: the header's layout fields are rewritten for the equivalent build, and .text's start stays
SPLIT_HEADER = Path("asm") / "header.s"


def split():
    """Runs splat, which writes asm/, include/'s macros and the linker script"""
    if not TARGET.exists():
        sys.exit(f"{TARGET} is missing: tools/extract_disc.py takes it out of your disc image (CONTRIBUTING.md)")
    subprocess.run([sys.executable, "-m", "splat", "split", str(SPLAT_CONFIG)], cwd=HERE, check=True)


def check_numeric_addresses():
    """Stops on a word of the split that holds an address into the image as a number: the equivalent build keeps its
    retail value while what it points at moves. config/symbol_addrs.txt gives such an address a symbol"""
    found = []
    for path in sorted((HERE / "asm").rglob("*.s")):
        source = path.relative_to(HERE)
        if source == SPLIT_HEADER:
            continue
        for number, line in enumerate(path.read_text().splitlines(), start=1):
            word = NUMERIC_WORD.search(line)
            if word is not None and IMAGE_START <= int(word.group(1), 0) < IMAGE_END:
                found.append(f"{source}:{number}: {word.group(1)}")
    if found:
        sys.exit("addresses into the image the split writes as numbers, stale in the equivalent build: "
                 "give them symbols in config/symbol_addrs.txt\n" + "\n".join(found))


def objects():
    """(object, source) of every object the linker script names, in its order; the source is C or assembly"""
    found = []
    for name in dict.fromkeys(LINKED_OBJECT.findall((HERE / LINKER_SCRIPT).read_text())):
        # Not with_suffix: the stems have dots of their own (asm/data/rodata.rodata)
        stem = Path(name).relative_to(BUILD)
        c_source = Path(f"{stem}.c")
        asm_source = Path(f"{stem}.s")
        if (HERE / c_source).exists():
            found.append((Path(f"{name}.o"), c_source))
        elif (HERE / asm_source).exists():
            found.append((Path(f"{name}.o"), asm_source))
        else:
            sys.exit(f"{LINKER_SCRIPT} links {name}.o, but there's neither {c_source} nor {asm_source}")
    return found


def c_dependencies(source):
    """What a C file's object depends on besides the file: the headers, its INCLUDE_ASM functions, the compile path"""
    unit = source.relative_to("src").with_suffix("")
    included = sorted((HERE / "asm" / "nonmatchings" / unit).glob("*.s"))
    headers = sorted((HERE / "include").glob("*"))
    return [path.relative_to(HERE) for path in included + headers] + [Path("tools") / "psx_cc.py"]


def nonmatching_link_files():
    """Writes the equivalent build's header, linker script and symbol files, from splat's, for a layout whose sizes may
    differ from the retail one. _gp keeps its retail value: the startup code loads $gp from the symbol the linker measures
    its offsets from"""
    # TRADEOFF: the handwritten code loads 20 of the game's function addresses as lui/ori pairs of numbers
    # (asm/game/80039D2C.s), which no relocation can write: they keep their retail values, stale once the C before them
    # changes size, until this build rewrites them as lui/addiu pairs of symbols
    header, fields = HEADER_FIELD.subn(lambda match: f".word {HEADER_SYMBOLS[match.group('field')]}{match.group('comment')}",
                                       (HERE / "asm" / "header.s").read_text())
    if fields != len(HEADER_SYMBOLS):
        sys.exit(f"asm/header.s: {fields} of the header's {len(HEADER_SYMBOLS)} layout fields found")
    script = (HERE / LINKER_SCRIPT).read_text()
    if LINKER_SCRIPT_END not in script:
        sys.exit(f"{LINKER_SCRIPT} has no {LINKER_SCRIPT_END.strip()} section")
    script = script.replace(f"{BUILD}/src/", f"{NONMATCHING_BUILD}/src/").replace(
        f"{HEADER_OBJECT}(", f"{NONMATCHING_BUILD / HEADER_OBJECT.relative_to(BUILD)}(").replace(
        LINKER_SCRIPT_END, f"    {TEXT_SIZE}\n{LINKER_SCRIPT_END}", 1)
    (HERE / NONMATCHING_HEADER).parent.mkdir(parents=True, exist_ok=True)
    (HERE / NONMATCHING_HEADER).write_text(header)
    (HERE / NONMATCHING_LINKER_SCRIPT).write_text(script)
    bss_end_found = False
    for path in UNDEFINED_SYMBOLS:
        lines = []
        for line in (HERE / path).read_text().splitlines():
            assignment = UNDEFINED_SYMBOL.match(line)
            if not line.strip():
                continue
            elif assignment is None:
                sys.exit(f"{path}: `{line}` isn't a symbol's assignment")
            elif assignment.group(1) == BSS_END_SYMBOL:
                lines.append(f"{BSS_END_SYMBOL} = {BSS_END};")
                bss_end_found = True
            else:
                lines.append(f"PROVIDE({assignment.group(1)} = {assignment.group(2)});")
        (HERE / NONMATCHING_BUILD / path.name).write_text("".join(f"{line}\n" for line in lines))
    if not bss_end_found:
        sys.exit(f"{BSS_END_SYMBOL}, the end of .bss, isn't in {' or '.join(str(path) for path in UNDEFINED_SYMBOLS)}")


def nonmatching_objects(linked):
    """(object, source, built here) of the equivalent build: its own C objects and header, the default build's assembly"""
    found = []
    for obj, source in linked:
        if source.suffix == ".c":
            found.append((NONMATCHING_BUILD / obj.relative_to(BUILD), source, True))
        elif obj == HEADER_OBJECT:
            found.append((NONMATCHING_BUILD / obj.relative_to(BUILD), NONMATCHING_HEADER, True))
        else:
            found.append((obj, source, False))
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
        "rule cc",
        f"  command = {sys.executable} tools/psx_cc.py $in -o $out $defines",
        "  description = cc $in",
        "",
        "rule ld",
        f"  command = {binutils['ld']} -nostdlib --no-check-sections --no-warn-rwx-segments -T $script $symbols "
        "-Map $map -o $out $in",
        "  description = ld $out",
        "",
        "rule exe",
        f"  command = {binutils['objcopy']} -O binary --gap-fill 0 --pad-to {EXE_SIZE:#x} $in $out",
        "  description = exe $out",
        "",
        "rule exe_nonmatching",
        f"  command = {binutils['objcopy']} -O binary $in $out && truncate -s %{SECTOR_SIZE} $out",
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
        if source.suffix == ".c":
            lines.append(f"build {obj}: cc {source} | {' '.join(str(path) for path in c_dependencies(source))}")
        else:
            lines.append(f"build {obj}: as {source}")
    lines += [
        "",
        f"build {ELF}: ld {' '.join(str(obj) for obj, _ in linked)} | {LINKER_SCRIPT} "
        + " ".join(str(path) for path in UNDEFINED_SYMBOLS),
        f"  script = {LINKER_SCRIPT}",
        "  symbols = " + " ".join(f"-T {path}" for path in UNDEFINED_SYMBOLS),
        f"  map = {BUILD / NAME}.map",
        f"build {EXE}: exe {ELF}",
        f"build {BUILD / 'check.stamp'}: check {CHECKSUM} | {EXE}",
        f"build check: phony {BUILD / 'check.stamp'}",
        "default check",
        "",
    ]
    nonmatching = nonmatching_objects(linked)
    nonmatching_symbols = [NONMATCHING_BUILD / path.name for path in UNDEFINED_SYMBOLS]
    for obj, source, built_here in nonmatching:
        if not built_here:
            # The default build's object
            continue
        elif source.suffix == ".c":
            lines.append(f"build {obj}: cc {source} | {' '.join(str(path) for path in c_dependencies(source))}")
            lines.append(f"  defines = -D {NONMATCHING_MACRO}")
        else:
            lines.append(f"build {obj}: as {source}")
    lines += [
        "",
        f"build {NONMATCHING_ELF}: ld {' '.join(str(obj) for obj, _, _ in nonmatching)} | {NONMATCHING_LINKER_SCRIPT} "
        + " ".join(str(path) for path in nonmatching_symbols),
        f"  script = {NONMATCHING_LINKER_SCRIPT}",
        "  symbols = " + " ".join(f"-T {path}" for path in nonmatching_symbols),
        f"  map = {NONMATCHING_BUILD / NAME}.map",
        f"build {NONMATCHING_EXE}: exe_nonmatching {NONMATCHING_ELF}",
        f"build nonmatching: phony {NONMATCHING_EXE}",
        "",
    ]
    (HERE / "build.ninja").write_text("\n".join(lines))


def main():
    split()
    check_numeric_addresses()
    linked = objects()
    nonmatching_link_files()
    write_ninja(linked)
    print(f"build.ninja: {len(linked)} objects; ninja builds and checks {EXE}, ninja nonmatching builds {NONMATCHING_EXE}")


if __name__ == "__main__":
    main()
