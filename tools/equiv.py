#!/usr/bin/env python3
"""Runs a function's C and its retail code side by side in an emulator (Unicorn), from the same random memory and
registers, and compares what each does: the judge of the equivalent track (docs/WORKFLOW.md). C under NON_MATCHING is
called equivalent only when every trial agrees; random trials are evidence, not a proof.

    python tools/equiv.py FILE.c FUNCTION [--trials 500] [--seed 0]

FILE.c is compiled as the build compiles it, with NON_MATCHING defined, and FUNCTION's code relocated into a scratch area:
every name the retail executable knows resolves to its retail address (the file's own .sdata and .sbss variables
included), what has no retail name (jump tables, labels, literals) to the scratch copies. Calls out of the function are
recorded and answered, not run. The GTE's instructions run on a model of it (tools/gte.py), its registers random on
entry and compared after. A trial whose retail run faults on a memory access through a word of its inputs gets
that word replaced and starts over (MAX_REPAIRS), before the candidate runs from the same repaired state. Compared
after each trial: how it ended, the return value, the callee-saved registers, each call's target and arguments, and
every byte either side changed outside its own stack frame.

The C file may state the function's contract with its callers (include/common.h): EQUIV_RANGE(function, argument, low,
high), EQUIV_NONNULL(function, argument), EQUIV_DISJOINT(function, first, second, count, unit), EQUIV_SCRATCH(function,
address, bytes), EQUIV_RUNS(function, routine, ...) and EQUIV_CALLBACK(function, argument, count, register...). The
trials then keep to it, and the verdict says so; each line goes in by hand with the evidence that every call keeps to
it (docs/WORKFLOW.md, "A function's contract").
HANDWRITTEN_PARTS(function, part, ...) and EQUIV_RUNS make the retail code the function runs as its own reach to those
routines (tools/handwritten.py).

Exit status: 0 when every trial agrees, 1 on a disagreement or an error, 2 when the code needs what the emulator lacks
(a GTE command the model doesn't have). Needs the build (build/SCUS_941.54.elf) and orig/USA/SCUS_941.54.
"""
import argparse
import bisect
import copy
import ctypes
import random
import re
import struct
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from unicorn import UC_ARCH_MIPS, UC_HOOK_CODE, UC_HOOK_INTR, UC_HOOK_MEM_UNMAPPED, UC_HOOK_MEM_WRITE_PROT
from unicorn import UC_MODE_LITTLE_ENDIAN, UC_MODE_MIPS32, UC_PROT_ALL, UC_PROT_EXEC, UC_PROT_READ, Uc, UcError
from unicorn import mips_const as mips

import gte
import handwritten
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
# others. In one trial in RETAIL_DATA_ODDS every word keeps its value, for a table of several words read whole
# (func_8002FFFC's vertex indices: one trial in 64 kept all six). Its .bss is random like the rest of RAM
RETAIL_DATA_ODDS = 4
GP_SYMBOL = "_gp"
LINKER_MARKER = re.compile(r"^(?:main|header)_|^_gp$")
KEPT_START_SYMBOL = "main_VRAM"
KEPT_END_SYMBOL = "main_DATA_START"
KEPT_BIT = "1"
# memoryview's format for a word, copied as it is (no byte order involved)
WORD_FORMAT = "I"

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
STT_FUNC = 2
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
# The GTE's instructions (coprocessor 2), which Unicorn's MIPS32 doesn't have: each runs on tools/gte.py's model from a
# code hook, which then skips it. TRADEOFF: mfc2's and cfc2's result is there at once, where the hardware gives the
# instruction after them the old value; the code the build writes waits a slot (include/gte.h), retail's handwritten too
GTE_OPCODES = {0x12: "cop2", 0x32: "lwc2", 0x3A: "swc2"}
OP_LWC2 = 0x32
OP_SWC2 = 0x3A
# cop2 with this bit is a command; without, rs picks a move: mfc2, cfc2, mtc2, ctc2
COP2_COMMAND = 1 << 25
COP2_MFC2, COP2_CFC2, COP2_MTC2, COP2_CTC2 = 0x00, 0x02, 0x04, 0x06
# What has a delay slot, which a hook skipping its instruction can't run: the branches and jumps by opcode (REGIMM, j,
# jal, beq, bne, blez, bgtz), SPECIAL's jr and jalr, a coprocessor's branch (rs 8)
BRANCH_OPCODES = range(0x01, 0x08)
JUMP_REGISTER_FUNCTS = {0x08, 0x09}
COPROCESSOR_OPCODES = range(0x10, 0x14)
COPROCESSOR_BRANCH = 0x08
# The GTE's registers on entry: random words, and in about half the trials a rotation, translation, screen offset and
# projection plane distance near the game's (RT entries within +-1.0 in 4.12, the object in front of the camera), so
# that RTPS/RTPT also project without flagging an error
GTE_PLAUSIBLE_ODDS = 0.5
GTE_ROTATION_LIMIT = 0x1000
GTE_TRANSLATION_XY_LIMIT = 0x400
GTE_TRANSLATION_Z_RANGE = (0x100, 0x4000)
GTE_SCREEN_OFFSET_LIMIT = 0x200
GTE_PROJECTION_RANGE = (0x80, 0x400)
# The rotation matrix's nine halves, in five words: the last word's upper half is unused
GTE_ROTATION_HALVES = 9
GTE_ROTATION_WORDS = 5

# Address space. Unicorn maps kseg0 and kseg1 to physical memory as the PS1 does (address & 0x1FFFFFFF) and kuseg to
# itself; its memory calls and hooks take physical addresses, its code hooks virtual ones. kseg2 needs a TLB and faults.
# Every region is mapped and filled before a run, none on demand: Unicorn 2.1.4 resumes a store in a taken branch's delay
# slot without the branch once a memory hook has run on it (a reserved-instruction exception follows). The retail
# code and read-only data are read-only: a store through a wild pointer would otherwise rewrite the retail function as
# it runs (func_80015344's table index went negative into its own code) and not the candidate's copy
PAGE_SIZE = 0x1000
CHUNK_SIZE = 0x10000
KSEG0 = 0x80000000
KSEG2 = 0xC0000000
PHYSICAL_MASK = 0x1FFFFFFF
# The PS1's 2 MB of RAM and its 1 KB data cache used as fast RAM (one page mapped)
RAM_BASE = 0x80000000
RAM_SIZE = 0x200000
SCRATCHPAD_BASE = 0x1F800000
# The regions whose words a repair may replace (Memory.repairable)
REPAIRABLE_REGIONS = (RAM_BASE & PHYSICAL_MASK, SCRATCHPAD_BASE)
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

# Repairs. A retail run ending on a memory access fault whose address comes from a word of the trial's inputs (a word
# of memory read as it was on entry, $a0-$a3, a register a callee returned) is run again with that word replaced: a
# pointer into the arena where the word is the address's base, a small aligned offset where it is added to an address
# already in memory, a small count where it is shifted or multiplied first; a count or an offset still faulting is set
# to zero once. Both sides then run from the repaired state. Random words make most dereferenced words bad pointers:
# two global pointers read in a row are both arena pointers in one trial in sixteen
MAX_REPAIRS = 16
# Instructions a traced run follows to find the faulting access's word: a fault later than this isn't repaired
# TRADEOFF: a Python hook on every instruction (about 1.3 us each), from the start at each repair; tracing natively or
# resuming from a snapshot if late faults matter
TRACE_LIMIT = 20_000
REPAIR_OFFSET_LIMIT = 0x100
REPAIR_COUNT_LIMIT = 8
# What a word is to the faulting address
ROLE_BASE, ROLE_OFFSET, ROLE_COUNT = "base", "offset", "count"
# Instruction fields
RS_SHIFT = 21
RT_SHIFT = 16
RD_SHIFT = 11
REGISTER_FIELD = 0x1F
FUNCT_MASK = 0x3F
OP_SPECIAL = 0x00
OP_REGIMM = 0x01
OP_JAL = 0x03
OP_ADDI = 0x08
OP_ADDIU = 0x09
OP_COP0 = 0x10
OP_COP2 = 0x12
OP_LW = 0x23
LOAD_OPCODES = range(0x20, 0x27)
STORE_OPCODES = range(0x28, 0x2F)
# Immediate arithmetic and logic but addi/addiu, and lui: the result isn't the input word any more
IMMEDIATE_ALU_OPCODES = range(0x0A, 0x10)
# REGIMM's bltzal and bgezal write $ra
REGIMM_LINKS = {0x10, 0x11}
# mfc and cfc write rt
COPROCESSOR_MOVES_TO_GPR = {0x00, 0x02}
FUNCT_ADD = 0x20
FUNCT_ADDU = 0x21
FUNCT_OR = 0x25
# sub and subu, followed as additions
FUNCT_SUBTRACTS = {0x22, 0x23}
# SPECIAL: shifts by an immediate or a register (of rt); mult, multu, div and divu; mfhi and mflo; mthi and mtlo
FUNCT_SHIFTS = {0x00, 0x02, 0x03, 0x04, 0x06, 0x07}
FUNCT_MULTIPLIES = {0x18, 0x19, 0x1A, 0x1B}
FUNCT_FROM_MULTIPLY = {0x10, 0x12}
FUNCT_TO_MULTIPLY = {0x11, 0x13}
# SPECIAL instructions writing no general register, $hi or $lo: jr, syscall, break
FUNCT_NO_DESTINATION = {0x08, 0x0C, 0x0D}
RA_NUMBER = 31
# SPECIAL's jalr, which links into rd ($ra as the game writes it)
FUNCT_JALR = 0x09
# An address's low bit, which a jump through a register takes for the MIPS16 mode
MIPS16_BIT = 1
# What an instruction does to the registers' origins (Tracer): nothing, drop its destination's, copy its source's, load
# a word (the word's own if as it was on entry), add, shift, multiply, move from or to $hi/$lo
(EFFECT_NONE, EFFECT_DROP, EFFECT_MOVE, EFFECT_LW, EFFECT_SUM, EFFECT_SCALE, EFFECT_MULTIPLY, EFFECT_FROM_MULTIPLY,
 EFFECT_TO_MULTIPLY) = range(9)
