# The compiler

Nothing matches until the compiler, its flags and the assembler's behaviour are the original's. This file records what is
known, what proves it, and what is still open.

## What is known

| Item | Value | Status |
| --- | --- | --- |
| Compiler | PsyQ's GCC (`ccpsx`) 2.6.0, 2.6.3 or 2.7.2 | Narrowed by `func_80011E98` (below); not yet separated. `tools/psx_cc.py` defaults to 2.7.2 until a function does |
| Optimisation | `-O2` | Matches `func_80011E98` |
| Small data | `-G8`, gp `0x8005F17C` | Likely: 413 gp-relative instructions. Confirmed when a function using one matches |
| Assembler | Sony's ASPSX 2.30 to 2.49, emulated by maspsx `--aspsx-version=2.34 --expand-div` | Proven by `func_80011E98` (below) |
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

So far every function scores the same under 2.6.0, 2.6.3 and 2.7.2: they differ mostly in register allocation and
scheduling details, which the first functions don't reach.
