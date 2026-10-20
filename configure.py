#!/usr/bin/env python3
"""Splits the executable with splat (config/SCUS_941.54.yaml) and writes build.ninja, which compiles src/'s C
(tools/psx_cc.py: the original compiler, its INCLUDE_ASM functions taken from asm/nonmatchings/) and assembles the rest of the
split, links it with splat's linker script into build/SCUS_941.54.exe and checks that file's SHA-1 against the retail one.
`ninja nonmatching` builds build/nonmatching/SCUS_941.54.exe with NON_MATCHING defined, checked only to fit the retail
file's sectors on the disc: the equivalent track (docs/WORKFLOW.md).

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

from tools import handwritten
from tools.symbols import HEXADECIMAL, IMAGE_END, IMAGE_START, functions_by_address

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
# splat pins an address its split references to its retail value, though an object defines most of them with a label;
# PROVIDE leaves those to the label. The startup code clears .bss up to this one, which nothing defines
UNDEFINED_SYMBOL = re.compile(r"^(\w+) = (0x[0-9A-Fa-f]+);$")
BSS_END_SYMBOL = f"D_{IMAGE_END:08X}"
BSS_END = "main_BSS_END"
# A word of the split written as a number, not as a symbol
NUMERIC_WORD = re.compile(r"\.word (0x[0-9A-Fa-f]+)\b")
# Not checked for numbers: the header's layout fields are rewritten for the equivalent build, and .text's start stays
SPLIT_HEADER = Path("asm") / "header.s"
# The handwritten code (docs/TARGET.md) loads the addresses of 20 of the game's functions as numbers in 21 pairs, a lui
# then an ori in the delay slot of the branch between them, which no relocation writes for an ori (%hi assumes a signed
# low half). The equivalent build assembles a copy where each pair is a lui/addiu of the function's symbol: same size,
# same address
HANDWRITTEN = Path("asm") / "game" / "80039D2C.s"
HANDWRITTEN_OBJECT = BUILD / "asm" / "game" / "80039D2C.o"
NONMATCHING_HANDWRITTEN = NONMATCHING_BUILD / HANDWRITTEN
ADDRESS_PAIR = re.compile(r"^(?P<lui>[ ]*/\*[^*]*\*/ +lui +)(?P<register>\$\w+), \((?P<address>0x[0-9A-F]{8}) >> 16\)\n"
                          r"(?P<branch>[ ]*/\*[^*]*\*/ +[bj]\w* [^\n]*\n)"
                          r"(?P<ori>[ ]*/\*[^*]*\*/ +)ori +(?P=register), (?P=register), \((?P=address) & 0xFFFF\)$",
                          re.MULTILINE)
ADDRESS_PAIRS = 21
# The handwritten code's $gp is one more register, a pointer to its structures (docs/TARGET.md, "Addresses from the
# disc"). splat writes 23 of its offsets as %gp_rel of the small variable each reaches from the retail _gp, which the
# linker would move with .data; the equivalent build's copy writes each as the number its instruction holds (the word in
# the comment, little-endian)
GP_RELATIVE = re.compile(r"^(?P<head>[ ]*/\* [0-9A-F]+ [0-9A-F]+ (?P<word>[0-9A-F]{8}) \*/ +\w+ +\$\w+, )"
                         r"%gp_rel\(\w+\)(?P<base>\(\$gp\))", re.MULTILINE)
GP_RELATIVE_ACCESSES = 23
GP_RELATIVE_LEFT = "%gp_rel"
# A 16-bit immediate: the instruction's low halfword, and its sign
IMMEDIATE_BYTES = 2
IMMEDIATE_SIGN = 0x8000
IMMEDIATE_RANGE = 0x10000
# The high half of a number loaded with lui, left by the rewrite
NUMERIC_HIGH_HALF = re.compile(r"\blui +\$\w+, \((0x[0-9A-F]+) >> 16\)")
# The handwritten code's equivalent C (tools/handwritten.py): the equivalent build compiles it, links it after its copy of
# the handwritten code and leaves each function it replaces out of that copy (the function's block, its padding after it
# included)
HANDWRITTEN_C = handwritten.HANDWRITTEN_C
HANDWRITTEN_C_OBJECT = NONMATCHING_BUILD / HANDWRITTEN_C.with_suffix(".o")
HANDWRITTEN_FUNCTION = re.compile(r"^(?:/\* Handwritten function \*/\n)?nonmatching (\w+), 0x[0-9A-F]+\n"
                                  r"(?:(?!nonmatching |/\* Handwritten function \*/\n).*\n)*", re.MULTILINE)
# The equivalent build's own copies of the split's assembly, by the default build's object
NONMATCHING_ASSEMBLY = {HEADER_OBJECT: NONMATCHING_HEADER, HANDWRITTEN_OBJECT: NONMATCHING_HANDWRITTEN}
# The equivalent build's data follows its code wherever that ends; the executable may only grow to the retail file's
# size, the sectors tools/patch_disc.py writes it into. Where the linker's image ends in the file, the header included
IMAGE_FILE_END = "main_ROM_END"
LAYOUT_CHECK = "check-layout"
NONMATCHING_LAYOUT_STAMP = NONMATCHING_BUILD / "layout.stamp"


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
    headers = sorted(path for path in (HERE / "include").rglob("*") if path.is_file())
    return [path.relative_to(HERE) for path in included + headers] + [Path("tools") / "psx_cc.py"]


def nonmatching_handwritten():
    """The handwritten code with each lui/ori pair of a function's address made a lui/addiu pair of its symbol, and its
    offsets from $gp written as numbers"""
    functions = functions_by_address()
    pairs = 0

    def symbolic(pair):
        nonlocal pairs
        address = int(pair.group("address"), 0)
        names = functions.get(address, [])
        if not IMAGE_START <= address < IMAGE_END:
            # A constant, not an address
            return pair.group(0)
        elif len(names) != 1:
            sys.exit(f"{HANDWRITTEN}: a lui/ori pair loads {pair.group('address')}, the start of no single function "
                     f"({', '.join(names) or 'none'})")
        else:
            pairs += 1
            register = pair.group("register")
            return (f"{pair.group('lui')}{register}, %hi({names[0]})\n{pair.group('branch')}"
                    f"{pair.group('ori')}addiu {register}, {register}, %lo({names[0]})")

    def numeric(access):
        word = bytes.fromhex(access.group("word"))
        offset = int.from_bytes(word[:IMMEDIATE_BYTES], "little")
        if offset >= IMMEDIATE_SIGN:
            offset -= IMMEDIATE_RANGE
        else:
            # A positive offset
            pass
        return f"{access.group('head')}{offset:#x}{access.group('base')}"

    code = ADDRESS_PAIR.sub(symbolic, (HERE / HANDWRITTEN).read_text())
    if pairs != ADDRESS_PAIRS:
        sys.exit(f"{HANDWRITTEN}: {pairs} lui/ori pairs of a function's address, not {ADDRESS_PAIRS}")
    left = [high for high in NUMERIC_HIGH_HALF.findall(code) if IMAGE_START <= int(high, 0) < IMAGE_END]
    if left:
        sys.exit(f"{HANDWRITTEN}: lui of addresses into the image left as numbers: {', '.join(left)}")
    code, accesses = GP_RELATIVE.subn(numeric, code)
    if accesses != GP_RELATIVE_ACCESSES or GP_RELATIVE_LEFT in code:
        sys.exit(f"{HANDWRITTEN}: {accesses} accesses through $gp written as {GP_RELATIVE_LEFT}, not "
                 f"{GP_RELATIVE_ACCESSES}, or one in another form")
    in_c = handwritten.replaced()
    blocks = {block.group(1): block.group(0) for block in HANDWRITTEN_FUNCTION.finditer(code)}
    missing = sorted(in_c - set(blocks))
    if missing:
        sys.exit(f"{HANDWRITTEN_C} replaces {', '.join(missing)}, which {HANDWRITTEN} doesn't have")
    for name in sorted(in_c):
        code = code.replace(blocks[name], "", 1)
    return code


def check_layout(elf):
    """Stops when the equivalent executable outgrows the retail file's sectors on the disc"""
    symbols = {}
    listing = subprocess.run([BINUTILS_PREFIX + "nm", str(elf)], capture_output=True, text=True, check=True).stdout
    for line in listing.splitlines():
        fields = line.split()
        if len(fields) == len(("address", "type", "name")):
            symbols[fields[2]] = int(fields[0], HEXADECIMAL)
        else:
            # An undefined symbol, listed without an address
            continue
    if IMAGE_FILE_END not in symbols:
        sys.exit(f"{elf} has no {IMAGE_FILE_END}")
    over = symbols[IMAGE_FILE_END] - EXE_SIZE
    if over > 0:
        sys.exit(f"the equivalent executable is {over} bytes longer than the retail file's {EXE_SIZE} on the disc, "
                 "which tools/patch_disc.py writes it into: the equivalent C must shrink by that much")
    else:
        print(f"{elf}: {-over} bytes of the retail file's room left")


