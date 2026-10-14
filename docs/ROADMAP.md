# Roadmap

Each milestone ends with something checkable. A later one never starts by loosening an earlier one's check.

## 0. The ground (done)

The repository, its licence, the docs, `tools/extract_disc.py` (the executable out of the disc image, checked) and
`tools/setup_toolchain.py` (the compilers, objdiff, maspsx, m2c and decomp-permuter at fixed versions).

## 1. The split rebuilt byte for byte (done)

splat splits `SCUS_941.54` (`config/SCUS_941.54.yaml`: rodata, text, data and bss as one segment each), `configure.py`
writes `build.ninja`, and ninja assembles, links and pads it into `build/SCUS_941.54.exe`, whose SHA-1 is checked against
the retail one (`config/SCUS_941.54.sha1`) on every build. A changed instruction fails the check.

## 2. The compiler proven

- The code split into its source files (splat's subsegments): the game's units and the SDK's libraries apart. PsyQ's
  libraries come from Sony's prebuilt archives, so their functions are identified by signature.
- A function that matches under one GCC version and no other (docs/COMPILER.md, "How a version is proven"), its flags and
  the assembler's version with it.
- The build compiling C: per-file compiler profiles (a source file naming its GCC version and flags), maspsx, and objdiff's
  configuration so each unit's progress is measured.

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
