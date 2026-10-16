#!/usr/bin/env python3
"""Runs a function's C and its retail code side by side in an emulator (Unicorn), from the same random memory and
registers, and compares what each does: the judge of the equivalent track (docs/WORKFLOW.md). C under NON_MATCHING is
called equivalent only when every trial agrees; random trials are evidence, not a proof.

    python tools/equiv.py FILE.c FUNCTION [--trials 500] [--seed 0]

FILE.c is compiled as the build compiles it, with NON_MATCHING defined, and FUNCTION's code relocated into a scratch area:
every name the retail executable knows resolves to its retail address (the file's own .sdata and .sbss variables
included), what has no retail name (jump tables, labels, literals) to the scratch copies. Calls out of the function are
recorded and answered, not run. Compared after each trial: how it ended, the return value, the callee-saved registers,
each call's target and arguments, and every byte either side changed outside its own stack frame.

Exit status: 0 when every trial agrees, 1 on a disagreement or an error, 2 when the code needs what the emulator lacks
(the GTE). Needs the build (build/SCUS_941.54.elf) and orig/USA/SCUS_941.54.
"""
import argparse
import bisect
import random
import re
import struct
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from unicorn import UC_ARCH_MIPS, UC_HOOK_CODE, UC_HOOK_INTR, UC_HOOK_MEM_UNMAPPED, UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS32
from unicorn import Uc, UcError
from unicorn import mips_const as mips

from psx_cc import DEFAULT_ASPSX, DEFAULT_CFLAGS, DEFAULT_GCC, HERE, compile_c, compiler, cpp_command

RETAIL_ELF = HERE / "build" / "SCUS_941.54.elf"
RETAIL_EXE = HERE / "orig" / "USA" / "SCUS_941.54"
NONMATCHING_MACRO = "NON_MATCHING"
# The wrapper INCLUDE_ASM puts around a function left as assembly (include/include_asm.h)
INCLUDE_ASM_WRAPPER = "__maspsx_include_asm_hack_"
DEFAULT_TRIALS = 500
EXIT_DISAGREE = 1
EXIT_UNSUPPORTED = 2
MAX_DIFFERENCES_SHOWN = 10

# PS-X EXE: a header, then one block loaded at a fixed address
EXE_MAGIC = b"PS-X EXE"
EXE_HEADER_SIZE = 0x800
EXE_LOAD_ADDRESS_OFFSET = 0x18
EXE_LOAD_SIZE_OFFSET = 0x1C
# Linker script symbols: the retail image keeps its read-only data and code. Each word of its data keeps its retail value
# or is random, at even odds per trial: tables read as constants stay meaningful in some trials, state variables vary in
# others. Its .bss is random like the rest of RAM
GP_SYMBOL = "_gp"
LINKER_MARKER = re.compile(r"^(?:main|header)_|^_gp$")
KEPT_START_SYMBOL = "main_VRAM"
KEPT_END_SYMBOL = "main_DATA_START"

# ELF32, little-endian
ELF_MAGIC = b"\x7fELF"
ELF_CLASS_OFFSET = 4
ELF_DATA_OFFSET = 5
ELFCLASS32 = 1
ELFDATA2LSB = 1
ELF_SHOFF_OFFSET = 0x20
ELF_SHENTSIZE_OFFSET = 0x2E
SECTION_HEADER = struct.Struct("<10I")
SYMBOL_ENTRY = struct.Struct("<IIIBBH")
REL_ENTRY = struct.Struct("<II")
SHT_PROGBITS = 1
SHT_SYMTAB = 2
SHT_NOBITS = 8
SHT_REL = 9
SHT_MIPS_REGINFO = 0x70000006
SHF_ALLOC = 0x2
SHF_EXECINSTR = 0x4
SHN_UNDEF = 0
SHN_ABS = 0xFFF1
SHN_COMMON = 0xFFF2
STB_LOCAL = 0
STT_NOTYPE = 0
STT_SECTION = 3
STT_FILE = 4
SYMBOL_BIND_SHIFT = 4
SYMBOL_TYPE_MASK = 0xF
REL_SYMBOL_SHIFT = 8
REL_TYPE_MASK = 0xFF
# .reginfo: the gp value the object was assembled against (ri_gp_value), added to a local GPREL16's addend
REGINFO_GP_OFFSET = 20

R_MIPS_NONE = 0
R_MIPS_32 = 2
R_MIPS_26 = 4
R_MIPS_HI16 = 5
R_MIPS_LO16 = 6
R_MIPS_GPREL16 = 7
WORD = 4
WORD_MASK = 0xFFFFFFFF
HALF_MASK = 0xFFFF
HALF_BITS = 16
HALF_SIGN = 0x8000
# A %hi rounds up when its %lo is negative
HALF_ROUNDING = 0x8000
JUMP_FIELD = 0x03FFFFFF
JUMP_SHIFT = 2
JUMP_SEGMENT = 0xF0000000
OPCODE_SHIFT = 26
# The GTE's instructions (coprocessor 2), which Unicorn's MIPS32 doesn't have
GTE_OPCODES = {0x12: "cop2", 0x32: "lwc2", 0x3A: "swc2"}
GTE_SHOWN = 3