WRITING_EFFECTS = {EFFECT_DROP, EFFECT_MOVE, EFFECT_LW, EFFECT_SUM, EFFECT_SCALE, EFFECT_FROM_MULTIPLY}

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
# Unicorn's registers by number, $zero first
REGISTER_NUMBERS = [mips.UC_MIPS_REG_ZERO] + [REGISTERS[name] for name in REGISTER_NAMES]
# Set at random on entry: all but $zero, $gp, $sp, $ra
RANDOM_ON_ENTRY = [name for name in REGISTER_NAMES if name not in ("gp", "sp", "ra")]
# What a call may change: set at random after each call out
CLOBBERED_BY_CALLS = ("at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9")
CALLEE_SAVED = ("s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "fp", "gp", "sp")
MULTIPLY_REGISTERS = (mips.UC_MIPS_REG_HI, mips.UC_MIPS_REG_LO)

# What a callee can return, where a made-up value past it takes the caller down a path the game never runs: {callee:
# (lowest, highest)} of its $v0, each bound read from the callee's code. TRADEOFF: a caller's code for values its callee
# can't return isn't checked; the bound is only as right as its reading (docs/WORKFLOW.md)
CALLEE_RETURN_RANGES = {
    # One segment at most for each of the 6 quads it clips, or 1 when it returns early
    "func_8002FFFC": (0, 6),
}

# How a run ended
RETURNED = "returned"
LIMIT = "the limit"
# Unicorn's exception numbers (QEMU's), as UC_HOOK_INTR reports them
EXCEPTIONS = {12: "address error on load", 13: "address error on store", 17: "syscall", 18: "break", 21: "overflow",
              22: "trap", 26: "TLB miss on load", 27: "TLB miss on store"}
EXCEPTION_ADDRESS_LOAD, EXCEPTION_ADDRESS_STORE, EXCEPTION_TLB_LOAD, EXCEPTION_TLB_STORE = 12, 13, 26, 27
UNSUPPORTED_EXCEPTIONS = {19: "coprocessor unusable", 20: "reserved instruction"}
# Followed by the address, in a run's ending
UNMAPPED = "access to unmapped"
STORE_TO_CODE = "store to the retail code or read-only data at"
# The endings a repair may lift: address errors and TLB misses, then the two above
ACCESS_EXCEPTIONS = {EXCEPTIONS[EXCEPTION_ADDRESS_LOAD], EXCEPTIONS[EXCEPTION_ADDRESS_STORE],
                     EXCEPTIONS[EXCEPTION_TLB_LOAD], EXCEPTIONS[EXCEPTION_TLB_STORE]}
ACCESS_PREFIXES = (UNMAPPED, STORE_TO_CODE)

# A name followed by an argument list, in the preprocessed C: a prototype, a definition or a call
NAME_BEFORE_PARENTHESIS = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
VARIADIC = "..."
LONG_RETURN = r"\b(?:long\s+long|s64|u64)\s+"
# A function's contract with its callers in its C file (include/common.h): EQUIV_RANGE(function, argument, low, high),
# its bounds numbers or names the file #defines as numbers, EQUIV_NONNULL(function, argument),
# EQUIV_DISJOINT(function, first, second, count, unit), EQUIV_SCRATCH(function, address, bytes),
# EQUIV_RUNS(function, routine, ...) and EQUIV_CALLBACK(function, argument, count)
NUMBER = r"-?(?:0x[0-9A-Fa-f]+|\d+)"
BOUND = rf"{NUMBER}|[A-Za-z_]\w*"
CONTRACT_LINE = re.compile(r"^EQUIV_\w*\(.*$", re.MULTILINE)
ARGUMENT_RANGE = re.compile(rf"^EQUIV_RANGE\((\w+),\s*(\d+),\s*({BOUND}),\s*({BOUND})\)$")
ARGUMENT_NONNULL = re.compile(r"^EQUIV_NONNULL\((\w+),\s*(\d+)\)$")
ARGUMENTS_DISJOINT = re.compile(r"^EQUIV_DISJOINT\((\w+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\)$")
SCRATCH_AREA = re.compile(rf"^EQUIV_SCRATCH\((\w+),\s*({BOUND}),\s*({BOUND})\)$")
RUNS_INTO = re.compile(r"^EQUIV_RUNS\((\w+)((?:,\s*\w+)+)\)$")
CALLBACK_ARGUMENT = re.compile(r"^EQUIV_CALLBACK\((\w+),\s*(\d+),\s*(\d+)((?:,\s*\w+)*)\)$")
NUMERIC_DEFINE = re.compile(rf"^#define\s+(\w+)\s+\(?({NUMBER})\)?\s*$", re.MULTILINE)
CONTRACT = "the C's contract:"
SIGNED_MIN = -(1 << (WORD_BITS - 1))
SIGNED_MAX = (1 << (WORD_BITS - 1)) - 1


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


def shown(address):
    """A physical address as the C writes it: the scratchpad's as it is, RAM's in kseg0"""
    if address >= SCRATCHPAD_BASE:
        return address
    else:
        return virtual(address)


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
        if len(self.data) % WORD or self.kept_end % WORD:
            sys.exit(f"{RETAIL_EXE}'s data from {KEPT_END_SYMBOL} isn't whole words")

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
    """The C's function relocated into the scratch area, with the file's static functions it may call: its own code runs
    from start for size bytes, entered at entry"""
    image: bytes
    entry: int
    start: int
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
    # The file's static functions in the same section: a helper the C calls stands for code the retail function has
    # inline, so it runs as the candidate's own code, not as a call out
    own = [entry_symbol] + [symbol for symbol in elf.symbols if symbol.bind == STB_LOCAL and symbol.type == STT_FUNC
                            and symbol.shndx == entry_symbol.shndx and symbol.size > 0]
    own_start = min(symbol.value for symbol in own)
    own_end = max(symbol.value + symbol.size for symbol in own)

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
            # Of the code, only the function's and its static helpers': the file's other functions are reached at their
            # retail addresses
            relocations = [relocation for relocation in relocations
                           if any(symbol.value <= relocation[0] < symbol.value + symbol.size for symbol in own)]
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
    base = bases[entry_symbol.shndx]
    return Candidate(bytes(image), base + entry_symbol.value, base + own_start, own_end - own_start)


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
    # Each parameter's narrow integer type, (bits, signed), or None (narrow_type)
    narrow_parameters: tuple
    # The C's contract with its callers (contract): {argument's index: (low, high)}, the pointer arguments never NULL,
    # (first, second, count, unit) of the buffers that never overlap, the physical ranges of its own scratch, and the
    # handwritten routines its retail code runs into, {argument's index: (the arguments a callback it holds takes, the
    # registers a call to it leaves)}
    argument_ranges: dict
    nonnull_arguments: frozenset
    disjoint_arguments: tuple
    scratch: tuple
    runs_into: frozenset
    callbacks: dict


# A parameter's integer types narrower than a word, by the words that name them: (bits, signed). Plain char is left out,
# its signedness being the compiler's
NARROW_TYPES = {("u8",): (8, False), ("s8",): (8, True), ("unsigned", "char"): (8, False), ("signed", "char"): (8, True),
                ("u16",): (16, False), ("s16",): (16, True), ("unsigned", "short"): (16, False),
                ("signed", "short"): (16, True), ("short",): (16, True)}


def narrow_type(parameter):
    """(bits, signed) of a parameter declared with an integer type narrower than a word, else None"""
    if "*" in parameter or "[" in parameter:
        return None
    else:
        words = tuple(word for word in re.findall(r"\w+", parameter) if word not in ("const", "volatile", "int"))
        # The type's words, with or without the parameter's name after them
        return NARROW_TYPES.get(words) or NARROW_TYPES.get(words[:-1])


def narrowed(value, narrow):
    """A word as a parameter of that narrow type holds it, extended to a word as the caller passes it"""
    if narrow is None:
        return value
    else:
        bits, signed = narrow
        value &= (1 << bits) - 1
        if signed and value >> (bits - 1):
            return (value - (1 << bits)) & WORD_MASK
        else:
            return value


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
    narrow_parameters = ()
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
                narrow_parameters = tuple(narrow_type(part) for part in parts)
    counts.update(variadic)
    returns_void = re.search(rf"\bvoid\s+{function}\s*\(", text) is not None
    returns_long = re.search(rf"{LONG_RETURN}{function}\s*\(", text) is not None
    ranges, nonnull, disjoint, scratch, runs, callbacks = contract(source, function)
    pointers = pointer_parameters or ()
    named_pointers = nonnull.union(*((first, second) for first, second, _, _ in disjoint))
    not_pointers = sorted(index for index in named_pointers if index >= len(pointers) or not pointers[index])
    if not_pointers:
        sys.exit(f"{source}: the contract of {function} takes its argument {not_pointers[0]} for a pointer, which its C "
                 "doesn't declare one")
    return Declarations({retail.addresses[name]: count for name, count in counts.items()}, not returns_void, returns_long,
                        pointers, narrow_parameters, ranges, nonnull, disjoint, scratch, runs, callbacks)


def contract(source, function):
    """({argument's index: (low, high)}, the arguments never NULL, the buffers never overlapping, its own scratch's
    physical ranges, the routines it runs into, the callbacks' argument counts) of the function's EQUIV_RANGE,
    EQUIV_NONNULL, EQUIV_DISJOINT, EQUIV_SCRATCH, EQUIV_RUNS and EQUIV_CALLBACK lines in its C file"""
    text = source.read_text()
    defines = {name: int(value, 0) for name, value in NUMERIC_DEFINE.findall(text)}

    def bound(token):
        if re.fullmatch(NUMBER, token):
            return int(token, 0)
        elif token in defines:
            return defines[token]
        else:
            sys.exit(f"{source}: the contract's bound {token} is neither a number nor a #define of one in the file")

    ranges = {}
    nonnull = set()
    disjoint = []
    scratch = []
    runs = set()
    callbacks = {}
    for found in CONTRACT_LINE.finditer(text):
        written = found.group(0).strip()
        in_range = ARGUMENT_RANGE.fullmatch(written)
        never_null = ARGUMENT_NONNULL.fullmatch(written)
        apart = ARGUMENTS_DISJOINT.fullmatch(written)
        own = SCRATCH_AREA.fullmatch(written)
        into = RUNS_INTO.fullmatch(written)
        called = CALLBACK_ARGUMENT.fullmatch(written)
        line = in_range or never_null or apart or own or into or called
        if line is None:
            sys.exit(f"{source}: `{written}` is none of EQUIV_RANGE(function, argument, low, high), "
                     "EQUIV_NONNULL(function, argument), EQUIV_DISJOINT(function, first, second, count, unit), "
                     "EQUIV_SCRATCH(function, address, bytes), EQUIV_RUNS(function, routine, ...) and "
                     "EQUIV_CALLBACK(function, argument, count)")
        elif line.group(1) != function:
            continue
        elif called is not None:
            index, count = int(called.group(2)), int(called.group(3))
            # group(4) starts with the comma before the first register
            kept = tuple(name.strip() for name in called.group(4).split(",")[1:])
            if index >= ARGUMENT_WORDS or count > len(ARGUMENT_REGISTERS):
                sys.exit(f"{source}: `{written}` names an argument past the {ARGUMENT_WORDS} drawn, or a callback of "
                         f"more than {len(ARGUMENT_REGISTERS)} arguments")
            elif any(name not in CLOBBERED_BY_CALLS for name in kept):
                sys.exit(f"{source}: `{written}` keeps a register a call may not change anyway, or none of the CPU's")
            else:
                callbacks[index] = (count, frozenset(kept))
                continue
        elif into is not None:
            # group(2) starts with the comma after the function
            runs.update(routine.strip() for routine in into.group(2).split(",")[1:])
            continue
        elif own is not None:
            address, length = bound(own.group(2)) & WORD_MASK, bound(own.group(3))
            if address % WORD or length <= 0 or length % WORD:
                sys.exit(f"{source}: `{written}` needs a word-aligned address and a positive count of whole words")
            else:
                scratch.append(range(physical(address), physical(address) + length))
                continue
        index = int(line.group(2))
        if apart is not None:
            first, second, count, unit = (int(apart.group(number)) for number in range(2, 6))
            if max(first, second, count) >= len(ARGUMENT_REGISTERS) or len({first, second, count}) != 3 or unit == 0:
                sys.exit(f"{source}: `{written}` needs three different argument registers and a unit of 1 or more")
            else:
                disjoint.append((first, second, count, unit))
                continue
        elif index >= ARGUMENT_WORDS:
            sys.exit(f"{source}: `{written}` names an argument past the {ARGUMENT_WORDS} drawn")
        elif never_null is not None:
            nonnull.add(index)
            continue
        low, high = bound(in_range.group(3)), bound(in_range.group(4))
        if index in ranges:
            sys.exit(f"{source}: two EQUIV_RANGE lines for {function}'s argument {index}")
        elif not SIGNED_MIN <= low <= high <= SIGNED_MAX:
            sys.exit(f"{source}: EQUIV_RANGE of {function}'s argument {index}: [{low}, {high}] isn't a range of words")
        else:
            ranges[index] = (low, high)
    return ranges, frozenset(nonnull), tuple(disjoint), tuple(scratch), frozenset(runs), callbacks


def retail_extent(retail, source, function, entry, size, runs_into):
    """(start, size) of the retail code the function runs as its own: its code, and the handwritten routines its C file
    names its parts (HANDWRITTEN_PARTS, tools/handwritten.py), such as the tails its dispatch jumps into, or says it
    runs into (EQUIV_RUNS), which have to sit next to it with no other handwritten function between"""
    parts = handwritten.parts(source.read_text()).get(function, set()) | runs_into
    if not parts:
        return entry, size
    spans = [(entry, entry + size)]
    for part in sorted(parts):
        address, length = retail.function(part)
        spans.append((address, address + length))
    start, end = min(low for low, _ in spans), max(high for _, high in spans)
    between = sorted(name for name in handwritten.functions() - parts - {function}
                     if name in retail.addresses and start <= retail.addresses[name] < end)
    if between:
        sys.exit(f"{source}: {function}'s parts span {', '.join(between)} too")
    else:
        return start, end - start


def argument_name(index):
    """An argument's register, or its place among the stack's"""
    if index < len(ARGUMENT_REGISTERS):
        return f"$a{index}"
    else:
        return f"argument {index}"


def apart(first, second, length):
    """The second buffer's address, moved right after the first buffer when the two, length bytes each, overlap; its
    offset in a word kept, which picks the copy's path"""
    if first + length <= second or second + length <= first:
        return second
    else:
        return (align(first + length, WORD) + second % WORD) & WORD_MASK


def ranged(value, limits):
    """A word in a signed range: as it is when it falls in it, else moved into it by its remainder"""
    low, high = limits
    signed = value - (1 << WORD_BITS) if value > SIGNED_MAX else value
    if low <= signed <= high:
        return value
    else:
        return (low + value % (high - low + 1)) & WORD_MASK


ROOT_MEMORY, ROOT_ARGUMENT, ROOT_CALL = "memory", "argument", "call"


@dataclass(frozen=True)
class Root:
    """A word of a trial's inputs: of memory (key: its physical address), an argument register (its index), or a
    register a call out returned (the call's index and the register's name)"""
    kind: str
    key: object

    def describe(self, retail):
        if self.kind == ROOT_MEMORY:
            return f"memory at {retail.describe(virtual(self.key))}"
        elif self.kind == ROOT_ARGUMENT:
            return f"$a{self.key}"
        elif self.kind == ROOT_CALL:
            call, register = self.key
            return f"${register} returned by call {call}"
        else:
            raise ValueError(f"root kind {self.kind}")


class Fuzz:
    """A trial's random values, each drawn from a generator seeded by the trial and what it is for"""

    def __init__(self, seed, trial, constants, pointer_parameters, narrow_parameters=(), argument_ranges=None,
                 nonnull_arguments=frozenset(), disjoint_arguments=(), callbacks=None):
        self.key = f"{seed}:{trial}"
        self.constants = constants
        self.pointer_parameters = pointer_parameters
        self.narrow_parameters = narrow_parameters
        self.argument_ranges = argument_ranges or {}
        self.nonnull_arguments = nonnull_arguments
        self.disjoint_arguments = disjoint_arguments
        # {argument's index: (the arguments of the callback it holds, the registers a call to it leaves)} (EQUIV_CALLBACK)
        self.callbacks = callbacks or {}
        # {input word: its replacement}, by repair (MAX_REPAIRS); an input word is a Root
        self.repairs = {}
        # The registers set at random on entry, the arguments aside, inverted bit by bit or zeroed (entry_registers)
        self.inverted = False
        self.zeroed = False

    def with_other_registers(self):
        """The same trial with every bit flipped in the registers that aren't arguments: a random draw may repeat a
        register's value (zeros and small numbers are frequent), a flipped word never does"""
        other = copy.copy(self)
        other.inverted = True
        return other

    def with_zeroed_registers(self):
        """The same trial with the registers that aren't arguments zeroed: a value and its flip can lead the code the same
        way (two huge sizes both covering a box), where 0 doesn't"""
        other = copy.copy(self)
        other.zeroed = True
        return other

    def generator(self, *purpose):
        return random.Random(":".join([self.key, *(str(part) for part in purpose)]))

    def words(self, generator, count):
        return draw(generator, count, self.constants)

    def arguments(self):
        """The function's arguments, in parameter order: a pointer parameter gets a pointer into the arena, or now and
        then NULL; the others any random word. Repaired registers take their replacement"""
        generator = self.generator("arguments")
        values = []
        for index, word in enumerate(self.words(generator, ARGUMENT_WORDS)):
            if index < len(self.pointer_parameters) and self.pointer_parameters[index]:
                # Drawn whether or not the contract keeps it from NULL: the other draws stay the same
                null = generator.randrange(NULL_POINTER_ODDS) == 0 and index not in self.nonnull_arguments
                values.append(0 if null else ARENA_BASE + generator.randrange(ARENA_SIZE // WORD) * WORD)
            else:
                values.append(word)
        for index in range(len(ARGUMENT_REGISTERS)):
            values[index] = self.repairs.get(Root(ROOT_ARGUMENT, index), values[index])
        # TRADEOFF: a parameter declared narrower than a word gets only values of its type, as callers compiled with the
        # prototype pass; the C's declaration is trusted there, and a type narrower than the callers' values would hide
        # what the function does with the upper bits
        for index, narrow in enumerate(self.narrow_parameters[:len(values)]):
            values[index] = narrowed(values[index], narrow)
        # The C's contract with its callers, repaired values included: what no call passes isn't tried
        for index, limits in self.argument_ranges.items():
            values[index] = ranged(values[index], limits)
        for first, second, count, unit in self.disjoint_arguments:
            values[second] = apart(values[first], values[second], values[count] * unit)
        return values

    def gte_words(self):
        """The GTE's 64 registers on entry, data then control (GTE_PLAUSIBLE_ODDS)"""
        generator = self.generator("gte")
        words = self.words(generator, 2 * gte.REGISTERS)
        if generator.random() < GTE_PLAUSIBLE_ODDS:
            control = gte.REGISTERS
            halves = [generator.randint(-GTE_ROTATION_LIMIT, GTE_ROTATION_LIMIT) & HALF_MASK
                      for _ in range(GTE_ROTATION_HALVES)] + [0]
            for index in range(GTE_ROTATION_WORDS):
                words[control + gte.RT + index] = halves[2 * index] | (halves[2 * index + 1] << HALF_BITS)
            words[control + gte.TRX] = generator.randint(-GTE_TRANSLATION_XY_LIMIT, GTE_TRANSLATION_XY_LIMIT) & WORD_MASK
            words[control + gte.TRY] = generator.randint(-GTE_TRANSLATION_XY_LIMIT, GTE_TRANSLATION_XY_LIMIT) & WORD_MASK
            words[control + gte.TRZ] = generator.randint(*GTE_TRANSLATION_Z_RANGE)
            for register in (gte.OFX, gte.OFY):
                offset = generator.randint(-GTE_SCREEN_OFFSET_LIMIT, GTE_SCREEN_OFFSET_LIMIT)
                words[control + register] = (offset << HALF_BITS) & WORD_MASK
            words[control + gte.H] = generator.randint(*GTE_PROJECTION_RANGE)
        else:
            # Random words throughout
            pass
        return words

    def repair(self, root, role):
        """Replaces an input word by what its role takes (MAX_REPAIRS)"""
        generator = self.generator("repair", len(self.repairs))
        if role == ROLE_BASE:
            self.repairs[root] = ARENA_BASE + generator.randrange(ARENA_SIZE // WORD) * WORD
        elif role == ROLE_OFFSET:
            self.repairs[root] = generator.randrange(REPAIR_OFFSET_LIMIT // WORD) * WORD
        elif role == ROLE_COUNT:
            self.repairs[root] = generator.randrange(REPAIR_COUNT_LIMIT)
        else:
            raise ValueError(f"role {role}")

    def memory_repairs(self):
        """{physical address: word} of the repaired words of memory"""
        return {root.key: value for root, value in self.repairs.items() if root.kind == ROOT_MEMORY}


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
        pool_view = memoryview(pool)
        ram = bytearray().join(pool_view[offset:offset + PAGE_SIZE] for offset in offsets[:-1])
        kept = physical(retail.kept_start) - physical(RAM_BASE)
        ram[kept:kept + len(retail.kept)] = retail.kept
        # Zeros below the entry $sp: whatever a frame leaves uninitialised reads alike however it is laid out
        stack = physical(ENTRY_SP) - physical(RAM_BASE)
        ram[stack - STACK_RESERVE:stack] = bytes(STACK_RESERVE)
        stack_arguments = fuzz.arguments()[len(ARGUMENT_REGISTERS):]
        ram[stack + ARGUMENT_HOME_SIZE:stack + ARGUMENT_HOME_SIZE + len(stack_arguments) * WORD] = struct.pack(
            f"<{len(stack_arguments)}I", *stack_arguments)
        # Bit n of the draw keeps word n's retail value
        words = len(retail.data) // WORD
        kept_words = format(generator.getrandbits(words), f"0{words}b")[::-1]
        if fuzz.generator("data").randrange(RETAIL_DATA_ODDS) == 0:
            kept_words = KEPT_BIT * words
        else:
            # Word by word, as drawn
            pass
        first = (kept + len(retail.kept)) // WORD
        with memoryview(ram) as ram_view, memoryview(retail.data) as data_view:
            ram_words, data_words = ram_view.cast(WORD_FORMAT), data_view.cast(WORD_FORMAT)
            for index, bit in enumerate(kept_words):
                if bit == KEPT_BIT:
                    ram_words[first + index] = data_words[index]
                else:
                    continue
            ram_words.release()
            data_words.release()
        self.regions = {
            physical(RAM_BASE): ram,
            SCRATCHPAD_BASE: pool[offsets[-1]:offsets[-1] + PAGE_SIZE],
            physical(SCRATCH_BASE): candidate.image + bytes(align(len(candidate.image), PAGE_SIZE) - len(candidate.image)),
            physical(SENTINEL_RETURN): bytes(PAGE_SIZE),
        }
        self.code = (physical(retail.kept_start), physical(retail.kept_end))

    def patched(self, words):
        """A copy with {physical address: word} written over"""
        copy = object.__new__(Memory)
        copy.code = self.code
        copy.regions = dict(self.regions)
        for start, content in self.regions.items():
            inside = {address: word for address, word in words.items() if start <= address < start + len(content)}
            if inside:
                patched = bytearray(content)
                for address, word in inside.items():
                    write_word(patched, address - start, word)
                copy.regions[start] = patched
            else:
                continue
        return copy

    def repairable(self, address):
        """Whether a word of memory may be repaired: random on entry, in RAM or the scratchpad, outside the retail code
        and read-only data and below-$sp frame (whose zeros either side reads alike however it lays its frame out)"""
        if address % WORD:
            return False
        elif self.code[0] <= address < self.code[1]:
            return False
        elif physical(ENTRY_SP - STACK_RESERVE) <= address < physical(ENTRY_SP):
            return False
        elif physical(RAM_BASE) <= address < physical(RAM_BASE) + RAM_SIZE:
            return True
        elif SCRATCHPAD_BASE <= address < SCRATCHPAD_BASE + PAGE_SIZE:
            return True
        else:
            return False


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
    # The GTE's registers at the end (gte.Gte.registers)
    gte: tuple = ()
    # Where an exception stopped it
    fault_pc: int = None
    registers: dict = field(default_factory=dict)
    calls: list = field(default_factory=list)
    regions: dict = field(default_factory=dict)


def entry_registers(fuzz):
    """{Unicorn register: value} of the registers set at random on entry, the arguments among them"""
    registers = [REGISTERS[name] for name in RANDOM_ON_ENTRY] + list(MULTIPLY_REGISTERS)
    words = fuzz.words(fuzz.generator("registers"), len(registers))
    if fuzz.zeroed:
        words = [0] * len(words)
    elif fuzz.inverted:
        words = [~word & WORD_MASK for word in words]
    else:
        # The trial's own values
        pass
    values = dict(zip(registers, words))
    values.update(zip(ARGUMENT_REGISTERS, fuzz.arguments()))
    return values


def register_value(uc, number):
    """A general register's value, by its number"""
    return uc.reg_read(REGISTER_NUMBERS[number]) if number else 0


def points_to_memory(value):
    """Whether a word is an address in RAM through kseg0 or kseg1, or in the scratchpad"""
    address = physical(value)
    if SCRATCHPAD_BASE <= address < SCRATCHPAD_BASE + PAGE_SIZE:
        return True
    elif KSEG0 <= value < KSEG2:
        return physical(RAM_BASE) <= address < physical(RAM_BASE) + RAM_SIZE
    else:
        return False


def as_offset(origin):
    """An origin added to an address in memory: an offset, unless a count already"""
    if origin is None:
        return None
    elif origin[1] == ROLE_COUNT:
        return origin
    else:
        return origin[0], ROLE_OFFSET


def access_fault(status):
    """Whether a run ended on a memory access fault, which a repair may lift"""
    return status in ACCESS_EXCEPTIONS or status.startswith(ACCESS_PREFIXES)


def decode(code):
    """What each instruction of a function's code does to the registers' origins (Tracer): (effect, the base register
    if it accesses memory else $zero, source register, other source, destination register, immediate)"""
    decoded = []
    for position in range(0, len(code), WORD):
        word = read_word(code, position)
        opcode = word >> OPCODE_SHIFT
        rs = (word >> RS_SHIFT) & REGISTER_FIELD
        rt = (word >> RT_SHIFT) & REGISTER_FIELD
        rd = (word >> RD_SHIFT) & REGISTER_FIELD
        funct = word & FUNCT_MASK
        access = rs if opcode in LOAD_OPCODES or opcode in STORE_OPCODES or opcode in (OP_LWC2, OP_SWC2) else 0
        source, other, destination = 0, 0, 0
        if opcode == OP_LW:
            effect, source, destination = EFFECT_LW, rs, rt
        elif opcode in LOAD_OPCODES:
            effect, destination = EFFECT_DROP, rt
        elif opcode in (OP_ADDI, OP_ADDIU):
            effect, source, destination = EFFECT_MOVE, rs, rt
        elif opcode in IMMEDIATE_ALU_OPCODES:
            effect, destination = EFFECT_DROP, rt
        elif opcode == OP_JAL or (opcode == OP_REGIMM and rt in REGIMM_LINKS):
            effect, destination = EFFECT_DROP, RA_NUMBER
        elif opcode in (OP_COP0, OP_COP2) and rs in COPROCESSOR_MOVES_TO_GPR:
            effect, destination = EFFECT_DROP, rt
        elif opcode != OP_SPECIAL:
            # Stores, branches, jumps, the coprocessors' operations and transfers to memory: no general register written
            effect = EFFECT_NONE
        elif funct in FUNCT_SHIFTS:
            effect, source, destination = EFFECT_SCALE, rt, rd
        elif funct in FUNCT_MULTIPLIES:
            effect, source, other = EFFECT_MULTIPLY, rs, rt
        elif funct in FUNCT_FROM_MULTIPLY:
            effect, destination = EFFECT_FROM_MULTIPLY, rd
        elif funct in FUNCT_TO_MULTIPLY:
            effect = EFFECT_TO_MULTIPLY
        elif funct in FUNCT_NO_DESTINATION:
            effect = EFFECT_NONE
        elif funct in (FUNCT_ADD, FUNCT_ADDU, FUNCT_OR) and (not rs or not rt):
            # A move: the origin of the source, none of $zero
            effect, source, destination = EFFECT_MOVE, rs or rt, rd
        elif funct in (FUNCT_ADD, FUNCT_ADDU) or funct in FUNCT_SUBTRACTS:
            effect, source, other, destination = EFFECT_SUM, rs, rt, rd
        else:
            effect, destination = EFFECT_DROP, rd
        if destination == 0 and effect in WRITING_EFFECTS:
            # A write to $zero
            effect = EFFECT_NONE
        decoded.append((effect, access, source, other, destination, signed16(word & HALF_MASK)))
    return decoded


def as_count(origin):
    """An origin shifted or multiplied"""
    return None if origin is None else (origin[0], ROLE_COUNT)


class Tracer:
    """Follows a retail run instruction by instruction, keeping for each register the input word it holds, if any: an
    origin, (Root, its role). Moves and immediate additions keep a register's origin, shifts and multiplications make
    it a count; an addition keeps the origin of the operand not in memory already, an offset (unless a count) when the
    other operand is in memory; any other write drops it"""

    def __init__(self, memory, decoded, start, repaired, buffers):
        self.memory = memory
        self.decoded = decoded
        # Where the decoded code starts: the function's entry, or the first of its parts before it
        self.start = start
        # The roots repaired already
        self.repaired = repaired
        # RAM's and the scratchpad's words on entry, and as the run goes (the tracing machine's buffers)
        self.initial_words = {start: memoryview(memory.regions[start]).cast(WORD_FORMAT)
                              for start in REPAIRABLE_REGIONS}
        self.words = {start: memoryview(buffers[start]).cast(WORD_FORMAT) for start in REPAIRABLE_REGIONS}
        # By register number; $zero never has one
        self.origins = {REGISTER_NAMES.index(f"a{index}") + 1: (Root(ROOT_ARGUMENT, index), ROLE_BASE)
                        for index in range(len(ARGUMENT_REGISTERS))}
        # The origin of $hi and $lo
        self.multiplied = None
        self.steps = 0
        # The origin of the base register of the last instruction followed, when it accesses memory: the faulting one's
        self.access = None

    def called(self, call):
        for name in CLOBBERED_BY_CALLS:
            self.origins[REGISTER_NAMES.index(name) + 1] = (Root(ROOT_CALL, (call, name)), ROLE_BASE)

    def step(self, uc, address):
        """Follows the instruction at address, before it runs; False past TRACE_LIMIT"""
        self.steps += 1
        if self.steps > TRACE_LIMIT:
            return False
        effect, access, source, other, destination, immediate = self.decoded[(address - self.start) // WORD]
        origins = self.origins
        self.access = origins.get(access)
        if effect == EFFECT_NONE:
            pass
        elif effect == EFFECT_MOVE:
            origins[destination] = origins.get(source)
        elif effect == EFFECT_DROP:
            origins[destination] = None
        elif effect == EFFECT_LW:
            target = physical((register_value(uc, source) + immediate) & WORD_MASK)
            if self.memory.repairable(target):
                start = SCRATCHPAD_BASE if target >= SCRATCHPAD_BASE else physical(RAM_BASE)
                index = (target - start) // WORD
                initial = self.words[start][index] == self.initial_words[start][index]
            else:
                initial = False
            origins[destination] = (Root(ROOT_MEMORY, target), ROLE_BASE) if initial else None
        elif effect == EFFECT_SUM:
            origins[destination] = self.sum(uc, source, other)
        elif effect == EFFECT_SCALE:
            origins[destination] = as_count(origins.get(source))
        elif effect == EFFECT_MULTIPLY:
            self.multiplied = as_count(origins.get(source)) or as_count(origins.get(other))
        elif effect == EFFECT_FROM_MULTIPLY:
            origins[destination] = self.multiplied
        elif effect == EFFECT_TO_MULTIPLY:
            self.multiplied = None
        else:
            raise ValueError(f"effect {effect}")
        return True

    def sum(self, uc, rs, rt):
        """The origin of rs + rt: the input that isn't an address in memory already, an offset when the other is one"""
        # TRADEOFF: one origin per register; an address built from two bad inputs is repaired one input per run
        left, right = self.origins.get(rs), self.origins.get(rt)
        left_valid, right_valid = points_to_memory(register_value(uc, rs)), points_to_memory(register_value(uc, rt))
        if left_valid and right_valid:
            # Both in memory: their sum isn't, and no input of them is to blame alone
            return None
        elif left_valid:
            return as_offset(right)
        elif right_valid:
            return as_offset(left)
        elif left is not None and (right is None or left[0] not in self.repaired):
            return left
        else:
            # Neither in memory: the one not repaired yet, if either
            return right


def repaired_run(fuzz, base, machine, tracing, decoded):
    """The trial's retail run on machine, its inputs repaired while it ends on an access fault through one of them (the
    tracing machine finds which): the memory both sides run from, and the retail run on it"""
    memory = base
    expected = machine.run(memory, fuzz)
    for _ in range(MAX_REPAIRS):
        if not access_fault(expected.status):
            break
        tracer = Tracer(memory, decoded, machine.start, set(fuzz.repairs), tracing.buffers)
        traced = tracing.run(memory, fuzz, tracer)
        if traced.status != expected.status or tracer.access is None:
            # Faulting past TRACE_LIMIT, or through no input word
            break
        root, role = tracer.access
        if root not in fuzz.repairs:
            fuzz.repair(root, role)
        elif role != ROLE_BASE and fuzz.repairs[root] != 0:
            fuzz.repairs[root] = 0
        else:
            # A pointer, or a zero, faulting again: its address comes from elsewhere too
            break
        memory = base.patched(fuzz.memory_repairs())
        expected = machine.run(memory, fuzz)
    return memory, expected


class Machine:
    """An emulator running one function's code, kept from run to run while they return or reach the limit: the regions
    are buffers of this process mapped in place, each run copies the trial's memory into them and starts from the CPU
    state saved when the emulator was made. A new emulator per run spent most of a trial allocating, mapping and reading
    back 2 MB. Translated code is kept: the retail code is read-only, and a run that writes into the function's own code
    (the candidate's, in the scratch area) flushes it. A Tracer, if made with one, follows each of the function's
    instructions"""

    def __init__(self, memory, retail, entry, size, arguments, traced=False, start=None):
        self.retail = retail
        self.entry = entry
        # The function's own code, where a jump isn't a call out: from its entry unless it has static helpers before it
        self.start = entry if start is None else start
        self.size = size
        self.code = memory.code
        # The pages Unicorn keeps read-only (make_emulator), where a store is a store to code
        self.protected = (align(self.code[0], PAGE_SIZE), self.code[1] // PAGE_SIZE * PAGE_SIZE)
        self.gte_instructions, self.delay_slots = gte_addresses(memory, self.start, size)
        self.gte = None
        self.traced_hook = traced
        self.buffers = {start: bytearray(len(content)) for start, content in memory.regions.items()}
        # A memoryview copies a trial's memory in about three times faster than slice assignment does
        self.views = {start: memoryview(buffer) for start, buffer in self.buffers.items()}
        # {callee address: the arguments compared at a call to it}
        self.arguments = arguments
        # {callee address: (lowest, highest) of what it returns} (CALLEE_RETURN_RANGES)
        self.return_ranges = {retail.addresses[name]: bounds for name, bounds in CALLEE_RETURN_RANGES.items()
                              if name in retail.addresses}
        # The run in progress
        self.outcome = None
        self.fuzz = None
        self.tracer = None
        self.returned = None
        # {callee address: the arguments compared} for the run in progress, its callbacks' among them, and the registers
        # its callbacks leave
        self.run_arguments = arguments
        self.run_kept = {}
        self.emulator, self.fresh = self.make_emulator()

    def make_emulator(self):
        """A new emulator on the buffers, and its CPU state before any run"""
        emulator = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 | UC_MODE_LITTLE_ENDIAN)
        for start, buffer in self.buffers.items():
            address = ctypes.addressof((ctypes.c_char * len(buffer)).from_buffer(buffer))
            emulator.mem_map_ptr(start, len(buffer), UC_PROT_ALL, address)
        code_start, code_end = self.protected
        emulator.mem_protect(code_start, code_end - code_start, UC_PROT_READ | UC_PROT_EXEC)
        emulator.hook_add(UC_HOOK_MEM_UNMAPPED, self.unmapped)
        emulator.hook_add(UC_HOOK_MEM_WRITE_PROT, self.store_to_code)
        if self.traced_hook:
            emulator.hook_add(UC_HOOK_CODE, self.traced, begin=self.start, end=self.start + self.size - 1)
        emulator.hook_add(UC_HOOK_CODE, self.call_out, begin=0, end=self.start - 1)
        emulator.hook_add(UC_HOOK_CODE, self.call_out, begin=self.start + self.size, end=WORD_MASK)
        emulator.hook_add(UC_HOOK_INTR, self.exception)
        for address in self.gte_instructions:
            emulator.hook_add(UC_HOOK_CODE, self.coprocessor, begin=address, end=address)
        for address in self.delay_slots:
            emulator.hook_add(UC_HOOK_CODE, self.delayed_coprocessor, begin=address, end=address)
        return emulator, emulator.context_save()

    def run(self, memory, fuzz, tracer=None):
        """Runs the function on the trial's memory and registers: how it ended, its registers, its calls out and its
        final memory (the machine's buffers, valid until its next run)"""
        self.outcome = outcome = Run()
        self.fuzz = fuzz
        self.tracer = tracer
        # The values calls return, drawn in call order: the same for the same calls on both sides
        self.returned = fuzz.generator("calls")
        self.gte = gte.Gte(fuzz.gte_words())
        emulator = self.emulator
        emulator.context_restore(self.fresh)
        for start, content in memory.regions.items():
            self.views[start][:] = content
        # A callback's calls compare the arguments it takes, wherever the trial points it: a register's value, or the
        # stack's word as the trial's memory holds it, repairs included
        self.run_arguments = dict(self.arguments)
        self.run_kept = {}
        registers = entry_registers(fuzz)
        for index, (count, kept) in fuzz.callbacks.items():
            if index < len(ARGUMENT_REGISTERS):
                target = registers[ARGUMENT_REGISTERS[index]]
            else:
                target = read_word(emulator.mem_read(physical(ENTRY_SP + ARGUMENT_HOME_SIZE)
                                                     + (index - len(ARGUMENT_REGISTERS)) * WORD, WORD), 0)
            # A jump to an odd address lands on the even one below (the MIPS16 bit, which Unicorn clears)
            self.run_arguments[target & ~MIPS16_BIT] = count
            self.run_kept[target & ~MIPS16_BIT] = kept
        # A GTE instruction in a delay slot runs as its branch starts (delayed_coprocessor), the slot a nop meanwhile
        self.patch_delay_slots(0)
        for register, value in entry_registers(fuzz).items():
            emulator.reg_write(register, value)
        emulator.reg_write(REGISTERS["gp"], self.retail.gp)
        emulator.reg_write(REGISTERS["sp"], ENTRY_SP)
        emulator.reg_write(REGISTERS["ra"], SENTINEL_RETURN)
        try:
            emulator.emu_start(self.entry, SENTINEL_RETURN, count=MAX_INSTRUCTIONS)
        except UcError as error:
            if outcome.status is None:
                outcome.status = f"emulator error: {error}"
        for slot, word in self.delay_slots.values():
            self.patch_delay_slots(word, slot)
        if outcome.status is None:
            outcome.status = RETURNED if emulator.reg_read(mips.UC_MIPS_REG_PC) == SENTINEL_RETURN else LIMIT
        outcome.registers = {name: emulator.reg_read(REGISTERS[name]) for name in REGISTER_NAMES}
        outcome.gte = self.gte.registers()
        outcome.regions = self.buffers
        code = physical(self.start)
        for start, buffer in self.buffers.items():
            if start <= code < start + len(buffer) and buffer[code - start:code - start + self.size] != \
                    memory.regions[start][code - start:code - start + self.size]:
                emulator.ctl_flush_tb()
            else:
                continue
        if outcome.status not in (RETURNED, LIMIT):
            # Unicorn 2.1.4 keeps something of a guest exception that its saved CPU state doesn't hold: the next run on
            # the same emulator may crash the process (func_8001AA80, seed 1: trial 346 after trial 345's address error)
            self.emulator, self.fresh = self.make_emulator()
        return outcome

    def unmapped(self, uc, access, address, size, value, data):
        # Ends the run: mapping on demand isn't safe (PAGE_SIZE's comment)
        self.outcome.status = f"{UNMAPPED} {virtual(address):#010x}"
        return False

    def store_to_code(self, uc, access, address, size, value, data):
        self.outcome.status = f"{STORE_TO_CODE} {virtual(address):#010x}"
        return False

    def traced(self, uc, address, size, data):
        if not self.tracer.step(uc, address):
            self.outcome.status = LIMIT
            uc.emu_stop()

    def call_out(self, uc, address, size, data):
        outcome = self.outcome
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
        count = self.run_arguments.get(address, len(ARGUMENT_REGISTERS))
        sp = uc.reg_read(REGISTERS["sp"])

        def argument(value):
            # An address in the caller's frame stands for the words it points to: frames may be laid out differently,
            # and a local passed by reference is compared by its value
            # TRADEOFF: FRAME_WORDS words whatever the local's size; past a smaller local they may be other locals,
            # which a frame laid out otherwise holds elsewhere. Unset words read zero alike (Memory)
            # Only while $sp is the stack's: handwritten code holds other values in it (func_8003CD0C an origin's x)
            if ENTRY_SP - STACK_RESERVE <= sp <= value < ENTRY_SP:
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
        call = len(outcome.calls) - 1
        values = self.fuzz.words(self.returned, len(CLOBBERED_BY_CALLS))
        bounds = self.return_ranges.get(address)
        kept = self.run_kept.get(address, frozenset())
        for name, value in zip(CLOBBERED_BY_CALLS, values):
            if name in kept:
                # A callback that never writes it (EQUIV_CALLBACK)
                continue
            value = self.fuzz.repairs.get(Root(ROOT_CALL, (call, name)), value)
            if name == "v0" and bounds is not None:
                low, high = bounds
                value = low + value % (high - low + 1)
            else:
                # Any word
                pass
            uc.reg_write(REGISTERS[name], value)
        if self.tracer is not None:
            self.tracer.called(call)
        uc.reg_write(mips.UC_MIPS_REG_PC, uc.reg_read(REGISTERS["ra"]))

    def exception(self, uc, number, data):
        outcome = self.outcome
        if outcome.status is not None:
            # A hook ended the run already, on the instruction that raised this
            pass
        elif number in UNSUPPORTED_EXCEPTIONS:
            outcome.unsupported = UNSUPPORTED_EXCEPTIONS[number]
            outcome.status = outcome.unsupported
        elif number in EXCEPTIONS:
            outcome.status = EXCEPTIONS[number]
            outcome.fault_pc = uc.reg_read(mips.UC_MIPS_REG_PC)
        else:
            outcome.status = f"exception {number}"
        uc.emu_stop()

    def patch_delay_slots(self, word, only=None):
        """Writes word into the delay slots of GTE instructions (or into the one at only) in the machine's buffers"""
        for slot, _ in self.delay_slots.values():
            if only is not None and slot != only:
                continue
            address = physical(slot)
            for start, buffer in self.buffers.items():
                if start <= address < start + len(buffer):
                    write_word(buffer, address - start, word)
                else:
                    continue

    def delayed_coprocessor(self, uc, address, size, data):
        """Runs the GTE instruction in this branch's delay slot on the model, before the branch"""
        slot, word = self.delay_slots[address]
        self.gte_instruction(uc, slot, word)
        if self.outcome.status is not None:
            uc.emu_stop()
        else:
            # The branch and its emptied slot run on
            pass

    def coprocessor(self, uc, address, size, data):
        """Runs a GTE instruction on the model and skips it"""
        word = read_word(uc.mem_read(physical(address), WORD), 0)
        self.gte_instruction(uc, address, word)
        if self.outcome.status is None:
            uc.reg_write(mips.UC_MIPS_REG_PC, address + WORD)
        else:
            uc.emu_stop()

    def gte_instruction(self, uc, address, word):
        """A GTE instruction's effect on the model and the CPU's registers or memory; an unsupported one, or a fault,
        sets the run's ending"""
        outcome = self.outcome
        opcode = word >> OPCODE_SHIFT
        rs = (word >> RS_SHIFT) & REGISTER_FIELD
        rt = (word >> RT_SHIFT) & REGISTER_FIELD
        rd = (word >> RD_SHIFT) & REGISTER_FIELD
        if opcode == OP_COP2 and word & COP2_COMMAND:
            if not self.gte.command(word):
                outcome.unsupported = f"GTE command {word:#010x}, which tools/gte.py doesn't model"
                outcome.status = outcome.unsupported
        elif opcode == OP_COP2 and rs in (COP2_MFC2, COP2_CFC2):
            value = self.gte.read_data(rd) if rs == COP2_MFC2 else self.gte.read_control(rd)
            if rt:
                uc.reg_write(REGISTER_NUMBERS[rt], value)
            else:
                # A move to $zero
                pass
        elif opcode == OP_COP2 and rs == COP2_MTC2:
            self.gte.write_data(rd, register_value(uc, rt))
        elif opcode == OP_COP2 and rs == COP2_CTC2:
            self.gte.write_control(rd, register_value(uc, rt))
        elif opcode in (OP_LWC2, OP_SWC2):
            store = opcode == OP_SWC2
            target = (register_value(uc, rs) + signed16(word & HALF_MASK)) & WORD_MASK
            if self.gte_access_fault(target, store):
                outcome.fault_pc = address
            elif store:
                uc.mem_write(physical(target), struct.pack("<I", self.gte.read_data(rt)))
            else:
                self.gte.write_data(rt, read_word(uc.mem_read(physical(target), WORD), 0))
        else:
            outcome.unsupported = f"coprocessor 2 instruction {word:#010x}, which tools/gte.py doesn't model"
            outcome.status = outcome.unsupported

    def gte_access_fault(self, target, store):
        """Sets the run's ending when lwc2 or swc2 at target faults as a load or store there would; whether it does"""
        outcome = self.outcome
        address = physical(target)
        if target % WORD:
            outcome.status = EXCEPTIONS[EXCEPTION_ADDRESS_STORE if store else EXCEPTION_ADDRESS_LOAD]
        elif target >= KSEG2:
            outcome.status = EXCEPTIONS[EXCEPTION_TLB_STORE if store else EXCEPTION_TLB_LOAD]
        elif store and self.protected[0] <= address < self.protected[1]:
            outcome.status = f"{STORE_TO_CODE} {virtual(address):#010x}"
        elif not any(start <= address < start + len(buffer) for start, buffer in self.buffers.items()):
            outcome.status = f"{UNMAPPED} {virtual(address):#010x}"
        else:
            return False
        return True


def gte_addresses(memory, start, size):
    """(the addresses of the GTE instructions in a function's own code, {branch address: (slot address, word)} of those
    in a delay slot). One in a delay slot runs on the model as its branch starts (delay_slot_safe), else it ends the
    program, unsupported"""
    region = next(base for base in memory.regions if base <= physical(start) < base + len(memory.regions[base]))
    code = memory.regions[region][physical(start) - region:physical(start) - region + size]
    found = []
    slots = {}
    previous = 0
    for offset in range(0, len(code), WORD):
        word = read_word(code, offset)
        if word >> OPCODE_SHIFT in GTE_OPCODES and delayed(previous):
            if delay_slot_safe(previous, word):
                slots[start + offset - WORD] = (start + offset, word)
            else:
                print(f"unsupported: the GTE instruction at {start + offset:#010x} is in a delay slot, where it moves "
                      f"a register its branch reads or links")
                sys.exit(EXIT_UNSUPPORTED)
        elif word >> OPCODE_SHIFT in GTE_OPCODES:
            found.append(start + offset)
        else:
            # Not the GTE's
            pass
        previous = word
    return found, slots


def delay_slot_safe(branch, word):
    """Whether a GTE instruction in branch's delay slot can run before the branch: it writes no CPU register (mfc2,
    cfc2 do), and reads none the branch links into ($ra)"""
    opcode = word >> OPCODE_SHIFT
    rs = (word >> RS_SHIFT) & REGISTER_FIELD
    rt = (word >> RT_SHIFT) & REGISTER_FIELD
    branch_opcode = branch >> OPCODE_SHIFT
    links = (branch_opcode == OP_JAL
             or (branch_opcode == OP_REGIMM and (branch >> RT_SHIFT) & REGISTER_FIELD in REGIMM_LINKS)
             or (branch_opcode == OP_SPECIAL and branch & FUNCT_MASK == FUNCT_JALR))
    if opcode == OP_COP2 and word & COP2_COMMAND:
        return True
    elif opcode == OP_COP2 and rs in (COP2_MTC2, COP2_CTC2):
        return not (links and rt == RA_NUMBER)
    elif opcode in (OP_LWC2, OP_SWC2):
        return not (links and rs == RA_NUMBER)
    else:
        # mfc2 and cfc2 write a CPU register
        return False


def delayed(word):
    """Whether the instruction after word is in its delay slot"""
    opcode = word >> OPCODE_SHIFT
    if opcode in BRANCH_OPCODES:
        return True
    elif opcode == OP_SPECIAL:
        return word & FUNCT_MASK in JUMP_REGISTER_FUNCTS
    elif opcode in COPROCESSOR_OPCODES:
        return (word >> RS_SHIFT) & REGISTER_FIELD == COPROCESSOR_BRANCH
    else:
        return False


def changed(memory, outcome):
    """The physical addresses of the words a run changed"""
    words = set()
    for start, initial in memory.regions.items():
        final = outcome.regions[start]
        if final == initial:
            continue
        # Narrowed down by chunks, then pages: slicing a bytearray costs more per slice than per byte
        for chunk in range(0, len(initial), CHUNK_SIZE):
            if final[chunk:chunk + CHUNK_SIZE] == initial[chunk:chunk + CHUNK_SIZE]:
                continue
            for page in range(chunk, min(chunk + CHUNK_SIZE, len(initial)), PAGE_SIZE):
                if final[page:page + PAGE_SIZE] == initial[page:page + PAGE_SIZE]:
                    continue
                words.update(start + offset for offset in range(page, page + PAGE_SIZE, WORD)
                             if final[offset:offset + WORD] != initial[offset:offset + WORD])
    return words


def compare(memory, retail, declared, expected, actual, restored=()):
    """What differs between the retail run (expected) and the candidate's (actual); the callee-saved registers named in
    restored aren't compared (each run gave them back as it got them)"""
    differences = []
    if expected.status == LIMIT:
        # The retail code still running has no final state to compare with; a candidate past the limit where the retail
        # code returned still disagrees (below). TRADEOFF: such trials go uncompared; a higher limit for the retail side
        # would judge them, at the cost of every slow trial
        return differences
    elif expected.status != actual.status:
        differences.append(f"the retail code ended by {expected.status}, the candidate by {actual.status}")
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

    compared = [name for name in CALLEE_SAVED if name not in restored]
    if declared.compare_v0:
        compared.append("v0")
    if declared.compare_v1:
        compared.append("v1")
    for name in compared:
        if expected.registers[name] != actual.registers[name]:
            differences.append(f"${name} is {expected.registers[name]:#010x} after the retail code, "
                               f"{actual.registers[name]:#010x} after the candidate")

    for number, (want, have) in enumerate(zip(expected.gte, actual.gte)):
        if want != have:
            kind = "data" if number < gte.REGISTERS else "control"
            differences.append(f"GTE {kind} register {number % gte.REGISTERS} is {want:#010x} after the retail code, "
                               f"{have:#010x} after the candidate")
        else:
            continue

    frame = range(physical(ENTRY_SP - STACK_RESERVE), physical(ENTRY_SP + ARGUMENT_HOME_SIZE))
    # Its stack frame and its own scratch (EQUIV_SCRATCH) aside
    kept = [frame, *declared.scratch]
    for word in sorted(word for word in changed(memory, expected) | changed(memory, actual)
                       if not any(word in area for area in kept)):
        want, have = (next(read_word(content, word - start) for start, content in outcome.regions.items()
                           if start <= word < start + len(content)) for outcome in (expected, actual))
        if want != have:
            differences.append(f"memory at {retail.describe(virtual(word))} is {want:#010x} after the retail code, "
                               f"{have:#010x} after the candidate")
    return differences


def depends_on_unset(memory, retail, declared, first, second):
    """Whether two retail runs of one trial, (outcome, entry registers) each, the second's registers that aren't
    arguments flipped, end otherwise: the code read a register it never set. A register left as each run got it is
    the flip, not a difference; two runs stopped by the same exception also compare where it stopped and their
    registers there, which an address error's status doesn't tell"""
    (one, one_entry), (two, two_entry) = first, second
    kept = [name for name in REGISTER_NAMES if one.registers[name] == one_entry.get(REGISTERS[name])
            and two.registers[name] == two_entry.get(REGISTERS[name])]
    if compare(memory, retail, declared, one, two, [name for name in CALLEE_SAVED if name in kept]):
        return True
    elif one.status in (RETURNED, LIMIT):
        # Compared whole above
        return False
    else:
        # The same exception after the same calls
        return one.fault_pc != two.fault_pc or any(one.registers[name] != two.registers[name]
                                                    for name in REGISTER_NAMES if name not in kept)


def describe_inputs(fuzz, retail):
    """The trial's arguments: $a0-$a3 and the words on the caller's stack, then its repairs"""
    values = fuzz.arguments()
    registers, stack = values[:len(ARGUMENT_REGISTERS)], values[len(ARGUMENT_REGISTERS):]
    repairs = "".join(f", {root.describe(retail)} repaired to {value:#010x}" for root, value in fuzz.repairs.items())
    return (" ".join(f"$a{index}={value:#010x}" for index, value in enumerate(registers))
            + ", stack " + " ".join(f"{word:#010x}" for word in stack) + repairs)


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
    retail_start, retail_size = retail_extent(retail, source, arguments.function, retail_entry, retail_size,
                                              declared.runs_into)
    retail_code = retail.kept[retail_start - retail.kept_start:retail_start - retail.kept_start + retail_size]
    if (declared.argument_ranges or declared.nonnull_arguments or declared.disjoint_arguments or declared.scratch
            or declared.runs_into or declared.callbacks):
        limits = [f"{argument_name(index)} in [{low:#x}, {high:#x}]"
                  for index, (low, high) in sorted(declared.argument_ranges.items())]
        limits += [f"{argument_name(index)} not NULL" for index in sorted(declared.nonnull_arguments)]
        limits += [f"{argument_name(first)} and {argument_name(second)} apart by {argument_name(count)} * {unit} bytes"
                   for first, second, count, unit in declared.disjoint_arguments]
        limits += [f"{shown(area.start):#010x} to {shown(area.stop):#010x} its own scratch" for area in declared.scratch]
        limits += [f"runs {routine} as its own" for routine in sorted(declared.runs_into)]
        limits += [f"{argument_name(index)} a callback of {count} arguments"
                   + (f" leaving {', '.join(f'${name}' for name in sorted(kept))}" if kept else "")
                   for index, (count, kept) in sorted(declared.callbacks.items())]
        print(f"{CONTRACT} {', '.join(limits)}")
    else:
        # Every argument drawn from the whole of its type
        pass

    agreeing = 0
    repaired = 0
    replaced = 0
    retail_limit = 0
    both_exception = 0
    unset = 0
    # The retail code's, run again with other values in the registers it gets unset (made on the first disagreement)
    again_machine = None
    first = None
    constants = immediates(retail_code)
    decoded = decode(retail_code)
    pool = fuzz_pool(arguments.seed, constants)
    machines = None
    for trial in range(arguments.trials):
        fuzz = Fuzz(arguments.seed, trial, constants, declared.pointer_parameters, declared.narrow_parameters,
                    declared.argument_ranges, declared.nonnull_arguments, declared.disjoint_arguments,
                    declared.callbacks)
        base = Memory(fuzz, pool, retail, candidate)
        if machines is None:
            # The retail code's, the tracing one's, the candidate's
            machines = (Machine(base, retail, retail_entry, retail_size, declared.callee_arguments, start=retail_start),
                        Machine(base, retail, retail_entry, retail_size, declared.callee_arguments, traced=True,
                                start=retail_start),
                        Machine(base, retail, candidate.entry, candidate.size, declared.callee_arguments,
                                start=candidate.start))
        memory, expected = repaired_run(fuzz, base, machines[0], machines[1], decoded)
        actual = machines[2].run(memory, fuzz)
        if fuzz.repairs:
            repaired += 1
            replaced += len(fuzz.repairs)
        for side, outcome in (("retail code", expected), ("candidate", actual)):
            if outcome.unsupported is not None:
                print(f"unsupported: trial {trial}: the {side} ran into {outcome.unsupported}")
                sys.exit(EXIT_UNSUPPORTED)
            else:
                continue
        differences = compare(memory, retail, declared, expected, actual)
        if differences:
            if again_machine is None:
                again_machine = Machine(base, retail, retail_entry, retail_size, declared.callee_arguments,
                                        start=retail_start)
            depends = False
            for other in (fuzz.with_other_registers(), fuzz.with_zeroed_registers()):
                again = again_machine.run(memory, other)
                if depends_on_unset(memory, retail, declared, (expected, entry_registers(fuzz)),
                                    (again, entry_registers(other))):
                    depends = True
                    break
                else:
                    # This one ends as the trial did: the next tells more
                    continue
        else:
            depends = False
        if depends:
            # TRADEOFF: the retail code's result here comes from registers its caller left (a path reading one it never
            # set), which C can't name: not compared. A caller's real values would judge such trials
            agreeing += 1
            unset += 1
        elif differences:
            if first is None:
                first = (trial, describe_inputs(fuzz, retail), differences)
        else:
            agreeing += 1
            if expected.status == LIMIT:
                retail_limit += 1
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
    if repaired:
        print(f"{repaired} trials ran from inputs repaired after an access fault ({replaced} words replaced)")
    if retail_limit:
        print(f"{retail_limit} trials agree by the retail code reaching the limit of {MAX_INSTRUCTIONS} instructions or "
              f"{MAX_CALLS} calls (not compared)")
    if both_exception:
        print(f"{both_exception} trials agree by both taking the same exception (calls compared, not memory)")
    if unset:
        print(f"{unset} trials agree by the retail code depending on registers it didn't set (not compared)")
    print(f"{agreeing} of {arguments.trials} trials agree")
    if agreeing != arguments.trials:
        sys.exit(EXIT_DISAGREE)


if __name__ == "__main__":
    main()
