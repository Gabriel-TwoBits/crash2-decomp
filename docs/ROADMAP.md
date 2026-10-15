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

`func_8003D2EC`'s `b` against `j` is no compiler setting: no GCC writes `b` for a jump, and from `0x80039D2C` to the SDK
every unconditional branch is one. That part of the game is assembly source, `func_8003D2EC` included (docs/TARGET.md,
"What the code holds"); it starts after `func_80039C08`, the last function using `j`. The build now assembles it as one
splat `asm` subsegment (`asm/game/80039D2C.s`): `tools/match.py next` no longer offers its functions, and
`tools/progress.py` measures the compiled code, 166,780 bytes (`func_8003DD0C`, an empty function in it, went back to
assembly).

A third batch of 20 (the smallest left, 0x20 to 0x5C bytes), Sonnet first and Opus on what it left: all 20 matched (18
by Sonnet), at $2.50 for Sonnet and $0.90 for Opus, 15 minutes of runs in all. 16 landed by themselves; 4 needed a hand
where landing met the rest of their file: a variable this project had typed `s32` used as a pointer (`D_8005F264`), a
prototype an earlier landing had put in `include/game.h` with the wrong signature (`func_80036084`), a parameter type
against the callee's definition (`func_8001FF30`), a structure two functions of one file defined apart
(`func_800163E8`). `land` now refuses a data definition whose type differs from its file's, with both in the reason, and
counts only declarations above the function as already there.

A fourth batch of 20 (0x30 to 0x40 bytes): all 20 matched (19 by Sonnet), at $2.73 for Sonnet and $0.52 for Opus, 13
minutes of runs. 19 landed by themselves; `func_80014220` named its structure `Entry`, which `func_8001521C` in the same
file defines otherwise, and landed once renamed. The agent's instructions now ask for structures named after their
function or data.

A fifth batch of 50 (0x40 to 0x64 bytes), in two runs: 42 matched (36 by Sonnet, 5 by Opus, 1 by hand) and 8 were left
as assembly, at $8.83 for Sonnet and $11.17 for Opus, 78 minutes of runs; Opus's retries now cost more than Sonnet's
first runs. 35 landed by themselves; 7 met signatures this project had wrong or left open: definitions typed `void`
that return their callee's result (`func_80035B3C`, `func_80035C40`), a prototype in `include/game.h` returning `void`
(`func_80012ED0`), `func_80018D28` declared as `char` to take its address, a structure its file already defined
(`func_8001FFCC`), and prototypes written against a callee's C definition. `include/game.h` lost its `()` prototypes,
and the SDK functions declared in game files moved to `include/sdk.h`. The brief now lists the signature of every callee
defined in C, and `land` refuses a prototype with an empty parameter list.

A sixth batch of 50 (0x64 to 0x94 bytes), Sonnet only, since Opus's retries had cost $2.23 a match against Sonnet's
$0.25: 21 matched and 29 were left as assembly, at $11.36, 71 minutes of runs. Size doesn't tell them apart (130 bytes on
average left, 123 matched). Four of the 29 are within 10% of a match and five more within 13%; several differ only in
the prologue's schedule, `func_80018B50` and `func_800325DC` among them. 15 landed by themselves; 6 met their file: 2 used structures
their file defines, which `match.c` can't compile without, 2 used `.sbss` variables their file defines further down, 1
read `D_8005F264` as `s32` and 1 typed `D_8006CB6C` against its file. `land` now drops a structure or a declaration
the file already has above the function and moves one the file has further down up to it; `func_80014148` got its
parameter and `D_8006CB6C` its structure.

## Next

In order; each is a session's worth of work, and its result goes back here:

1. **More batches**, a few hundred functions in all.

## 4. The engine in C

Function by function, by subsystem: the startup and main loop, the NSF paging, the GOOL interpreter, objects, collision,
rendering, audio. Names and types come with understanding (docs/WORKFLOW.md, "Naming"). Progress is the share of the
compiled code's bytes matching (`tools/progress.py`); the assembly part stays assembly.

## 5. Beyond matching

With the game in C, a native port (the way Crash Twinsanity's PS2 decomp got its PC port) becomes possible. Not before the
code is.