def nonmatching_link_files():
    """Writes the equivalent build's header, handwritten code, linker script and symbol files, from splat's, for a
    layout whose sizes may differ from the retail one. _gp keeps its retail value: the startup code loads $gp from the
    symbol the linker measures its offsets from"""
    header, fields = HEADER_FIELD.subn(lambda match: f".word {HEADER_SYMBOLS[match.group('field')]}{match.group('comment')}",
                                       (HERE / "asm" / "header.s").read_text())
    if fields != len(HEADER_SYMBOLS):
        sys.exit(f"asm/header.s: {fields} of the header's {len(HEADER_SYMBOLS)} layout fields found")
    script = (HERE / LINKER_SCRIPT).read_text()
    if LINKER_SCRIPT_END not in script:
        sys.exit(f"{LINKER_SCRIPT} has no {LINKER_SCRIPT_END.strip()} section")
    script = script.replace(f"{BUILD}/src/", f"{NONMATCHING_BUILD}/src/").replace(
        LINKER_SCRIPT_END, f"    {TEXT_SIZE}\n{LINKER_SCRIPT_END}", 1)
    for obj in NONMATCHING_ASSEMBLY:
        if f"{obj}(" not in script:
            sys.exit(f"{LINKER_SCRIPT} doesn't link {obj}")
        script = script.replace(f"{obj}(", f"{NONMATCHING_BUILD / obj.relative_to(BUILD)}(")
    if (HERE / HANDWRITTEN_C).exists():
        copy = re.escape(str(NONMATCHING_BUILD / HANDWRITTEN_OBJECT.relative_to(BUILD)))
        script, sections = re.subn(rf"^(?P<indent>[ \t]*){copy}\((?P<section>\.\w+)\);$",
                                   rf"\g<0>\n\g<indent>{HANDWRITTEN_C_OBJECT}(\g<section>);", script, flags=re.MULTILINE)
        if sections == 0:
            sys.exit(f"{LINKER_SCRIPT} links no section of {HANDWRITTEN_OBJECT}")
    else:
        # No handwritten function in C yet
        pass
    (HERE / NONMATCHING_HEADER).parent.mkdir(parents=True, exist_ok=True)
    (HERE / NONMATCHING_HEADER).write_text(header)
    (HERE / NONMATCHING_HANDWRITTEN).parent.mkdir(parents=True, exist_ok=True)
    (HERE / NONMATCHING_HANDWRITTEN).write_text(nonmatching_handwritten())
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
        elif obj in NONMATCHING_ASSEMBLY:
            found.append((NONMATCHING_BUILD / obj.relative_to(BUILD), NONMATCHING_ASSEMBLY[obj], True))
        else:
            found.append((obj, source, False))
        if obj == HANDWRITTEN_OBJECT and (HERE / HANDWRITTEN_C).exists():
            found.append((HANDWRITTEN_C_OBJECT, HANDWRITTEN_C, True))
        else:
            # Another object, or no handwritten function in C yet
            continue
    return found