# Address space. Unicorn maps kseg0 and kseg1 to physical memory as the PS1 does (address & 0x1FFFFFFF) and kuseg to
# itself; its memory calls and hooks take physical addresses, its code hooks virtual ones. kseg2 needs a TLB and faults.
# Every region is mapped and filled before a run, none on demand: Unicorn 2.1.4 resumes a store in a taken branch's delay
# slot without the branch once a memory hook has run on it (a reserved-instruction exception follows)
PAGE_SIZE = 0x1000
KSEG0 = 0x80000000
KSEG2 = 0xC0000000
PHYSICAL_MASK = 0x1FFFFFFF
# The PS1's 2 MB of RAM and its 1 KB data cache used as fast RAM (one page mapped)
RAM_BASE = 0x80000000
RAM_SIZE = 0x200000
SCRATCHPAD_BASE = 0x1F800000
# The candidate's sections, past RAM; mapped in both runs alike
SCRATCH_BASE = 0x80400000
SECTION_ALIGNMENT = 16
# $ra on entry: reaching it ends the run
SENTINEL_RETURN = 0x80FFF000
ENTRY_SP = 0x801FFE00
# Below the entry $sp: the function's frame and anything it pushes, left out of the comparison
STACK_RESERVE = 0x10000
# o32: the caller's first 16 bytes belong to the callee, which may store $a0-$a3 there
ARGUMENT_HOME_SIZE = 16
ARGUMENT_REGISTERS = (mips.UC_MIPS_REG_A0, mips.UC_MIPS_REG_A1, mips.UC_MIPS_REG_A2, mips.UC_MIPS_REG_A3)
# The arguments a trial sets: $a0-$a3, then words on the caller's stack
ARGUMENT_WORDS = 8
# The words a call's argument pointing into the caller's frame is compared by
FRAME_WORDS = 4
# One pointer argument in this many is NULL
NULL_POINTER_ODDS = 10
# A run stops at either: a loop waiting on what a callee would change spins until then
MAX_INSTRUCTIONS = 500_000
MAX_CALLS = 1_000

# Random words: zeros, small numbers either side of zero, pointers into an arena whose own words are drawn the same way
# (so that pointer chasing goes several levels deep), and any 32 bits. RAM outside the retail code and read-only data,
# and the scratchpad, start this way: each page a slice of a pool drawn once per seed, at an offset drawn per trial
ARENA_BASE = 0x80100000
ARENA_SIZE = 0x8000
# A small number's magnitude: up to one of these, picked at even odds, so that 1 comes often (flags, modes) and counts
# reach a few hundred now and then
SMALL_LIMITS = (1, 4, 16, 256)
FUZZ_ZERO, FUZZ_SMALL, FUZZ_NEGATIVE, FUZZ_POINTER, FUZZ_ANY, FUZZ_CONSTANT = range(6)
FUZZ_KINDS = (FUZZ_ZERO, FUZZ_SMALL, FUZZ_NEGATIVE, FUZZ_POINTER, FUZZ_ANY, FUZZ_CONSTANT)
FUZZ_WEIGHTS = (25, 20, 10, 25, 5, 15)
# The constants of the retail function's immediates, so that equality tests and bounds get hit: each one, its
# neighbours and its negation. By opcode: sign-extended, zero-extended, or the upper half (lui)
SIGNED_IMMEDIATES = {0x08, 0x09, 0x0A, 0x0B}
UNSIGNED_IMMEDIATES = {0x0C, 0x0D, 0x0E}
UPPER_IMMEDIATE = 0x0F
WORD_BITS = 32
POOL_WORDS = 0x40000

REGISTER_NAMES = ("at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2",
                  "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra")
