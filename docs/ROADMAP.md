# Roadmap

Each milestone ends with something checkable. A later one never starts by loosening an earlier one's check.

## 0. The ground (done)

The repository, its licence, the docs, `tools/extract_disc.py` (the executable out of the disc image, checked) and
`tools/setup_toolchain.py` (the compilers, objdiff, maspsx, m2c and decomp-permuter at fixed versions).

## 1. The split rebuilt byte for byte (done)

splat splits `SCUS_941.54` (`config/SCUS_941.54.yaml`: text by source file, the game's rodata by file, data and bss as one segment each), `configure.py`
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
$0.25: 21 matched and 29 were left as assembly, at $11.36, 71 minutes of runs. Size doesn't tell them apart (130 bytes
on average left, 123 matched). Four of the 29 are within 10% of a match and five more within 13%; several differ only in
the prologue's schedule, `func_80018B50` and `func_800325DC` among them. 15 landed by themselves; 6 met their file: 2
used structures their file defines, which `match.c` can't compile without, 2 used `.sbss` variables their file defines
further down, 1 read `D_8005F264` as `s32` and 1 typed `D_8006CB6C` against its file. `land` now drops a structure or a
declaration the file already has above the function and moves one the file has further down up to it; `func_80014148`
got its parameter and `D_8006CB6C` its structure.

