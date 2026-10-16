#!/usr/bin/env python3
"""Compiles one function's C with every PSX GCC version in tools/toolchain and every ASPSX behaviour maspsx emulates, and
compares each result with the function's retail bytes from the split (asm/), relocated fields masked. It's how
docs/COMPILER.md's facts get proven: a match under one configuration and no other.

    python tools/probe_compiler.py FILE.c FUNCTION [--cflags "-O2 -G8 -mcpu=r8000"]
    python tools/probe_compiler.py FILE.c FUNCTION --diff 2.7.2 [--aspsx 2.34]   one configuration, side by side

The C file holds the function (and what it needs declared); FUNCTION is its name in the split (func_80011E98).
"""
import argparse
import difflib
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

from psx_cc import DEFAULT_ASPSX, DEFAULT_CFLAGS, compile_c

HERE = Path(__file__).resolve().parents[1]
TOOLCHAIN = HERE / "tools" / "toolchain"
ASM = HERE / "asm"
OBJDUMP = "mipsel-linux-gnu-objdump"
# One version from each range where maspsx changes behaviour (< 2.30, 2.30, 2.50, 2.60, 2.70, 2.80)
ASPSX_VERSIONS = ("2.21", "2.34", "2.56", "2.67", "2.77", "2.86")
# The bits a relocation fills in, by relocation type: what is masked on both sides before comparing
RELOCATION_MASKS = {
    "R_MIPS_26": 0x03FFFFFF,
    "R_MIPS_HI16": 0x0000FFFF,
    "R_MIPS_LO16": 0x0000FFFF,
    "R_MIPS_GPREL16": 0x0000FFFF,
}
INSTRUCTION_SIZE = 4
# A split instruction's comment: /* file offset, address, the word's bytes in file order */
SPLIT_WORD = re.compile(r"/\* [0-9A-F]+ [0-9A-F]{8} ([0-9A-F]{8}) \*/")
# objdump -dr: an instruction line ("  1c:	27bdffe8 	addiu ...") and a relocation line ("			1c: R_MIPS_26	sprintf")
DUMP_WORD = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]{8})\s")
DUMP_RELOCATION = re.compile(r"^\s*([0-9a-f]+): (R_MIPS_\w+)\s")
# A register operand in rabbitizer's disassembly ("$s0", "$v1", "$31"), "$gp" and "$sp" aside
REGISTER = re.compile(r"\$(?!gp\b|sp\b)\w+")


def retail_assembly(function):
    """The function's assembly in the split, from its glabel to its endlabel"""
    for path in sorted(ASM.rglob("*.s")):
        text = path.read_text()
        start = text.find(f"glabel {function}\n")
        if start < 0:
            continue
        end = text.find(f"endlabel {function}", start)
        if end < 0:
            sys.exit(f"{path}: {function} has no endlabel")
        return text[start:text.index("\n", end) + 1]
    sys.exit(f"{function} isn't in {ASM} (configure.py splits the executable)")


def retail_words(function):
    """The function's words from the split, in order"""
    return [struct.unpack("<I", bytes.fromhex(word))[0] for word in SPLIT_WORD.findall(retail_assembly(function))]


def compiled_words(object_file, function):
    """(words, {word index: mask}) of the function in a compiled object, from objdump -dr"""
    dump = subprocess.run([OBJDUMP, "-dr", "--disassemble=" + function, str(object_file)], capture_output=True, text=True,
                          check=True).stdout
    words, masks = [], {}
    first = None
    for line in dump.splitlines():
        word = DUMP_WORD.match(line)
        relocation = DUMP_RELOCATION.match(line)
        if word is not None:
            offset = int(word.group(1), 16)
            if first is None:
                first = offset
            words.append(int(word.group(2), 16))
        elif relocation is not None and first is not None:
            index = (int(relocation.group(1), 16) - first) // INSTRUCTION_SIZE
            kind = relocation.group(2)
            if kind not in RELOCATION_MASKS:
                sys.exit(f"{function}: relocation {kind} isn't handled")
            masks[index] = RELOCATION_MASKS[kind]
        else:
            continue
    return words, masks


