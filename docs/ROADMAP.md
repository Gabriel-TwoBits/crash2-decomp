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
- The build compiling C through `tools/psx_cc.py`, its unmatched functions included from the split (`INCLUDE_ASM`), and
  `tools/progress.py` measuring what is in C.
- The game's code split into 32 source files, `src/game/<first function>.c`, by the small data and rodata each function
  uses (docs/TARGET.md, "The game's source files"), each file's `.sdata` defined in it. The small data's addressing
  proven: through `$gp` only what the file defines (docs/COMPILER.md, "Small data").
- GCC 2.7.2 proven: `func_8001521C` and `func_80032808` match under it and under no other.

Left:

- Per-file compiler profiles, once a file needs one other than the default.
- The files' boundaries where nothing fixes them yet (docs/TARGET.md's last column), and their `.sbss` statics, `.data`
  and `.rodata` in C once a function needs them.

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

With the game split into its files and their small data in C, `func_8001521C` and `func_80032808` landed, and so did
`func_80019E5C`, whose prototype of `func_80019E28` contradicted `func_8002001C`'s.

The game's functions are declared once, in `include/game.h`, which every file includes: `prepare` lists its prototypes
in the brief, and `land` drops a prototype the header has and moves a new one there. Writing it settled four signatures
from the callees' assembly: `func_80019E28`, `func_800163B4` and `func_80013F28` take a pointer the matched callers had
left out (it was still in `$a0`, so the bytes are the same), and `func_80032F18`'s fourth argument is a pointer.

The near misses, by hand: `func_800157A0` landed once its m2c placeholder types became PsyQ's declarations
(`include/sdk.h`), `func_8001FDDC` once decomp-permuter (30 minutes) and a separate statement for its `/ 3` closed the
last 7 words. `func_80038C14` (2 of 61) and `func_800171E8` (6 of 92) did not move; what was tried is in their notes,
and they wait for more of their files in C.

## Next

In order; each is a session's worth of work, and its result goes back here:

1. **`func_8003D2EC`'s `b` against `j`** (docs/TARGET.md): which compiler or assembler setting writes `b` for an
   unconditional branch. It may be the per-file profile the build needs (docs/COMPILER.md): its file may not be built
   with GCC 2.7.2.
2. **More batches**, a few hundred functions in all, between the steps above.

## 4. The engine in C

Function by function, by subsystem: the startup and main loop, the NSF paging, the GOOL interpreter, objects, collision,
rendering, audio. Names and types come with understanding (docs/WORKFLOW.md, "Naming"). Progress is the share of code bytes
matching.

## 5. Beyond matching

With the game in C, a native port (the way Crash Twinsanity's PS2 decomp got its PC port) becomes possible. Not before the
code is.
