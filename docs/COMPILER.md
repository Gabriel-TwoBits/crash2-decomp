# The compiler

Nothing matches until the compiler, its flags and the assembler's behaviour are the original's. This file records what is
known, what proves it, and what is still open.

## What is known

| Item | Value | Status |
| --- | --- | --- |
| Compiler | PsyQ's GCC (`ccpsx`) 2.7.2 | Proven by `func_8001521C` and `func_80032808` (below), which match under 2.7.2 and no other; for their two files so far, as another file may name another version. `tools/psx_cc.py` defaults to it |
| Optimisation | `-O2` | Matches `func_80011E98` |
| Small data | `-G8`, gp `0x8005F17C` | Proven by `func_8001521C` and `func_80032808`, which read their file's variables through `$gp`; 8 bytes because an 8-byte `RECT` (`D_8005F1B0`) is in `.sdata` and nothing larger is. What goes through `$gp`: below |
| Assembler | Sony's ASPSX 2.30 to 2.49, emulated by maspsx `--aspsx-version=2.34 --expand-div -G8 --use-comm-section` | Proven by `func_80011E98` (below) |
| PsyQ libraries | PsyQ 4.0 | 158 of its library objects found by signature (`tools/find_psyq.py`), against 118 for 3.7 and 108 for 4.2; also the strings `bios.c 1.86`, `intr.c 1.76`, `sys.c 1.129` |

## How a version is proven

One function decides nothing when it compiles the same under several versions (small leaf functions usually do). A version is
established by a function that matches under it and under no other:

1. Pick a function with a frame, calls and a loop (register allocation and scheduling differ most between versions).
2. Write its C, compile it with each of `tools/toolchain/gcc-*-psx` at `-O2 -G8`, run each through maspsx, and compare with
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
| `func_80018CAC` (0x7C bytes) | Walks a linked list, calling `func_80018AA8` on each node | 2 words differ under 2.6.0 to 2.7.2: the prologue's saves and the argument copy scheduled in another order. decomp-permuter: 20 minutes, nothing better than the base |
| `func_80012F24` (0x114 bytes) | Reads sectors from the CD (`CdIntToPos`, `CdControlF`, `CdSync`, `CdRead`), retrying on error | 50 of 69 differ: the original keeps the first argument in `$a2`. Its retry loop is written with `goto` (a `while` hoists the constant 5 into a saved register) and its locals take 16 bytes |
| `func_80013888` (0xE0 bytes) | Finds a free entry in a 16-entry table of 0x2C-byte slots, by three criteria in turn | 44 of 56 differ: the loops' shape isn't the original's yet |

| `func_8001521C` (0x128 bytes) | Tears down a state object: callbacks, frees, a `$gp` variable of its file | **Matches** with GCC 2.7.2 only, ASPSX 2.34. GCC 2.6.0 and 2.6.3: 36 of 74 words differ, a loop's increment moved into a delay slot. ASPSX 2.21: 43 differ; 2.56 and later: 3 |
| `func_80032808` (0xCC bytes) | Releases a table's entries owned by a key; a `.sdata` and a `.sbss` variable through `$gp` | **Matches** with GCC 2.7.2 only, ASPSX 2.21 and 2.34. GCC 2.6.0 and 2.6.3: 17 of 51 differ |

The first functions scored the same under 2.6.0, 2.6.3 and 2.7.2; `func_8001521C` and `func_80032808`, with more calls and
loops, separate them.

## Small data

At `-G8` GCC puts every variable of 8 bytes or less in `.sdata` (initialised) or `.sbss`, and so it does with string
literals of 8 bytes or less. The retail code shows how the assembler addressed them (docs/TARGET.md, "The game's source
files"), and maspsx at `-G8` does the same:

- **Through `$gp` only what the file defines**: its `.sdata`, its `.sbss`, a common it declares (`int x;`). A variable
  declared `extern` is read through `lui`/`%lo`, from any file. So a function reading a variable through `$gp` is in the
  file that defines it, and its C defines it too. The two matches above prove it: as `extern`, their variables compile to
  `lui`/`lw`.
- **An address never** (`la`): `&x` is `lui`/`addiu` even in the defining file (`func_800125F8` with `D_8005F1B0`).

How the build follows it:

- A file's `.sdata` is defined at the top of its C file with the retail values, in address order: GCC writes it in the order
  of the definitions. GCC aligns arrays and structures on a word (`DATA_ALIGNMENT`), so `D_8005F1B0` is a `RECT`, not
  four `s16`, and an array with an initialiser between braces needs its size (`s32 x[2] = {...}`): without it, GCC picks
  the section before the size is known and puts it in `.data`.
- `.sbss` stays in the split: a variable in it is a tentative definition (`s32 D_8005F304;`), a common
  (`--use-comm-section`) that resolves to the split's symbol, so nothing moves.
- `tools/psx_cc.py` ends a C file's data sections on a word: the linker script packs sections at 2 bytes (`SUBALIGN(2)`),
  while each file's data starts on a word in retail.
