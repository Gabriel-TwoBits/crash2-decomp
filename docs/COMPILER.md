# The compiler

Nothing matches until the compiler, its flags and the assembler's behaviour are the original's. This file records what is
known, what proves it, and what is still open.

## What is known

| Item | Value | Status |
| --- | --- | --- |
| Compiler | PsyQ's GCC (`ccpsx`) 2.7.2 | Proven by `func_8001521C` and `func_80032808` (below), which match under 2.7.2 and no other; for their two files so far, as another file may name another version. `tools/psx_cc.py` defaults to it |
| Optimisation | `-O2` | Matches `func_80011E98` |
| Scheduling | `-mcpu=r8000` | Proven by `func_80018B50` (below): the one CPU model under which it and every function in C match. `cc1`'s default, the R3000's, schedules some prologues otherwise. GCC 2.6.3 and before don't know `r8000` |
| Small data | `-G8`, gp `0x8005F17C` | Proven by `func_8001521C` and `func_80032808`, which read their file's variables through `$gp`; 8 bytes because an 8-byte `RECT` (`D_8005F1B0`) is in `.sdata` and nothing larger is. What goes through `$gp`: below |
| Assembler | Sony's ASPSX 2.30 to 2.49, emulated by maspsx `--aspsx-version=2.34 --expand-div -G8 --use-comm-section` | Proven by `func_80011E98` (below) |
| PsyQ libraries | PsyQ 4.0 | 158 of its library objects found by signature (`tools/find_psyq.py`), against 118 for 3.7 and 108 for 4.2; also the strings `bios.c 1.86`, `intr.c 1.76`, `sys.c 1.129` |

## How a version is proven

One function decides nothing when it compiles the same under several versions (small leaf functions usually do). A version is
established by a function that matches under it and under no other:

1. Pick a function with a frame, calls and a loop (register allocation and scheduling differ most between versions).
2. Write its C, compile it with each of `tools/toolchain/gcc-*-psx` at `-O2 -G8 -mcpu=r8000`, run each through maspsx, and compare with
   the retail bytes (objdiff, relocations masked).
3. Record the result here: the function, the versions and flags tried, which matched. If none match, try the flags next
   (`-O1`, `-O3`, `-fno-schedule-insns`, ...), then the assembler's version.

The game's code and the SDK's may use different versions: the SDK's libraries were prebuilt by Sony. Crash Bandicoot 1's
decomp needed gcc 2.5.7 for its allocator and 2.6.3 for its engine, set per source file. The build is to allow the same
(docs/ROADMAP.md, milestone 2): a source file naming its compiler version and flags.

## Results

The tools: `tools/probe_compiler.py` (one C file against every GCC version and ASPSX behaviour, `--diff` for one side by
side), `tools/psx_cc.py` (the compile path they share), `tools/permute.py` (decomp-permuter on one function).

| Function | What it does | Result |
| --- | --- | --- |
| `func_80011E98` (0x84 bytes) | A number into a 5-character code through a 64-character table (odd numbers), else `sprintf` | **Matches** with GCC 2.6.0, 2.6.3 and 2.7.2 at `-O2 -G8`, ASPSX 2.34 (also with `-fno-schedule-insns2`). GCC 2.5.7: 22 of 33 words differ; 2.8.1: 27. ASPSX before 2.30: 26 differ. ASPSX 2.56 and later: 1 differs, the `li` that retail assembles as `ori` and those versions as `addiu` |
| `func_80018B50` (0x94 bytes) | Walks a tree depth first, calling a callback on each node | **Matches** with GCC 2.7.2 at `-mcpu=r8000`, `r4000` or `r6000`. At the default (`r3000`), `r2000`, `r4600`, `orion` and `default`, 35 of 37 words differ: the argument copies scheduled among the saves instead of into the first load's delay slot. Of the 139 functions in C then, all match at `r8000` (and the build's SHA-1 holds); 10 differ at `r4000`, 7 at `r6000`, 1 at `r4650`. Ten more of the loop's near misses match at `r8000` |
| `func_80018CAC` (0x7C bytes) | Walks a linked list, calling `func_80018AA8` on each node | 2 words differ under 2.6.0 to 2.7.2: the prologue's saves and the argument copy scheduled in another order. decomp-permuter: 20 minutes, nothing better than the base |
| `func_80012F24` (0x114 bytes) | Reads sectors from the CD (`CdIntToPos`, `CdControlF`, `CdSync`, `CdRead`), retrying on error | 50 of 69 differ: the original keeps the first argument in `$a2`. Its retry loop is written with `goto` (a `while` hoists the constant 5 into a saved register) and its locals take 16 bytes |
| `func_80013888` (0xE0 bytes) | Finds a free entry in a 16-entry table of 0x2C-byte slots, by three criteria in turn | 44 of 56 differ: the loops' shape isn't the original's yet |