def write_ninja(linked):
    binutils = {tool: BINUTILS_PREFIX + tool for tool in ("as", "ld", "objcopy", "nm")}
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
        "rule layout",
        f"  command = {sys.executable} configure.py {LAYOUT_CHECK} $in && touch $out",
        "  description = check the layout of $in",
        "",
        # The symbols name the split's files: a rename moves an INCLUDE_ASM's assembly to another path
        f"build build.ninja: configure | configure.py {SPLAT_CONFIG.relative_to(HERE)} config/symbol_addrs.txt",
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
        f"build {NONMATCHING_LAYOUT_STAMP}: layout {NONMATCHING_ELF} | configure.py",
        f"build nonmatching: phony {NONMATCHING_EXE} {NONMATCHING_LAYOUT_STAMP}",
        "",
    ]
    (HERE / "build.ninja").write_text("\n".join(lines))


def main():
    if len(sys.argv) == len(("configure.py", LAYOUT_CHECK, "elf")) and sys.argv[1] == LAYOUT_CHECK:
        check_layout(Path(sys.argv[2]))
        return
    elif len(sys.argv) > 1:
        sys.exit(f"usage: configure.py, or configure.py {LAYOUT_CHECK} ELF (which ninja nonmatching runs)")
    else:
        # Split and write build.ninja
        pass
    split()
    check_numeric_addresses()
    linked = objects()
    nonmatching_link_files()
    write_ninja(linked)
    print(f"build.ninja: {len(linked)} objects; ninja builds and checks {EXE}, ninja nonmatching builds {NONMATCHING_EXE}")


if __name__ == "__main__":
    main()