def score(target, words, masks):
    """How many words differ, relocated fields masked; a length difference counts each missing word"""
    differing = abs(len(target) - len(words))
    for index, (expected, actual) in enumerate(zip(target, words)):
        mask = ~masks.get(index, 0) & 0xFFFFFFFF
        if expected & mask != actual & mask:
            differing += 1
    return differing


def show_diff(gcc, aspsx, source, cflags, function, target):
    """Each word's disassembly, retail beside compiled, a differing one marked"""
    import rabbitizer
    with tempfile.TemporaryDirectory() as folder:
        obj = Path(folder) / "source.o"
        error = compile_c(gcc, aspsx, source, cflags, obj)
        if error is not None:
            sys.exit(f"{gcc.name} aspsx {aspsx}: {error}")
        words, masks = compiled_words(obj, function)
    shapes = ([], [])
    for index in range(max(len(target), len(words))):
        expected = rabbitizer.Instruction(target[index]).disassemble() if index < len(target) else ""
        actual = rabbitizer.Instruction(words[index]).disassemble() if index < len(words) else ""
        mask = ~masks.get(index, 0) & 0xFFFFFFFF
        same = index < len(target) and index < len(words) and target[index] & mask == words[index] & mask
        print(f"{'  ' if same else '! '}{index * INSTRUCTION_SIZE:4X}  {expected:40} {actual}")
        for side, text in zip(shapes, (expected, actual)):
            if not text:
                continue
            elif index in masks:
                # A relocated field holds 0 in the compiled object: only the mnemonic compares
                side.append(text.split()[0])
            else:
                side.append(REGISTER.sub("$r", text))
    # Registers aside and aligned, so that one extra instruction doesn't shift all that follow: what differs in the
    # instructions themselves, their order and their constants
    aligned = difflib.SequenceMatcher(None, *shapes, autojunk=False)
    unmatched = len(shapes[0]) - sum(block.size for block in aligned.get_matching_blocks())
    print(f"{unmatched} of {len(target)} instructions without a counterpart, registers aside")
    print(f"{score(target, words, masks)} of {len(target)} words differ")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("source")
    parser.add_argument("function")
    parser.add_argument("--cflags", default=DEFAULT_CFLAGS)
    parser.add_argument("--diff", metavar="GCC_VERSION", help="show one configuration's words beside the retail ones")
    parser.add_argument("--aspsx", default=DEFAULT_ASPSX, help="the ASPSX version for --diff")
    arguments = parser.parse_args()
    source = Path(arguments.source).resolve()
    target = retail_words(arguments.function)
    if arguments.diff is not None:
        show_diff(TOOLCHAIN / f"gcc-{arguments.diff}-psx", arguments.aspsx, source, arguments.cflags, arguments.function,
                  target)
        return
    compilers = sorted(TOOLCHAIN.glob("gcc-*-psx"))
    if not compilers:
        sys.exit(f"no compilers in {TOOLCHAIN}: tools/setup_toolchain.py")

    matched = []
    with tempfile.TemporaryDirectory() as folder:
        obj = Path(folder) / "source.o"
        for gcc in compilers:
            for aspsx in ASPSX_VERSIONS:
                error = compile_c(gcc, aspsx, source, arguments.cflags, obj)
                if error is not None:
                    print(f"{gcc.name} aspsx {aspsx}: error: {error}")
                    continue
                words, masks = compiled_words(obj, arguments.function)
                differing = score(target, words, masks)
                print(f"{gcc.name} aspsx {aspsx}: {differing} of {len(target)} words differ")
                if differing == 0:
                    matched.append(f"{gcc.name} aspsx {aspsx}")
    print(f"matched with {arguments.cflags}: {', '.join(matched) if matched else 'none'}")


if __name__ == "__main__":
    main()