| `func_8001521C` (0x128 bytes) | Tears down a state object: callbacks, frees, a `$gp` variable of its file | **Matches** with GCC 2.7.2 only, ASPSX 2.34. GCC 2.6.0 and 2.6.3: 36 of 74 words differ, a loop's increment moved into a delay slot. ASPSX 2.21: 43 differ; 2.56 and later: 3 |
| `func_80032808` (0xCC bytes) | Releases a table's entries owned by a key; a `.sdata` and a `.sbss` variable through `$gp` | **Matches** with GCC 2.7.2 only, ASPSX 2.21 and 2.34. GCC 2.6.0 and 2.6.3: 17 of 51 differ |
| `func_8003D2EC` (0x74 bytes) | A sine lookup in a quarter-wave table | **Not compiled**: 8 of 29 words differ under GCC 2.5.7 to 2.7.2 and every ASPSX, 18 under 2.8.1; the same 8 at `-O1`, `-O3`, `-g`, `-fno-thread-jumps`, `-fno-cse-follow-jumps -fno-cse-skip-blocks`, `-fno-rerun-cse-after-loop`; 30 at `-fno-delayed-branch`, 82 at `-O0`. What differs is its `b` jumps (each `cc1`'s `jump` pattern is `%*j %l0` only) and a `b` to the next instruction: the assembly part of the game (docs/TARGET.md, "What the code holds") |

The first functions scored the same under 2.6.0, 2.6.3 and 2.7.2; `func_8001521C` and `func_80032808`, with more calls and
loops, separate them.

## Read-only data

GCC 2.7.2 writes top-level `asm` statements as it reads them and the file's functions after them, each with its jump table
(`include/include_asm.h` works around the first for `INCLUDE_ASM`). Proven by `func_80012010`, `func_80033D10` and
`func_800345C8`: compiled `switch`es whose tables land at their retail addresses after their file's constants, included
with `INCLUDE_RODATA`, the build's SHA-1 holding. Every file with both has its constants before its tables in retail,
which fits constants written the same way; where GCC writes a string literal in a function's code isn't proven, so the
constants stay in the split and C reads them as `extern` (docs/TARGET.md, "The game's read-only data").

## Small data

At `-G8` GCC puts every variable of 8 bytes or less in `.sdata` (initialised) or `.sbss`, and so it does with string
literals of 8 bytes or less. The retail code shows how the assembler addressed them (docs/TARGET.md, "The game's source
files"), and maspsx at `-G8` does the same:

- **Through `$gp` only what the file defines**: its `.sdata`, its `.sbss`, a common it declares (`int x;`). A variable
  declared `extern` is read through `lui`/`%lo`, from any file. So a function reading a variable through `$gp` is in the
  file that defines it, and its C defines it too. The two matches above prove it: as `extern`, their variables compile to
  `lui`/`lw`.
- **An address never** (`la`): `&x` is `lui`/`addiu` even in the defining file (`func_800125F8` with `D_8005F1B0`).
- **An array's address is kept, a scalar's is not**: an `extern s32 x[]` read and written as `x[0]` around a call gets its
  address (`lui`/`addiu`) once, in a saved register, and `0x0($sN)` after; an `extern s32 x` gets a `lui` at each access.
  `func_80021F4C` with `D_8005B9AC` shows both (a scalar, a pointer to it and `volatile` never gave the retail's `$s1`).
  Matched functions declare `D_800608D4` and `D_800608D8` as arrays for the same reason. A structure over neighbouring
  variables is not the same thing: `D_800608CC` to `D_80060914` as one structure moved its base into a saved register
  where retail has a `lui` per variable (34 instructions without a counterpart became 56).

How the build follows it:

- A file's `.sdata` is defined at the top of its C file with the retail values, in address order: GCC writes it in the order
  of the definitions. GCC aligns arrays and structures on a word (`DATA_ALIGNMENT`), so `D_8005F1B0` is a `RECT`, not
  four `s16`, and an array with an initialiser between braces needs its size (`s32 x[2] = {...}`): without it, GCC picks
  the section before the size is known and puts it in `.data`.
- `.sbss` stays in the split: a variable in it is a tentative definition (`s32 D_8005F304;`), a common
  (`--use-comm-section`) that resolves to the split's symbol, so nothing moves.
- `tools/psx_cc.py` ends a C file's data sections on a word: the linker script packs sections at 2 bytes (`SUBALIGN(2)`),
  while each file's data starts on a word in retail.

## `main`

GCC 2.7.2 calls `__main` first in a function named `main`, by itself. Proven by `main` (`0x800117BC`): retail starts with
`jal __main`, and the C without the call compiles to it, the build's SHA-1 holding; while it was `func_800117BC` the C
called `__main()` explicitly, and the same body named `main` with that call kept compiled to two `jal __main`.

## Structure members and plain pointers

GCC 2.7.2 schedules an access to a structure's member otherwise than the same access through a plain pointer: it tells
memory in a structure from memory that isn't (`MEM_IN_STRUCT_P`) when it decides what may alias. Proven by
`func_8002BF58`: its store to the game object's 0x114 written as `*(s32 *)((s8 *)arg0 + 0x114)` matches, the same store
written as `arg0->unk_114` fails the build's SHA-1. Matched and equivalent C keeps such accesses at their raw offsets,
with a comment, where the member would move code (`func_80029884`'s reads of 0x60 to 0x68 move a store after them).

## Address arithmetic

How C spells an address moves GCC 2.7.2's code even where the value is the same:

- **A sum as integers or as a pointer**: `func_80021900` adds a bucket's byte offset to `D_80067590` as integers
  (`((v >> 13) & 0x3FC) + (s32)D_80067590`) and matches; the same sum on a `u8 *` swaps `addu`'s operands and fails
  the build's SHA-1.
- **A member's address**: `&nsd->records[n]` adds the member's offset first, `(u8 *)nsd + n * 8 + 0x520` last; only the
  second leaves the equivalent executable as it was (`Nsf_OpenLevel`, `func_800170B4`). A member read straight from an
  array element (`arg0->page_slots[i].unk_4`) folds both offsets into the load's, as the raw offset does; through a
  temporary pointer to the element it costs an `addiu` (`func_800155E0`).
