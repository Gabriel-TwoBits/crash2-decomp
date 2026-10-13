# The compiler

Nothing matches until the compiler, its flags and the assembler's behaviour are the original's. None of it is settled yet;
this file records what is known and how each answer gets proven.

## What is known

| Item | Value | Status |
| --- | --- | --- |
| Compiler | PsyQ's GCC (`ccpsx`), version unknown | To find. 2.6.3 and 2.7.2 are the likeliest: the libraries date from 1996-12 to 1997-03 (docs/TARGET.md) |
| Optimisation | unknown | `-O2` is the usual PsyQ release setting, and what Crash Bandicoot 1 used |
| Small data | `-G8`, gp `0x8005F17C` | Likely: 413 gp-relative instructions. Confirmed when a function using one matches |
| Assembler | Sony's ASPSX, emulated by maspsx | Version unknown (`--aspsx-version`); it decides the padding `nop`s after `mflo`/`mfhi` and how `div` is expanded |
| PsyQ libraries | `bios.c 1.86`, `intr.c 1.76`, `sys.c 1.129` | Read from the executable's strings |

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
(docs/ROADMAP.md, milestone 1): a source file naming its compiler version and flags.

## Results

None yet.
