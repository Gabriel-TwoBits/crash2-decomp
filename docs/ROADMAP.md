# Roadmap

Each milestone ends with something checkable. A later one never starts by loosening an earlier one's check.

## 0. The ground (done)

The repository, its licence, the docs, `tools/extract_disc.py` (the executable out of the disc image, checked) and
`tools/setup_toolchain.py` (the compilers, objdiff, maspsx, m2c and decomp-permuter at fixed versions).

## 1. The split rebuilt byte for byte

splat splits `SCUS_941.54` into assembly and data (`config/SCUS_941.54.yaml`), the build assembles and links it, makes a
PS-X EXE of it, and the result's SHA-1 is the retail one with 0% of the code in C. From then on every change is checked
against that hash.

- splat's configuration: the segments (the game's code, the SDK's libraries, data, rodata, bss), `gp`, the entry point.
- The build (assemble, link, the PS-X EXE header) and a `check` that compares the SHA-1.
- Per-file compiler profiles (docs/COMPILER.md), even though only one is used at first.
- objdiff's configuration, so each unit's progress is measured.

## 2. The compiler proven

A function that matches under one GCC version and no other (docs/COMPILER.md, "How a version is proven"), its flags and the
assembler's version with it. Then the SDK's: PsyQ's libraries come from Sony's prebuilt archives, so their functions are
identified (by signature) and kept apart from the game's.

## 3. The working loop

The per-function loop of docs/WORKFLOW.md automated: a function's assembly and context to m2c, the draft to an agent, the
result compiled and diffed, decomp-permuter on near misses, the outcome recorded. Measured on a fixed set of functions
before it is trusted on the rest.

## 4. The engine in C

Function by function, by subsystem: the startup and main loop, the NSF paging, the GOOL interpreter, objects, collision,
rendering, audio. Names and types come with understanding (docs/WORKFLOW.md, "Naming"). Progress is the share of code bytes
matching.

## 5. Beyond matching

With the game in C, a native port (the way Crash Twinsanity's PS2 decomp got its PC port) becomes possible. Not before the
code is.