The prologues that differed only in their schedule were the compiler's CPU model: at `-mcpu=r8000` every function in C
still matches and the build's SHA-1 holds, and 11 functions the loop had left as assembly match as they were,
`func_80018B50`, `func_800325DC` and the parked `func_80038C14` among them (docs/COMPILER.md, "Scheduling"). `r4000` and
`r6000` match the same 11 but break others; `cc1`'s default, the R3000's, matches neither. The tools compile at
`-mcpu=r8000` now, and the 11 landed (3 by hand, against their file's types). Several other near misses came closer
under it (`func_800343CC` from 28 differing words to 2, `func_80036BD8` to 1).

A seventh run tried again the 32 functions given up under the old flags, Sonnet only, each starting from its earlier
notes and best C (`tools/match.py` now records each run's compiler flags and retries a function given up under others
first): 5 matched, among them the parked `func_800171E8`, at $7.72; 3 landed by themselves, 2 once their file's
declarations agreed (`func_800171E8`'s prototype, `D_8005F288` read as a structure where its file defines the string
`"EMPTY"`), and `func_80036BD8` by hand, its sum written in the original's order. Most of the rest stay within 1 to 3
words, differences in source (operand order, a `$gp` load's place) no `-mcpu` or scheduling flag moves. From here Opus
retries only a near miss (within 10%).

An eighth batch of 50 (0x98 to 0xE4 bytes), Opus only on Sonnet's near misses: 28 matched and 22 were left as assembly,
at $13.32 for Sonnet and $3.74 for Opus, 94 minutes of runs. 18 landed by themselves, 8 once their files agreed with
them: prototypes earlier landings had guessed from a call (`func_800271D8`, `func_80012170`, which its caller passes
four arguments), definitions typed `void` that return their callee's result (`func_8002719C`), variables that are
structures (`D_8005B9E0`, the count before `D_8005BA58`'s entries), a structure's further fields (`Unk80063458`) and a
callee's parameter (`func_8003B940`'s count, `s32`). `land` now gives a function's definition signature to
`include/game.h` in place of a guessed one, refuses a callee prototype that differs from the header's, and records the
compiler's or linker's error line. Two matches couldn't land: `func_80012010` and `func_80033D10` jump through tables
in the executable's one `.rodata` segment, which a compiled `switch` brings with it; 22 functions in assembly do the
same.

The game's `.rodata` is now split by file (docs/TARGET.md, "The game's read-only data"): each jump table goes with its
function, and m2c gets it, so a draft has its `switch`. `func_80012010`, `func_80033D10` and `func_800345C8` landed with
theirs.

A ninth batch of 50 (0xE4 to 0x138 bytes), Opus on Sonnet's near misses: 18 matched and 32 were left as assembly, at
$15.18 for Sonnet and $4.19 for Opus. 11 landed by themselves, 6 by hand once their files agreed: a scratchpad pointer
typed `s32 *` (`D_8005F3C4`), one structure for `D_80062BB4` where two functions had their own, a callee given its
argument (`func_80013968`, which `func_80013A54` called with none) and another its pointer result (`func_8001557C`).

A tenth run tried again the two functions with a jump table given up before the split, then 25 more (0x140 to 0x170
bytes), Opus on Sonnet's near misses: $10.33 for Sonnet and $6.61 for Opus. With its `switch` in m2c's draft,
`func_800344B0` went from 63 words differing to a match; `func_80038F00` stays at 13. Of the 25, 10 matched: 7 landed by
themselves, 3 by hand once their callees' signatures were the assembly's (`func_8003649C` takes an argument it doesn't
read, `func_80030D74` returns `func_80030CD8`'s result and passes its second argument on, `func_80016F44` returns `s32`).
`func_800344B0` returns nothing yet is declared `s32`: as `void`, GCC fills the delay slot before its jump otherwise.

An eleventh batch of 40 (0x174 to 0x240 bytes): 5 matched, at $15.76 for Sonnet and $5.50 for Opus; 4 landed by
themselves, `func_80014790` by hand. On functions this size one register allocated otherwise changes most words, and
agents gave up within two minutes on counts like 79 of 93. `tools/probe_compiler.py --diff` now also counts the retail
instructions without a counterpart once registers are set aside (aligned with `difflib`, registers but `$gp` and `$sp`
normalised): the shape, which the agent brings to 0 first. Four of the batch's give-ups run again with it ($1.48):
`func_80019BBC` matched and landed by hand; `func_8001A90C`, `func_80022314` and `func_8001EE64` still differ in
register allocation, or in tails GCC merges and the original keeps apart.

A twelfth batch of 20 (0x244 to 0x31C bytes), Opus on Sonnet's near misses: 2 matched (`func_80026CA0`, `func_800237F8`,
both landed by hand once a variable had its file's type), at $8.87 for Sonnet and $4.85 for Opus. Of the 18 left, 3 are
a scheduling swap away (`func_800267C4` at 1 word, `func_8001A4A4` and `func_80026A08` at 2; permuter runs found nothing
better), and 13 still differ in shape by 11 to 63 instructions: Sonnet gave up on most of them within two to six minutes,
before an Opus retry, which only near misses get. The rate at this size, 1 in 10, is below the smaller functions'.

A thirteenth batch of 7 (0x320 to 0x37C bytes), Opus as the first model: 1 matched and landed by itself
(`func_800309A8`), at $8.53 plus one run cut at its 15 minutes, whose cost wasn't recorded. Opus gave up as early as
Sonnet on some (under three minutes at 59 and 87 words); its two closest, `func_80033EE0` and `func_8002A554` at 16, differ
in scheduling and register order only. At this size neither model matches more than about 1 function in 7 to 10.

Two tests of keeping the agent on a large function longer, on functions given up within three minutes:
- The instructions (`tools/match/CLAUDE.md`, step 4): rework the shape while it differs, stop after five attempts in a
  row without progress or two minutes before the limit, which `score.sh` now prints. Five runs with Sonnet, $2.13:
  `func_8001A23C` went from 128 words to 7 (12 minutes of permuter then found nothing better); the other four stopped
  again within six minutes, three of them within one.
- `tools/match.py` resuming the session up to three times while the shape differed and time was left. The same four and
  `func_8001CC10`, $11.40: no match, every run resumed three times, shape gains small (94 to 76, 160 to 121 words, the
  others within 4). Each resumption reloads the whole context, $0.5 to $0.9. Reverted.

On functions over about 0x240 bytes the loop's agent doesn't find the shape by itself, however long it is kept on them.

Whether shared types would bring the large functions closer, tried by hand on `func_80021F4C` and against the field
accesses in the assembly:
- A field typed in a structure compiles like the same offset written by hand (`p->f` and `*(s32 *)(p + 0x60)` are one
  load), so typing the game object mostly helps the reader, not the bytes.
- How a global is declared does change the bytes: an array keeps its address in a saved register, a scalar doesn't
  (docs/COMPILER.md, "Small data"). That fixed the first half of `func_80021F4C`.
- Neighbouring globals as one structure made it worse (34 instructions without a counterpart to 56): `D_800608CC` to
  `D_80060914` were separate variables in the original.
- Following the object pointer through calls from the assembly alone reaches few functions (8): most get the object
  through GOOL's tables of function pointers.

The game object's layout is still worth writing down for the port (section 4), but it won't raise the match rate.

The 41 functions under 0x20 bytes, which the loop skips (`MIN_SIZE`), matched by hand and landed with `tools/match.py
land`: getters, setters, the two generators' seeds, an absolute difference, a sign extension, two returning `-0xFF`
(`SUCCESS` in `include/game.h`). Twenty in `80031AE8.c` only store `arg3` in the caller's frame (`jr $ra` /
`sw $a3, 0xC($sp)`): taking its address compiles to that; a variadic definition stores from the last named parameter's
register on and leaves the delay slot empty. `MIN_SIZE` stays: no game function under it is left in assembly.

## Next

In order; each is a session's worth of work, and its result goes back here:

1. **More batches**, a few hundred functions in all.
2. **More equivalent functions** (section 4, "Next").

## 4. The engine in C

Function by function, by subsystem: the startup and main loop, the NSF paging, the GOOL interpreter, objects, collision,
rendering, audio. Names and types come with understanding (docs/WORKFLOW.md, "Naming"). Progress is the share of the
compiled code's bytes matching (`tools/progress.py`); the assembly part stays assembly.

A function too costly to match gets behaviour-equivalent C under `NON_MATCHING` (docs/WORKFLOW.md, "The equivalent
track"). Done: the convention, `ninja nonmatching`, `tools/progress.py` counting equivalent functions apart, and
`tools/equiv.py`, the differential tester that judges them (all 222 functions matched then agree with their C, and a
near miss differing only in registers agrees), and `tools/match.py`'s equivalent mode (docs/WORKFLOW.md, "Landing an
equivalent function" and "The loop's equivalent mode"): `land --equivalent` puts C into the block and keeps it only when
both builds hold and equiv.py agrees on the landed file (2,000 trials, seed 1, at least 5% compared); `loop --equivalent`
runs an agent on functions the matching loop gave up (`tools/match/EQUIVALENT.md`).

Of the matching runs' notes, 125 of the 152 left as assembly agree on 500 trials as they are. Landed from them:
`func_800365E4`, `func_8001C620`, `func_80011764` (the last then rewritten readably, the block replaced). Six notes'
drafts were refused by the builds: four over a variable their file declares with another type (`conflicting types for
D_800608D4`), one over the function's own prototype there, one over a callee's prototype its C definition contradicts.
The brief now lists what the file declares of the function and its data, for the agent to keep.

`land --equivalent` over all 125 landed 74 (no agent, no cost). The 51 refused break a build, all but one over
declarations: a type its file or `include/game.h` declares otherwise (a variable, the function, a callee), a structure
the file defines otherwise or further down; one runs an instruction Unicorn lacks. Landing the same C again leaves the
file and the history alone.

The equivalent executable moves what follows a function of another size; 194 words of `.data` splat had left as numbers
(pointers to the libraries' strings, entries into the handwritten code from dispatch tables) kept their retail values.
They have symbols now (`config/symbol_addrs.txt`) and `configure.py` stops on any such word: with a function and a
string added under `NON_MATCHING`, all 194 follow their targets, where before they all kept the retail value. The
handwritten code's 21 `lui`/`ori` pairs loading the addresses of 20 of the game's functions (`func_80037440` to
`func_80039C08`), which no relocation writes, become `lui`/`addiu` pairs of their symbols in the equivalent build only:
with 0xC00 bytes of code added under `NON_MATCHING` before them, all 21 load their function's linked address, across a
low half's sign (`func_80037448` at 0x80038008).

A first `loop --equivalent` batch of 40 (Sonnet, $0.15 to $0.30 a function) landed 25; 13 of the 14 refused had agreed
500 of 500 in `score.sh`, which judged the draft alone: 8 over a declaration their file or another file's calls have
otherwise, 4 over a structure the file defines further down. `score.sh` now judges a draft placed into a copy of its
file as landing would, so those show as compile errors or refusals during the run, and a structure further down moves
above the block instead of refusing it. Of the 14 runs' drafts refused over a declaration or a structure, 3 then landed
as they were (`func_80013888`, `func_80014148`, `func_800161F8`), and the placed score shows each of the other 11 its
conflict (`func_80033C84`'s once its structure moved), in about 0.1 seconds. The loop takes again the 12 functions refused
at landing after runs that scored alone.

The equivalent executable in the game (`tools/run_game.py`, docs/WORKFLOW.md, "Running the game"; PCSX-Redux build
436): with its 76 equivalent functions it boots from the disc, reaches the title, starts a game, plays Cortex's speech
and Turtle Woods, Crash running and jumping, with no crash and no exception but interrupts and system calls over 8,000
frames. What it displays differs from retail's from frame 1,278 on (two retail runs agree on every frame): the intro's
flare elsewhere, Cortex missing from the warp room, Crash invisible in Turtle Woods. The C isn't the cause; where the
data sits is:

- Layout alone gives the same: with no equivalent function, 8 bytes of padding at the start of the code display otherwise
  from frame 1,390 and crash in the warp room (frame 4,124, a jump to address 0x68); 32 bytes crash at frame 1,941.
  The same paddings at the start of `.data`, the code in place, give the same frames and the same crashes; moving only
  the heap's start (16 bytes) changes nothing.
- With `.data` kept at its retail address (the code's end padded to 0x8005B198 by hand in the linker script: the 76
  blocks shrink the code by 104 bytes), the equivalent executable displays what retail does up to frame 4,035, and with
  all blocks but `func_80027CD0`'s on all 8,000 frames. That one, an LZSS decompressor whose C reads as equivalent,
  seems to change only timing: Cortex's speech and Crash in Turtle Woods are there, a vsync out of phase. The game is
  sensitive to timing: the retail executable under PCSX-Redux's interpreter instead of its recompiler displays
  otherwise from frame 2,004.
- What depends on the data's addresses is still open. No `.data` word and no `lui`/`%lo` pair is left at a retail
  address in the moved range (scanned in a shifted build), but values pointing into `.bss` come in from the disc: heap
  objects read by `CdGetSector` hold 0x8005F340 (inside `D_8005F338`'s 12 bytes), which the shift leaves as it is, and
  the disc image holds the word 21 times (docs/TARGET.md, "Addresses from the disc").

`configure.py` now keeps `.data`, `.sdata`, `.sbss` and `.bss` at their retail addresses in the equivalent build: the
code's end is padded to `.data`'s retail start, and `ninja nonmatching` stops, with the bytes over, should the equivalent
code outgrow the retail code. With the 144 equivalent functions then on `main` (the code 568 bytes shorter), the
equivalent executable stays healthy over the 8,000 frames and displays what retail does up to frame 4,035; without
`func_80027CD0`, the 143 others agree on all 8,000. That divergence is timing: up to frame 4,034 the two runs' data and
heap agree word for word, code pointers aside, but for a handful of 16-bit counters one apart (0xA40D against 0xA40E
at 0x8005B9C8 from frame 3,500 at least), the decompressor's output included.

Next: find what in the disc's data holds the executable's addresses, then `loop --equivalent` on the 12 refused at
landing and the remaining given-up functions, in batches, each checked in the game too.

## 5. Beyond matching

With the game in C, a native port (the way Crash Twinsanity's PS2 decomp got its PC port) becomes possible. Not before the
code is.
