# Roadmap

Each milestone ends with something checkable. A later one never starts by loosening an earlier one's check.

## 0. The ground (done)

The repository, its licence, the docs, `tools/extract_disc.py` (the executable out of the disc image, checked) and
`tools/setup_toolchain.py` (the compilers, objdiff, maspsx, m2c and decomp-permuter at fixed versions).

## 1. The split rebuilt byte for byte (done)

splat splits `SCUS_941.54` (`config/SCUS_941.54.yaml`: rodata, text, data and bss as one segment each), `configure.py`
writes `build.ninja`, and ninja assembles, links and pads it into `build/SCUS_941.54.exe`, whose SHA-1 is checked against
the retail one (`config/SCUS_941.54.sha1`) on every build. A changed instruction fails the check.

## 2. The compiler proven (in progress)

Done:

- The SDK apart: PsyQ 4.0's library objects found by signature (`tools/find_psyq.py`), each its own subsegment under
  `asm/sdk/`, their 355 functions named in `config/symbol_addrs.txt`.
- The assembler proven (ASPSX 2.30 to 2.49) and the compiler narrowed to GCC 2.6.0 to 2.7.2 at `-O2 -G8`
  (docs/COMPILER.md).
- The build compiling C: `src/game.c` through `tools/psx_cc.py`, its unmatched functions included from the split
  (`INCLUDE_ASM`), the first match in it, and `tools/progress.py` measuring what is in C.

Left:

- A function that matches under one of GCC 2.6.0, 2.6.3 and 2.7.2 only.
- The game's code split into its original source files, as their boundaries become clear (rodata grouped by file, calls
  that stay within a group). It is one unit, `game`, until then.
- Per-file compiler profiles, once a file needs one other than the default.

## 3. The working loop (in progress)

`tools/match.py`: a scratch directory per function (`build/match/<function>/`: the target, m2c's draft with the project's
declarations, a brief of its callers, callees and data, `score.sh`, `permute.sh`, and `tools/match/CLAUDE.md` as the
agent's instructions), headless Claude Code working in it with only those tools, then the result checked by the probe and
landed in `src/game.c` by the script, committed only when the build's SHA-1 holds. Outcomes go to
`build/match/ledger.json`.

Done: the first function matched and landed this way (`func_80014C48`, 2 minutes). Left: a fixed set of functions run
through it to measure the match rate before it is trusted on the rest, and what the runs teach folded back into
`tools/match/CLAUDE.md`.

## 4. The engine in C

Function by function, by subsystem: the startup and main loop, the NSF paging, the GOOL interpreter, objects, collision,
rendering, audio. Names and types come with understanding (docs/WORKFLOW.md, "Naming"). Progress is the share of code bytes
matching.

## 5. Beyond matching

With the game in C, a native port (the way Crash Twinsanity's PS2 decomp got its PC port) becomes possible. Not before the
code is.
