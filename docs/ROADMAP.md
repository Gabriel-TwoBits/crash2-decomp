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
`config/match/ledger.json`, each run's notes and C to `config/match/notes/`; `tools/match.py report` sums them up.

Done: the first function matched and landed this way (`func_80014C48`, 2 minutes), then a first batch of 10 spread
over sizes 0x24 to 0x17C, five each with Claude Sonnet 5.5 and Opus 5.5, 20 minutes at most each:

| | Sonnet 5.5 | Opus 5.5 |
| --- | --- | --- |
| Matched and landed | 4 of 5 | 3 of 5 |
| Cost (API prices) | $1.00 | $3.91, plus one run stopped at 20 minutes |
| Time | 3.5 minutes | 33.7 minutes |

The misses: a handwritten GTE routine (no C can give it; `next` now skips the 58 functions splat marks handwritten), a
0x140-byte function left at 16 of 80 words (instruction scheduling around two `mult`s), and a 0x17C one stopped by the time
limit. The batch also showed decomp-permuter's score leaves out `nop`s (`tools/permute.py` now scores its candidates with
the probe) and that a stopped run lost its progress (the script now keeps the best attempt).

A second batch of 20 (0x20 to 0x19C), Sonnet first and Opus on what it left: 12 matched (8 by Sonnet, 4 by Opus on
retry), 8 of them landed, at $4.63 for Sonnet's 19 runs and $10.80 for Opus's 11. Two of the misses were handwritten code
splat doesn't mark (now in `config/match/skip.txt`); three were left within 2 to 7 words. Four matches didn't land, and
why is the next work below: two define a variable the original reads through `$gp` (blocked until the data is split by
file), one still used m2c's placeholder types, one declared a prototype another C function contradicts.

`tools/match.py loop --model claude-sonnet-5-5 --retry-model claude-opus-5-5` now runs this unattended
(docs/WORKFLOW.md, "Running the loop unattended").

## Next

In order; each is a session's worth of work, and its result goes back here:

1. **The game's data in C, file by file.** The original compiler addresses a variable through `$gp` only in the file that
   defines it (docs/COMPILER.md), and 413 retail instructions do: every function using one can only land once its file's
   `.sdata`/`.data` is defined in C. That needs `game.c` split into its original files (their boundaries from the data each
   function uses, its rodata and its calls) and splat's data subsegments split with them. Then land `func_8001521C` and
   `func_80032808` (their C is in `config/match/notes/`).
2. **One header of the game's prototypes** (`include/game.h`), so matched functions agree with each other: a landing
   now fails when two of them declare a callee differently (`func_80019E5C`). `tools/match.py prepare` would put it in the
   agent's context and `land` would add new prototypes to it.
3. **The near misses** in `tools/match.py report`, with Opus and a longer limit, or by hand: what keeps them apart says
   something about the compiler.
4. **`func_8003D2EC`'s `b` against `j`** (docs/TARGET.md): which compiler or assembler setting writes `b` for an
   unconditional branch. It may be the per-file profile the build needs (docs/COMPILER.md), and it may separate GCC 2.6.x
   from 2.7.2.
5. **More batches**, a few hundred functions in all, between the steps above.

## 4. The engine in C

Function by function, by subsystem: the startup and main loop, the NSF paging, the GOOL interpreter, objects, collision,
rendering, audio. Names and types come with understanding (docs/WORKFLOW.md, "Naming"). Progress is the share of code bytes
matching.

## 5. Beyond matching

With the game in C, a native port (the way Crash Twinsanity's PS2 decomp got its PC port) becomes possible. Not before the
code is.