REGISTERS = {name: getattr(mips, f"UC_MIPS_REG_{name.upper()}") for name in REGISTER_NAMES}
# Set at random on entry: all but $zero, $gp, $sp, $ra
RANDOM_ON_ENTRY = [name for name in REGISTER_NAMES if name not in ("gp", "sp", "ra")]
# What a call may change: set at random after each call out
CLOBBERED_BY_CALLS = ("at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9")
CALLEE_SAVED = ("s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "fp", "gp", "sp")
MULTIPLY_REGISTERS = (mips.UC_MIPS_REG_HI, mips.UC_MIPS_REG_LO)

# How a run ended
RETURNED = "returned"
LIMIT = "the limit"
# Unicorn's exception numbers (QEMU's), as UC_HOOK_INTR reports them
EXCEPTIONS = {12: "address error on load", 13: "address error on store", 17: "syscall", 18: "break", 21: "overflow",
              22: "trap", 26: "TLB miss on load", 27: "TLB miss on store"}
UNSUPPORTED_EXCEPTIONS = {19: "coprocessor unusable", 20: "reserved instruction"}

# A name followed by an argument list, in the preprocessed C: a prototype, a definition or a call
NAME_BEFORE_PARENTHESIS = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
VARIADIC = "..."
LONG_RETURN = r"\b(?:long\s+long|s64|u64)\s+"


@dataclass
class Section:
    name_offset: int
    type: int
    flags: int
    addr: int
    offset: int
    size: int
    link: int
    info: int
    addralign: int
    entsize: int
    name: str = ""


@dataclass
class Symbol:
    name: str
    value: int
    size: int
    bind: int
    type: int
    shndx: int


class Elf:
    """The parts of an ELF32 little-endian file used here: sections, the symbol table, REL relocations"""

    def __init__(self, path):
        if not path.exists():
            sys.exit(f"{path} is missing")
        data = path.read_bytes()
        if data[:len(ELF_MAGIC)] != ELF_MAGIC or data[ELF_CLASS_OFFSET] != ELFCLASS32 or data[ELF_DATA_OFFSET] != ELFDATA2LSB:
            sys.exit(f"{path}: not a 32-bit little-endian ELF")
        self.path = path
        self.data = data
        (section_offset,) = struct.unpack_from("<I", data, ELF_SHOFF_OFFSET)
        entry_size, count, names_index = struct.unpack_from("<HHH", data, ELF_SHENTSIZE_OFFSET)
        self.sections = [Section(*SECTION_HEADER.unpack_from(data, section_offset + index * entry_size))
                         for index in range(count)]
        names = self.sections[names_index]
        for section in self.sections:
            section.name = self.string(names, section.name_offset)
        tables = [section for section in self.sections if section.type == SHT_SYMTAB]
        if len(tables) != 1:
            sys.exit(f"{path}: {len(tables)} symbol tables")
        table = tables[0]
        strings = self.sections[table.link]
        self.symbols = []
        for offset in range(table.offset, table.offset + table.size, SYMBOL_ENTRY.size):
            name, value, size, info, _, shndx = SYMBOL_ENTRY.unpack_from(data, offset)
            self.symbols.append(Symbol(self.string(strings, name), value, size, info >> SYMBOL_BIND_SHIFT,
                                       info & SYMBOL_TYPE_MASK, shndx))

    def string(self, section, offset):
        start = section.offset + offset
        return self.data[start:self.data.index(b"\0", start)].decode()

    def contents(self, section):
        return self.data[section.offset:section.offset + section.size]

    def relocations(self, target):
        """(offset, type, symbol index) of the REL relocations applying to section index target, in order"""
        found = []
        for section in self.sections:
            if section.type != SHT_REL or section.info != target:
                continue
            for offset in range(section.offset, section.offset + section.size, REL_ENTRY.size):
                place, info = REL_ENTRY.unpack_from(self.data, offset)
                found.append((place, info & REL_TYPE_MASK, info >> REL_SYMBOL_SHIFT))
        return found


def physical(address):
    """The physical address Unicorn keeps a virtual one at"""
    if KSEG0 <= address < KSEG2:
        return address & PHYSICAL_MASK
    else:
        return address


def virtual(address):
    """A physical address as the game sees it, in kseg0 when it's below 512 MB"""
    if address <= PHYSICAL_MASK:
        return address | KSEG0
    else:
        return address


def signed16(value):
    return value - (1 << HALF_BITS) if value & HALF_SIGN else value


def read_word(buffer, offset):
    return struct.unpack_from("<I", buffer, offset)[0]


def write_word(buffer, offset, value):
    struct.pack_into("<I", buffer, offset, value & WORD_MASK)


class Retail:
    """The retail executable: its image, its symbols (from the build's ELF, every symbol at its retail address)"""

    def __init__(self):
        elf = Elf(RETAIL_ELF)
        self.addresses = {}
        self.sizes = {}
        # Global symbols first: a local of the same name doesn't shadow one
        for symbol in sorted(elf.symbols, key=lambda symbol: symbol.bind == STB_LOCAL):
            if not symbol.name or symbol.shndx == SHN_UNDEF or symbol.type in (STT_SECTION, STT_FILE):
                continue
            self.addresses.setdefault(symbol.name, symbol.value)
            self.sizes.setdefault(symbol.name, symbol.size)
        for name in (GP_SYMBOL, KEPT_START_SYMBOL, KEPT_END_SYMBOL):
            if name not in self.addresses:
                sys.exit(f"{RETAIL_ELF} has no {name}")
        self.gp = self.addresses[GP_SYMBOL]
        # Nearest name at or below an address, for the report; splat's aliases and the linker script's markers aside
        named = sorted((address, name) for name, address in self.addresses.items()
                       if "." not in name and not LINKER_MARKER.match(name))
        self.named_addresses = [address for address, _ in named]
        self.named = [name for _, name in named]

        if not RETAIL_EXE.exists():
            sys.exit(f"{RETAIL_EXE} is missing: tools/extract_disc.py takes it out of your disc image")
        exe = RETAIL_EXE.read_bytes()
        if exe[:len(EXE_MAGIC)] != EXE_MAGIC:
            sys.exit(f"{RETAIL_EXE} isn't a PS-X EXE")
        (load_address,) = struct.unpack_from("<I", exe, EXE_LOAD_ADDRESS_OFFSET)
        (load_size,) = struct.unpack_from("<I", exe, EXE_LOAD_SIZE_OFFSET)
        self.kept_start = self.addresses[KEPT_START_SYMBOL]
        self.kept_end = self.addresses[KEPT_END_SYMBOL]
        if not load_address <= self.kept_start < self.kept_end <= load_address + load_size:
            sys.exit(f"{RETAIL_ELF}'s {KEPT_START_SYMBOL}..{KEPT_END_SYMBOL} isn't inside {RETAIL_EXE}'s image")
        image = exe[EXE_HEADER_SIZE:EXE_HEADER_SIZE + load_size]
        self.kept = image[self.kept_start - load_address:self.kept_end - load_address]
        self.data = image[self.kept_end - load_address:]

    def function(self, name):
        """(address, size) of a retail function"""
        if name not in self.addresses:
            sys.exit(f"{name} isn't a symbol of {RETAIL_ELF}")
        address, size = self.addresses[name], self.sizes[name]
        if size == 0:
            sys.exit(f"{name} has no size in {RETAIL_ELF}")
        if not (self.kept_start <= address and address + size <= self.kept_end):
            sys.exit(f"{name} ({address:#010x}) isn't in the retail code")
        return address, size

    def describe(self, address):
        """An address by the nearest name at or below it"""
        index = bisect.bisect_right(self.named_addresses, address) - 1
        if index < 0:
            return f"{address:#010x}"
        offset = address - self.named_addresses[index]
        if offset == 0:
            return f"{self.named[index]} ({address:#010x})"
        else:
            return f"{self.named[index]}+{offset:#x} ({address:#010x})"


@dataclass
class Candidate:
    """The C's function relocated into the scratch area"""
    image: bytes
    entry: int
    size: int


def compile_candidate(source, function, retail):
    """Compiles source with NON_MATCHING defined and relocates its sections at SCRATCH_BASE"""
    with tempfile.TemporaryDirectory() as folder:
        obj = Path(folder) / "candidate.o"
        error = compile_c(compiler(DEFAULT_GCC), DEFAULT_ASPSX, source, DEFAULT_CFLAGS, obj, (NONMATCHING_MACRO,))
        if error is not None:
            sys.exit(f"{source}: {error}")
        elf = Elf(obj)

    names = {symbol.name for symbol in elf.symbols}
    if INCLUDE_ASM_WRAPPER + function in names:
        sys.exit(f"{function} is assembly in {source} (INCLUDE_ASM) with {NONMATCHING_MACRO} defined: no C to test")
    defined = [symbol for symbol in elf.symbols if symbol.name == function and symbol.shndx not in (SHN_UNDEF, SHN_ABS,
                                                                                                    SHN_COMMON)]
    if not defined:
        sys.exit(f"{source} doesn't define {function} with {NONMATCHING_MACRO} defined")
    entry_symbol = defined[0]
    text = elf.sections[entry_symbol.shndx]
    if not text.flags & SHF_EXECINSTR:
        sys.exit(f"{function} is in {text.name}, not in code")
    if entry_symbol.size == 0:
        sys.exit(f"{function} has no size in the compiled object")

    # Every allocated section of the object, one after another
    bases = {}
    cursor = SCRATCH_BASE
    for index, section in enumerate(elf.sections):
        if section.flags & SHF_ALLOC and section.type in (SHT_PROGBITS, SHT_NOBITS):
            cursor = align(cursor, max(section.addralign, SECTION_ALIGNMENT))
            bases[index] = cursor
            cursor += section.size
        else:
            continue
    commons = {}
    for symbol in elf.symbols:
        if symbol.shndx == SHN_COMMON and symbol.name not in retail.addresses:
            # A common's value is its alignment
            cursor = align(cursor, max(symbol.value, WORD))
            commons[symbol.name] = cursor
            cursor += symbol.size
        else:
            continue
    image = bytearray(cursor - SCRATCH_BASE)
    for index, base in bases.items():
        section = elf.sections[index]
        if section.type == SHT_PROGBITS:
            image[base - SCRATCH_BASE:base - SCRATCH_BASE + section.size] = elf.contents(section)
        else:
            # .bss and .sbss: zeros
            pass
    reginfo = [section for section in elf.sections if section.type == SHT_MIPS_REGINFO]
    gp0 = read_word(elf.data, reginfo[0].offset + REGINFO_GP_OFFSET) if reginfo else 0

    def resolve(symbol):
        if symbol.name == function:
            # A recursive call runs the candidate, as the retail function's runs the retail code
            return bases[entry_symbol.shndx] + entry_symbol.value
        elif (symbol.name and symbol.type not in (STT_SECTION, STT_FILE) and symbol.name in retail.addresses
              and not (symbol.bind == STB_LOCAL and symbol.type == STT_NOTYPE)):
            # A name the retail executable knows (an assembler's local label aside): its retail address
            return retail.addresses[symbol.name]
        elif symbol.shndx == SHN_UNDEF:
            sys.exit(f"{source}: {symbol.name or 'an unnamed symbol'} is undefined and isn't a symbol of {RETAIL_ELF}")
        elif symbol.shndx == SHN_COMMON:
            return commons[symbol.name]
        elif symbol.shndx == SHN_ABS:
            return symbol.value
        elif symbol.shndx in bases:
            return bases[symbol.shndx] + symbol.value
        else:
            sys.exit(f"{source}: {symbol.name or 'a symbol'} is in {elf.sections[symbol.shndx].name}, which isn't loaded")

    for target, base in bases.items():
        relocations = elf.relocations(target)
        if target == entry_symbol.shndx:
            # Of the code, only the function's: the file's other functions are reached at their retail addresses
            relocations = [relocation for relocation in relocations
                           if entry_symbol.value <= relocation[0] < entry_symbol.value + entry_symbol.size]
        pending_high = []
        for offset, kind, symbol_index in relocations:
            if kind == R_MIPS_NONE:
                continue
            symbol = elf.symbols[symbol_index]
            position = base - SCRATCH_BASE + offset
            value = resolve(symbol)
            word = read_word(image, position)
            if kind == R_MIPS_32:
                write_word(image, position, word + value)
            elif kind == R_MIPS_26:
                destination = value + ((word & JUMP_FIELD) << JUMP_SHIFT)
                if destination & JUMP_SEGMENT != (base + offset + WORD) & JUMP_SEGMENT:
                    sys.exit(f"{source}: a jump to {symbol.name} ({destination:#010x}) leaves its 256 MB segment")
                write_word(image, position, (word & ~JUMP_FIELD) | ((destination >> JUMP_SHIFT) & JUMP_FIELD))
            elif kind == R_MIPS_HI16:
                # Completed by the LO16 that follows it, whose addend holds the low half
                pending_high.append((position, symbol_index))
            elif kind == R_MIPS_LO16:
                low = signed16(word & HALF_MASK)
                for high_position, high_symbol in [pair for pair in pending_high if pair[1] == symbol_index]:
                    high_word = read_word(image, high_position)
                    full = value + ((high_word & HALF_MASK) << HALF_BITS) + low
                    write_word(image, high_position,
                               (high_word & ~HALF_MASK) | (((full + HALF_ROUNDING) >> HALF_BITS) & HALF_MASK))
                    pending_high.remove((high_position, high_symbol))
                write_word(image, position, (word & ~HALF_MASK) | ((value + low) & HALF_MASK))
            elif kind == R_MIPS_GPREL16:
                addend = signed16(word & HALF_MASK) + (gp0 if symbol.bind == STB_LOCAL else 0)
                offset_from_gp = value + addend - retail.gp
                if not -HALF_SIGN <= offset_from_gp < HALF_SIGN:
                    sys.exit(f"{source}: {symbol.name or 'a local symbol'} is read through $gp but has no retail address "
                             f"(give it its retail name)")
                write_word(image, position, (word & ~HALF_MASK) | (offset_from_gp & HALF_MASK))
            else:
                sys.exit(f"{source}: relocation type {kind} isn't handled")
        if pending_high:
            sys.exit(f"{source}: a HI16 relocation in {elf.sections[target].name} has no LO16")
    return Candidate(bytes(image), bases[entry_symbol.shndx] + entry_symbol.value, entry_symbol.size)


def align(value, alignment):
    return (value + alignment - 1) // alignment * alignment


def closing_parenthesis(text, start):
    """The index of the parenthesis closing the one before start"""
    depth = 1
    for index in range(start, len(text)):
        if text[index] == "(":
            depth += 1
        elif text[index] == ")":
            depth -= 1
            if depth == 0:
                return index
        else:
            continue
    return len(text)


def top_level_split(text):
    """A parameter or argument list split at its top-level commas"""
    depth = 0
    parts = [""]
    for character in text:
        if character == "(":
            depth += 1
        elif character == ")":
            depth -= 1
        elif character == "," and depth == 0:
            parts.append("")
            continue
        else:
            pass
        parts[-1] += character
    return parts


@dataclass
class Declarations:
    """What the C says of the function and its callees"""
    # {retail address of a callee: the arguments compared at a call to it}
    callee_arguments: dict
    compare_v0: bool
    compare_v1: bool
    # Whether each of the function's parameters is a pointer
    pointer_parameters: tuple


def declarations(source, function, retail):
    """Declarations, from the preprocessed source. A callee's arguments are those of its prototype, or its calls in the
    file; an unknown callee's are $a0-$a3"""
    # TRADEOFF: one word per parameter (no 64-bit or structure arguments), and a variadic callee's named parameters
    # only; a C parser would do better if such callees turn up
    with tempfile.TemporaryDirectory() as folder:
        preprocessed = Path(folder) / "source.i"
        result = subprocess.run(cpp_command(compiler(DEFAULT_GCC), source, preprocessed, (NONMATCHING_MACRO,)),
                                capture_output=True, text=True, cwd=HERE)
        if result.returncode != 0:
            sys.exit(f"{source}: {result.stderr.strip()}")
        text = preprocessed.read_text()
    counts = {}
    variadic = {}
    pointer_parameters = None
    for match in NAME_BEFORE_PARENTHESIS.finditer(text):
        name = match.group(1)
        if name not in retail.addresses:
            continue
        parameters = text[match.end():closing_parenthesis(text, match.end())].strip()
        if not parameters:
            # A call without arguments or an old-style declaration: says nothing
            continue
        elif parameters == "void":
            counts[name] = max(counts.get(name, 0), 0)
        elif VARIADIC in parameters:
            variadic[name] = len(top_level_split(parameters)) - 1
        else:
            parts = top_level_split(parameters)
            counts[name] = max(counts.get(name, 0), len(parts))
            if name == function and pointer_parameters is None:
                pointer_parameters = tuple("*" in part for part in parts)
    counts.update(variadic)
    returns_void = re.search(rf"\bvoid\s+{function}\s*\(", text) is not None
    returns_long = re.search(rf"{LONG_RETURN}{function}\s*\(", text) is not None
    return Declarations({retail.addresses[name]: count for name, count in counts.items()}, not returns_void, returns_long,
                        pointer_parameters or ())


class Fuzz:
    """A trial's random values, each drawn from a generator seeded by the trial and what it is for"""

    def __init__(self, seed, trial, constants, pointer_parameters):
        self.key = f"{seed}:{trial}"
        self.constants = constants
        self.pointer_parameters = pointer_parameters

    def generator(self, *purpose):
        return random.Random(":".join([self.key, *(str(part) for part in purpose)]))

    def words(self, generator, count):
        return draw(generator, count, self.constants)

    def arguments(self):
        """The function's arguments, in parameter order: a pointer parameter gets a pointer into the arena, or now and
        then NULL; the others any random word"""
        generator = self.generator("arguments")
        values = []
        for index, word in enumerate(self.words(generator, ARGUMENT_WORDS)):
            if index < len(self.pointer_parameters) and self.pointer_parameters[index]:
                null = generator.randrange(NULL_POINTER_ODDS) == 0
                values.append(0 if null else ARENA_BASE + generator.randrange(ARENA_SIZE // WORD) * WORD)
            else:
                values.append(word)
        return values


def draw(generator, count, constants):
    """count random words, the constants among them"""
    values = []
    for kind in generator.choices(FUZZ_KINDS, FUZZ_WEIGHTS, k=count):
        if kind == FUZZ_ZERO:
            values.append(0)
        elif kind == FUZZ_SMALL:
            values.append(generator.randint(1, generator.choice(SMALL_LIMITS)))
        elif kind == FUZZ_NEGATIVE:
            values.append(-generator.randint(1, generator.choice(SMALL_LIMITS)) & WORD_MASK)
        elif kind == FUZZ_POINTER:
            values.append(ARENA_BASE + generator.randrange(ARENA_SIZE // WORD) * WORD)
        elif kind == FUZZ_ANY:
            values.append(generator.getrandbits(WORD_BITS))
        elif kind == FUZZ_CONSTANT:
            values.append(generator.choice(constants) if constants else 0)
        else:
            raise ValueError(f"fuzz kind {kind}")
    return values


def fuzz_pool(seed, constants):
    """The random words pages are sliced from"""
    return struct.pack(f"<{POOL_WORDS}I", *draw(random.Random(f"{seed}:pool"), POOL_WORDS, constants))


def immediates(code):
    """The constants a function's code holds in its instructions' immediates, each with its neighbours and negation"""
    found = set()
    for offset in range(0, len(code), WORD):
        word = read_word(code, offset)
        opcode = word >> OPCODE_SHIFT
        if opcode in SIGNED_IMMEDIATES:
            value = signed16(word & HALF_MASK)
        elif opcode in UNSIGNED_IMMEDIATES:
            value = word & HALF_MASK
        elif opcode == UPPER_IMMEDIATE:
            value = (word & HALF_MASK) << HALF_BITS
        else:
            continue
        found.update(neighbour & WORD_MASK for neighbour in (value, value - 1, value + 1, -value))
    return sorted(found)


class Memory:
    """A trial's memory before either run: the regions both runs map, {physical start: content}. RAM and the scratchpad
    hold random words, RAM with the retail code and read-only data over them; the candidate's sections and the page of
    the return sentinel have regions of their own"""

    def __init__(self, fuzz, pool, retail, candidate):
        generator = fuzz.generator("pages")
        # RAM's pages, then the scratchpad's
        pages = RAM_SIZE // PAGE_SIZE + 1
        offsets = [generator.randrange(POOL_WORDS - PAGE_SIZE // WORD) * WORD for _ in range(pages)]
        ram = bytearray(b"".join(pool[offset:offset + PAGE_SIZE] for offset in offsets[:-1]))
        kept = physical(retail.kept_start) - physical(RAM_BASE)
        ram[kept:kept + len(retail.kept)] = retail.kept
        # Zeros below the entry $sp: whatever a frame leaves uninitialised reads alike however it is laid out
        stack = physical(ENTRY_SP) - physical(RAM_BASE)
        ram[stack - STACK_RESERVE:stack] = bytes(STACK_RESERVE)
        stack_arguments = fuzz.arguments()[len(ARGUMENT_REGISTERS):]
        ram[stack + ARGUMENT_HOME_SIZE:stack + ARGUMENT_HOME_SIZE + len(stack_arguments) * WORD] = struct.pack(
            f"<{len(stack_arguments)}I", *stack_arguments)
        data = kept + len(retail.kept)
        retail_words = generator.getrandbits(len(retail.data) // WORD)
        for index in range(0, len(retail.data), WORD):
            if retail_words >> (index // WORD) & 1:
                ram[data + index:data + index + WORD] = retail.data[index:index + WORD]
            else:
                continue
        self.regions = {
            physical(RAM_BASE): bytes(ram),
            SCRATCHPAD_BASE: pool[offsets[-1]:offsets[-1] + PAGE_SIZE],
            physical(SCRATCH_BASE): candidate.image + bytes(align(len(candidate.image), PAGE_SIZE) - len(candidate.image)),
            physical(SENTINEL_RETURN): bytes(PAGE_SIZE),
        }

    def word(self, address):
        """The initial word at a physical address"""
        for start, content in self.regions.items():
            if start <= address < start + len(content):
                return read_word(content, address - start)
            else:
                continue
        raise ValueError(f"{address:#x} isn't in a mapped region")


@dataclass(frozen=True)
class FrameWords:
    """A call's argument that points into the caller's frame, by the words there"""
    words: tuple

    def __format__(self, spec):
        return f"a frame address holding {' '.join(f'{word:#010x}' for word in self.words)}"


@dataclass
class Call:
    target: int
    arguments: tuple
    stack: tuple


@dataclass
class Run:
    status: str = None
    unsupported: str = None
    registers: dict = field(default_factory=dict)
    calls: list = field(default_factory=list)
    regions: dict = field(default_factory=dict)


def entry_registers(fuzz):
    """{Unicorn register: value} of the registers set at random on entry, the arguments among them"""
    registers = [REGISTERS[name] for name in RANDOM_ON_ENTRY] + list(MULTIPLY_REGISTERS)
    values = dict(zip(registers, fuzz.words(fuzz.generator("registers"), len(registers))))
    values.update(zip(ARGUMENT_REGISTERS, fuzz.arguments()))
    return values


def run(memory, fuzz, retail, entry, size, arguments):
    """Runs the code at entry (size bytes) on the trial's memory and registers: how it ended, its registers, its calls out
    and its final memory"""
    outcome = Run()
    emulator = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
    for start, content in memory.regions.items():
        emulator.mem_map(start, len(content))
        emulator.mem_write(start, content)

    def unmapped(uc, access, address, size, value, data):
        # Ends the run: mapping on demand isn't safe (PAGE_SIZE's comment)
        outcome.status = f"access to unmapped {virtual(address):#010x}"
        return False

    # The values calls return, drawn in call order: the same for the same calls on both sides
    returned = fuzz.generator("calls")

    def call_out(uc, address, size, data):
        if address == SENTINEL_RETURN:
            outcome.status = RETURNED
            uc.emu_stop()
            return
        if len(outcome.calls) == MAX_CALLS:
            outcome.status = LIMIT
            uc.emu_stop()
            return
        # TRADEOFF: the callee isn't run, so its side effects (memory it writes, what it returns for real) are missing;
        # both sides get the same made-up return value, and C relying on what the callee does to memory isn't checked
        count = arguments.get(address, len(ARGUMENT_REGISTERS))
        sp = uc.reg_read(REGISTERS["sp"])

        def argument(value):
            # An address in the caller's frame stands for the words it points to: frames may be laid out differently,
            # and a local passed by reference is compared by its value
            # TRADEOFF: FRAME_WORDS words whatever the local's size; past a smaller local they may be other locals,
            # which a frame laid out otherwise holds elsewhere. Unset words read zero alike (Memory)
            if sp <= value < ENTRY_SP:
                start = value - value % WORD
                end = min(start + FRAME_WORDS * WORD, ENTRY_SP)
                return FrameWords(tuple(read_word(uc.mem_read(physical(start), end - start), offset)
                                        for offset in range(0, end - start, WORD)))
            else:
                return value

        registers = tuple(argument(uc.reg_read(register)) for register in ARGUMENT_REGISTERS[:count])
        stack = tuple(argument(read_word(uc.mem_read(physical(sp + ARGUMENT_HOME_SIZE + index * WORD), WORD), 0))
                      for index in range(max(0, count - len(ARGUMENT_REGISTERS))))
        outcome.calls.append(Call(address, registers, stack))
        values = fuzz.words(returned, len(CLOBBERED_BY_CALLS))
        for name, value in zip(CLOBBERED_BY_CALLS, values):
            uc.reg_write(REGISTERS[name], value)
        uc.reg_write(mips.UC_MIPS_REG_PC, uc.reg_read(REGISTERS["ra"]))

    def exception(uc, number, data):
        if number in UNSUPPORTED_EXCEPTIONS:
            outcome.unsupported = UNSUPPORTED_EXCEPTIONS[number]
            outcome.status = outcome.unsupported
        elif number in EXCEPTIONS:
            outcome.status = EXCEPTIONS[number]
        else:
            outcome.status = f"exception {number}"
        uc.emu_stop()

    emulator.hook_add(UC_HOOK_MEM_UNMAPPED, unmapped)
    emulator.hook_add(UC_HOOK_CODE, call_out, begin=0, end=entry - 1)
    emulator.hook_add(UC_HOOK_CODE, call_out, begin=entry + size, end=WORD_MASK)
    emulator.hook_add(UC_HOOK_INTR, exception)

    for register, value in entry_registers(fuzz).items():
        emulator.reg_write(register, value)
    emulator.reg_write(REGISTERS["gp"], retail.gp)
    emulator.reg_write(REGISTERS["sp"], ENTRY_SP)
    emulator.reg_write(REGISTERS["ra"], SENTINEL_RETURN)
    try:
        emulator.emu_start(entry, SENTINEL_RETURN, count=MAX_INSTRUCTIONS)
    except UcError as error:
        if outcome.status is None:
            outcome.status = f"emulator error: {error}"
    if outcome.status is None:
        outcome.status = RETURNED if emulator.reg_read(mips.UC_MIPS_REG_PC) == SENTINEL_RETURN else LIMIT
    outcome.registers = {name: emulator.reg_read(REGISTERS[name]) for name in REGISTER_NAMES}
    outcome.regions = {start: bytes(emulator.mem_read(start, len(content))) for start, content in memory.regions.items()}
    return outcome


def gte_instructions(code, address):
    """'mnemonic at address' of the GTE instructions in a function's code"""
    found = []
    for offset in range(0, len(code), WORD):
        opcode = read_word(code, offset) >> OPCODE_SHIFT
        if opcode in GTE_OPCODES:
            found.append(f"{GTE_OPCODES[opcode]} at {address + offset:#010x}")
        else:
            continue
    return found


def changed(memory, outcome):
    """The physical addresses of the words a run changed"""
    words = set()
    for start, initial in memory.regions.items():
        final = outcome.regions[start]
        if final == initial:
            continue
        for page in range(0, len(initial), PAGE_SIZE):
            if final[page:page + PAGE_SIZE] == initial[page:page + PAGE_SIZE]:
                continue
            words.update(start + offset for offset in range(page, page + PAGE_SIZE, WORD)
                         if final[offset:offset + WORD] != initial[offset:offset + WORD])
    return words


def compare(memory, retail, declared, expected, actual):
    """What differs between the retail run (expected) and the candidate's (actual)"""
    differences = []
    if expected.status != actual.status:
        differences.append(f"the retail code ended by {expected.status}, the candidate by {actual.status}")
    elif expected.status == LIMIT:
        # Both still running: their states mid-way don't compare
        return differences
    else:
        # Both returned or both took the same exception: compared below
        pass

    for index, (wanted, got) in enumerate(zip(expected.calls, actual.calls)):
        if wanted.target != got.target:
            differences.append(f"call {index}: the retail code calls {retail.describe(wanted.target)}, the candidate "
                               f"{retail.describe(got.target)}")
            break
        for position, (want, have) in enumerate(zip(wanted.arguments, got.arguments)):
            if want != have:
                differences.append(f"call {index} to {retail.describe(wanted.target)}: $a{position} is {want:#010x} "
                                   f"in the retail code, {have:#010x} in the candidate")
        for position, (want, have) in enumerate(zip(wanted.stack, got.stack)):
            if want != have:
                differences.append(f"call {index} to {retail.describe(wanted.target)}: stack argument {position} is "
                                   f"{want:#010x} in the retail code, {have:#010x} in the candidate")
    if len(expected.calls) != len(actual.calls):
        differences.append(f"the retail code makes {len(expected.calls)} calls, the candidate {len(actual.calls)}")
    if expected.status != RETURNED or actual.status != RETURNED:
        # An exception stops either side at a point whose registers and memory don't compare
        return differences

    compared = list(CALLEE_SAVED)
    if declared.compare_v0:
        compared.append("v0")
    if declared.compare_v1:
        compared.append("v1")
    for name in compared:
        if expected.registers[name] != actual.registers[name]:
            differences.append(f"${name} is {expected.registers[name]:#010x} after the retail code, "
                               f"{actual.registers[name]:#010x} after the candidate")

    frame = range(physical(ENTRY_SP - STACK_RESERVE), physical(ENTRY_SP + ARGUMENT_HOME_SIZE))
    for word in sorted(word for word in changed(memory, expected) | changed(memory, actual) if word not in frame):
        want, have = (next(read_word(content, word - start) for start, content in outcome.regions.items()
                           if start <= word < start + len(content)) for outcome in (expected, actual))
        if want != have:
            differences.append(f"memory at {retail.describe(virtual(word))} is {want:#010x} after the retail code, "
                               f"{have:#010x} after the candidate")
    return differences


def describe_inputs(fuzz):
    """The trial's arguments: $a0-$a3 and the words on the caller's stack"""
    values = fuzz.arguments()
    registers, stack = values[:len(ARGUMENT_REGISTERS)], values[len(ARGUMENT_REGISTERS):]
    return (" ".join(f"$a{index}={value:#010x}" for index, value in enumerate(registers))
            + ", stack " + " ".join(f"{word:#010x}" for word in stack))


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("source")
    parser.add_argument("function")
    parser.add_argument("--trials", type=int, default=DEFAULT_TRIALS)
    parser.add_argument("--seed", type=int, default=0)
    arguments = parser.parse_args()
    source = Path(arguments.source).resolve()
    if not source.exists():
        sys.exit(f"{source} is missing")
    if arguments.trials < 1:
        sys.exit("--trials takes a positive number")

    retail = Retail()
    retail_entry, retail_size = retail.function(arguments.function)
    candidate = compile_candidate(source, arguments.function, retail)
    declared = declarations(source, arguments.function, retail)
    retail_code = retail.kept[retail_entry - retail.kept_start:retail_entry - retail.kept_start + retail_size]
    candidate_code = candidate.image[candidate.entry - SCRATCH_BASE:candidate.entry - SCRATCH_BASE + candidate.size]

    agreeing = 0
    both_limit = 0
    both_exception = 0
    first = None
    constants = immediates(retail_code)
    pool = fuzz_pool(arguments.seed, constants)
    for trial in range(arguments.trials):
        fuzz = Fuzz(arguments.seed, trial, constants, declared.pointer_parameters)
        memory = Memory(fuzz, pool, retail, candidate)
        expected = run(memory, fuzz, retail, retail_entry, retail_size, declared.callee_arguments)
        actual = run(memory, fuzz, retail, candidate.entry, candidate.size, declared.callee_arguments)
        for side, outcome, code, address in (("retail code", expected, retail_code, retail_entry),
                                             ("candidate", actual, candidate_code, candidate.entry)):
            if outcome.unsupported is not None:
                found = gte_instructions(code, address)
                more = f" and {len(found) - GTE_SHOWN} more" if len(found) > GTE_SHOWN else ""
                detail = f"GTE instructions ({', '.join(found[:GTE_SHOWN])}{more})" if found else "an instruction Unicorn lacks"
                print(f"unsupported: trial {trial}: the {side} raised {outcome.unsupported}; it has {detail}")
                sys.exit(EXIT_UNSUPPORTED)
        differences = compare(memory, retail, declared, expected, actual)
        if differences:
            if first is None:
                first = (trial, describe_inputs(fuzz), differences)
        else:
            agreeing += 1
            if expected.status == LIMIT:
                both_limit += 1
            elif expected.status != RETURNED:
                both_exception += 1
            else:
                # Both returned
                pass

    if first is not None:
        trial, inputs, differences = first
        print(f"trial {trial} disagrees: {inputs}")
        for difference in differences[:MAX_DIFFERENCES_SHOWN]:
            print(f"  {difference}")
        if len(differences) > MAX_DIFFERENCES_SHOWN:
            print(f"  ... {len(differences) - MAX_DIFFERENCES_SHOWN} more")
    if both_limit:
        print(f"{both_limit} trials agree by both reaching the limit of {MAX_INSTRUCTIONS} instructions or {MAX_CALLS} calls "
              f"(not compared)")
    if both_exception:
        print(f"{both_exception} trials agree by both taking the same exception (calls compared, not memory)")
    print(f"{agreeing} of {arguments.trials} trials agree")
    if agreeing != arguments.trials:
        sys.exit(EXIT_DISAGREE)


if __name__ == "__main__":
    main()
