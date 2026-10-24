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

1. **Readable C, R5 and R6**: the other subsystems' types and names, and the handwritten code's (section 5), where the
   port needs them first. The port itself goes on in its own repository (section 6, "Two repositories"), and what it
   needs of the C comes back here.

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
handwritten code's 21 `lui`/`ori` pairs loading the addresses of 20 of the game's functions (`Gool_OpDebug` to
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

With 155 equivalent functions (the code 1,572 bytes shorter), 154 agree with retail on all 8,000 frames, and
`func_80027CD0` alone still displays otherwise from frame 4,035; it changes timing, not behaviour:

- Hooked at its entry and its return in both runs (PCSX-Redux's interpreter, Lua execution breakpoints), its 160 calls
  over the 8,000 frames take the same arguments, the same input and ring buffer, and return the same length, output
  and ring buffer, call for call. Only when they return differs: 5 calls that cross a vsync in retail end within it
  in C, the first at frame 4,009.
- The stretched geometry the equivalent side shows in Turtle Woods (frame 7,350) is a frame retail displays too, every
  other pair of vsyncs (7,348-7,349 and 7,352-7,353); the equivalent run shows the same alternation two vsyncs out of
  phase. Pressing the buttons one or two frames later in retail moves Crash and the camera but leaves the phase as it
  is: the phase follows how long the game's frames take, here the decompressor's.

Later batches sent the untried functions of 0x240 bytes or more straight to `loop --equivalent`, and the 41
functions under 0x20 bytes were matched by hand. A regression over every equivalent function on a fresh seed caught
two that the landing's single seed had let through (`func_80013F5C` narrowed a call's argument to the callee's `s8`;
`func_80024A48` swapped two stores through pointers that may alias), so landing now checks two seeds. 115 equivalent
runs cost US$37.74 in all. With 163 equivalent functions, the equivalent executable agrees with retail on all 8,000
frames without `func_80027CD0`, and with it differs from frame 4,035 (timing, above).

equiv.py's random words made most dereferenced words bad pointers: in large functions most trials ended with both sides
taking the same fault, compared by their calls only, and the landing's 5% floor refused them (`func_8001F9C4`
dereferences two global pointers in a row: 13 of 2,000 trials compared). Traced, the faulting addresses came from global
pointers holding random words, arguments the C declares as integers but uses as pointers, callees' return values, and
indices loaded from memory, shifted and added to a table's address. equiv.py now repairs such a trial: it finds the
input word the faulting address came from and replaces it (an arena pointer, a small offset or count), then runs again,
both sides from the repaired state (docs/WORKFLOW.md, "Testing an equivalent function"). Compared trials of 2,000, seeds
1 and 2, before and after:

| Function | Before | After |
| --- | --- | --- |
| `func_8001F9C4` | 13, 5 | 233, 229 (most of the rest spin in a `for (;;) {}` behind an equality of two words) |
| `func_8002F6C8` | 34, 40 | 1,462, 1,503 |
| `func_80029538` | 62, 65 | 1,667, 1,634 |
| `func_8002433C` | 84, 96 | 1,591, 1,559 |
| the 163 equivalent functions | 448,500 of 652,000 (69%, median 74%) | 613,469 (94%, median 100%) |

All 163 still agree on every trial of both seeds. `func_80015344`'s "instruction Unicorn lacks" (above) was the retail
function rewriting its own code through a table index gone negative; the retail code is read-only now, and the
function runs: 941 and 906 compared, with one disagreement on seed 2 that a higher instruction limit removes (the
retail loop needs more instructions than the C's). Reusing one emulator per side instead of one per run, regions mapped
from this process's memory, makes a typical 500-trial run 1 to 2 seconds; functions whose repaired trials run long
take up to 25. An emulator that took a guest exception is rebuilt: run again, Unicorn 2.1.4 crashed the process now
and then (4 of the 163 functions on 2,000 trials).

A last `loop --equivalent` batch on the 12 largest untried functions landed 4 (`func_800334A0`, `func_800200A8`,
`func_80037698`, `func_80020A24`; 127 equivalent runs, US$47.99 in all): 167 equivalent functions, 36 left as assembly.
Of those 4, checked with the repairs, `func_800334A0` disagrees on 2 of 2,000 trials per seed: retail passes
`func_8001EC2C` a frame vector still holding another local's words (0x10000, 0x80), the C a zeroed one, and copies it
back after the call. `func_8001EC2C` writes all three words of that vector (`RotTrans`, then the shifts) before
anything reads them, so the difference is the stubbed callee's, not the C's: a false positive of "callees don't run".
A callee's outputs known to be written whole would let equiv.py fill them; until then the function stays as it is.

The four functions the 5% floor had refused (`func_8001F9C4`, `func_8002F6C8`, `func_80029538`, `func_8002433C`) landed
with their earlier drafts, unchanged: 2,000 of 2,000 trials agree on both seeds, 229 to 1,634 compared. 171 equivalent
functions, 32 left as assembly.

In the game, the 170 equivalents without `func_80027CD0` now display otherwise from frame 2,867: there the equivalent
executable shows retail's previous frame, by frame 3,150 it lags 15 to 17 frames, and the inputs then land elsewhere.
Two groups each agree with retail on all 8,000 frames on their own: the equivalents of the first 15 source files
(`800111B0.c` to `80023B04.c`), and those of the other 15. Disabling any one of the last 8 landed changes nothing. The
difference is their sum's timing, as with the decompressor (above); PCSX-Redux 436 has no CPU overclock that would hide
it.

A second `run --equivalent` on the 30 functions left without GTE instructions (Sonnet, US$10.13; 169 equivalent runs,
US$60.45 in all by the ledger), each retry told why its earlier draft wasn't landed, landed 7. Three more failed to
compile in place, matched C declaring them as data to pass their address (`extern char func_80019D30[];`): declared as
functions without a prototype, the SHA-1 holds and two landed. `func_80015344` landed once a trial where the retail run
reaches equiv.py's limit is left uncompared (docs/WORKFLOW.md). Five agreed on the agent's 500 trials and failed
landing's 2,000 on one to four trials of seeds 1 and 2: agents now run landing's check before stopping. 181 equivalent
functions, 22 left as assembly; in the game, as above, the two halves each agree with retail on all 8,000 frames and
together lag from frame 2,867.

A third pass on the 20 left without GTE (US$8.69; 189 equivalent runs, US$69.13 in all), agents now running landing's
check, landed 7. Two equiv.py changes landed 4 more from earlier drafts. A `static` helper in the candidate's file ran
as a call out, never relocated (`func_80029D0C`, `func_800158E0`); it now runs as the candidate's own code. And three
functions read, on paths the game may never take, registers their caller left (`func_80017380`'s `$s0` for an entry type
it doesn't handle, `func_80019098`'s `$s2`, `func_80027F3C`'s `$t0` when all three velocities are 0): a disagreeing
trial's retail code now runs again with those registers flipped, and a trial whose retail result changes isn't compared.
192 equivalent functions, 11 left as assembly, two with GTE instructions; in the game, the two halves each agree with
retail on all 8,000 frames.

A fourth pass (Sonnet, 9 functions, US$2.50) landed `func_8003981C`; one with Opus on the 8 left (US$11.35; 206
equivalent runs, US$82.97 in all) landed 6. Before it, equiv.py gave a parameter declared narrower than a word only
values of its type (`func_80019098`'s `u8`, which the retail code adds unmasked), and compared where two retail runs
stopped by the same address error faulted. Four of the six reproduce the retail code on paths a valid input never takes,
and so lean on GCC 2.7.2's layout: `func_800316EC`, `func_8002A8D0` and `func_80024658` keep their locals in one
structure laid out as retail's frame, which an index out of range writes into, and `func_8001F054` reads two locals left
unset, as the retail code reads its caller's `$s5` and `$s7`. A native port has to give those paths defined behaviour.
199 equivalent functions, 4 left: `func_8002CC50` (10 of 2,000 trials disagree), `func_8002FFFC` (agrees, 82 of 2,000
trials compared, under the 5% floor), and the two handwritten GTE routines `func_8001856C` and `func_800186D8` (`rtpt`,
`rtps`), which wait on how the C states the GTE: PsyQ's inline-assembly macros, or a GTE in C as a port needs. In the
game, the two halves each agree with retail on all 8,000 frames.

Running a callee's retail code instead of answering it, tried on `func_8002CC50` (whose made-up `func_8002FFFC` counts
run its loops past 12 points), took it from 10 to 1 of 2,000 trials disagreeing, but most trials then faulted inside the
callee, in random memory repairs don't reach (1,500 of 2,000 compared by their calls only), and the last disagreement
was again a frame word the two layouts place apart. Not kept; `func_800334A0`'s callee, the other candidate, has GTE
instructions.

The GTE's two routines, by hand (no agent runs): the GTE stated in C, as a native port needs it, rather than as PsyQ's
inline-assembly macros. `include/gte.h` models RTPS and RTPT from psx-spx, its register moves one instruction each;
`tools/gte.py` models the same for equiv.py, which now runs the GTE's instructions on it from a code hook, its registers
random on entry (in half the trials a projection the game would use) and compared after. Writing the model found a
maspsx bug: it writes ASPSX's `li $r,0` as an `ori` GNU as reads as `ori $r,$r,0` (docs/COMPILER.md, "Loading zero"),
which the 64-bit sum of the GTE's division hit; no C before had. Fixed upstream since (mkst/maspsx#156). `func_8001856C`
and `func_800186D8` (a box's textured quad and a glyph's shaded one, sorted into the ordering table by depth) agree on 2
seeds of 2,000 trials, about 30% of them projecting and drawing. In the game they can't run alone: with the model they
outgrow the retail code by 1,996 bytes, so they need equivalents that shrink beside them. Such a group, with and without
the two routines, displays the same on all 8,000 frames (`run_game.py --baseline`, added for it), and the same group
with them drawing nothing differs in 41 frames of the title's menu, so the game reaches them there. The group itself
diverges from retail from frame 3,435, as it does without them: its ten files together, `func_80027CD0` (the
decompressor shown above to change timing only) left out or not, while each of its parts with the six files the others
need agrees on all 8,000, and the scenes are the same a step out of phase, as timing that adds up gives. Another group's
divergence from frame 4,035 narrowed to `func_80027CD0` alone. The whole equivalent build diverges from frame 2,515
(2,867 before; 1,626 with the model's code in and the routines not), the frame moving with the layout. 201 equivalent
functions, 2 left; the equivalent build has 208 bytes of the retail code's room left.

`func_8002FFFC`'s C agreed but compared too few trials: it reads its six words of vertex indices from `.data`, and
equiv.py kept each data word's retail value at even odds, so one trial in 64 had the whole table. One trial in four now
keeps all the retail data (a regression over the 201 equivalent functions on a fresh seed: all agree but
`func_800334A0`'s known false positive, above); it lands with 573 of 2,000 trials compared. In the game, a group with it
agrees on all 8,000 frames, but the same group with it returning 0 does too: the schedule doesn't reach it, or nothing
shown depends on it. 202 equivalent functions, 1 left.

`func_8002CC50` disagreed only where the made-up count `func_8002FFFC` returned exceeded what it can return (it clips
6 quads, one segment each at most): its loops then read past its 12 points into frame words GCC places itself. equiv.py
now bounds a callee's return value where the callee's code bounds it (`CALLEE_RETURN_RANGES`, the user's call: a
caller's code for impossible values goes unchecked), and it lands with all 2,000 trials of each seed compared. In the
game a group with it agrees on all 8,000 frames, but so does the same group with it returning at once: the schedule
doesn't reach it either. With it, every function GCC compiled has C: 263 matched, 203 equivalent (134,080 bytes,
80.39%; the earlier counts here summed sizes that overstated it by 264 bytes). The equivalent build has 32 bytes of the
retail code's room left.

What held the data in place isn't the disc: its `.NSF` and `.NSD` files hold no address of the executable, and the
0x8005F340 the heap held is vertex bits of one animation entry (docs/TARGET.md, "Addresses from the disc"). It is the
handwritten code's `$gp`, a pointer to its structures, whose offsets splat wrote 23 times as `%gp_rel` of small data,
so that the linker moved them with `.data`. With those 23 as their numbers, the default build's C with `.data` 8 bytes,
32 bytes or 1 KB later displays what it does at the retail layout on all 8,000 frames, and so does the equivalent build
with `.data` right after its code. The executable's file on the disc then bounds it: 1,372 bytes of room after the
retail image's data.

The equivalent build now does so (the user's call): its copy of the handwritten code holds the 23 offsets as numbers,
its `.data` follows the code (32 bytes earlier than retail's, at 0x8005B178), and `ninja nonmatching` stops only when
the executable outgrows the retail file, with 1,392 bytes of room left now (the image's end rounded to 16 bytes). It is
healthy over the 8,000 frames and displays what the same C at the retail layout does on all of them, and what retail
does up to frame 4,035, as before.

Two retail runs stopped agreeing on every frame: the emulated pads read the host's gamepads, a press the host reported
reaching the game's pad buffer at a different frame each run. With the pads set to the keyboard (`tools/run_game.lua`),
retail agrees with itself on all 8,000 frames again, and the equivalent build, run twice, gives the same trace both
times (docs/WORKFLOW.md, "Running the game").

`tools/run_game.py` now plays on the game clock by default (docs/WORKFLOW.md, "Running the game"): the game measures
each frame with `g_TimerTicks` and moves its objects by it, so the script freezes that counter's interrupt-driven
increments and sets it to 34 ticks per game frame on entering `Gfx_EndFrame`, and keys presses and images to game frames
(`Gfx_EndFrame`'s calls). The schedule moved to game frames, found from screenshots of a retail run on the clock: New
Game at 1,050, Turtle Woods' portal at 2,750, its first steps from 2,950, 3,450 game frames in all. The whole
equivalent build then displays retail's image at all 3,451 game frames, 6 vsyncs behind at the end, where in real time
it differs from vsync 4,035 on. `func_80027CD0`'s equivalent alone, which in real time differs from vsync 4,035 too,
agrees on all 2,937 game frames of a run through the attract demo (below). `--real-time` keeps the old schedule.

Left alone at the title's menu, the game plays its attract demo, and there the equivalent build differs: from game frame
2,071 by a 16 by 17 pixel area of the image, and it stops at game frame 2,200 on a read from 0x16E1F088. Split by file,
then by block, on the game clock:

- The difference is `func_80028C48`'s equivalent (`src/game/80026CA0.c`), a search through the zone's entries and the
  objects' boxes around a point, alone of its file's 13 blocks.
- The stop is `func_800328D4`'s (`src/game/800324A8.c`), which picks an SPU voice.

The schedule shows no difference from either. Both were C that differs from the assembly where `tools/equiv.py`'s
trials rarely or never look:

- `func_80028C48` keeps its best object's height when it is at least the zone's result (`!(bestd < arg[1])`, the
  retail `slt`); its C had the comparison the other way (`!(bestd > arg[1])`), a path random trials seldom reach (an
  object list, a best candidate, a zone result and two unequal heights at once).
- `func_800328D4` passes the SDK code's `func_800521A8`, which reads a voice's attributes, a structure on its stack that
  the callee fills up to offset 0x3E: 0x40 bytes, the retail frame's room for it. Its C declared 0x34, so the call wrote
  12 bytes past it into the equivalent function's frame. equiv.py answers calls instead of running them, so it never saw
  the write. (The structure's word at offset 4, which neither side sets, is not the cause: `func_800521A8` never reads
  it.)

Both fixed, each agrees on 2 seeds of 2,000 trials, and `tools/run_game.py --schedule demo` (the second schedule:
nothing pressed, the attract demo through Turtle Woods and a river level, 4,800 game frames, retail reaching the last by
vsync 14,130) shows the equivalent build displaying retail's image at all 4,801 game frames, 88 vsyncs behind at the
end; the new-game schedule still agrees at all 3,451. Retail agrees with itself on both.

A third schedule, `--schedule turtle-woods`, runs on into Turtle Woods blindly through deaths and respawns, 6,000 game
frames, retail reaching the last by vsync 14,068. The equivalent build differs there from game frame 3,524, and the C is
not the cause: a handwritten renderer, `func_80042818`, reads its depth's texture shift past a 32-byte table into the
scratchpad's copy of the GOOL opcode table, the bytes of the handwritten code's addresses, which the equivalent build
places 0x14 bytes earlier (docs/TARGET.md, "Code addresses read as data"). With retail's word put back around that one
read, the shifted build displays retail's image at all 6,001 game frames. So the equivalent build agrees with retail on
this schedule only once the handwritten GOOL operations keep their retail addresses (or their low bytes), or once a C
rewrite of the renderer reads the bytes retail does.

A fourth, `--schedule memory-card`, presses through Load Game, Load/Save, Save Game and Enter Name blindly and saves on
the empty card, 3,500 game frames. Unpaced the equivalent build differs from game frame 1,464: split by file then by
block, `func_800186D8`'s equivalent (the menu's quads) alone does it, by taking 4 vsyncs a game frame on the card's
screen where retail takes 3, so the card, which answers in real time, ends its directory read 50 game frames earlier.
Paced (each game frame from 1,100 to 3,000 lasting 10 vsyncs in both runs, docs/WORKFLOW.md) the equivalent build
displays retail's image at all 3,501 game frames, as retail does against itself, and the new-game and demo schedules
still agree at every frame. The two saves differ by 12 bytes, a random number the game draws while waiting for the card
and puts in the save's name; retail's two runs save the same bytes.

Later, with the matches landed since, the equivalent build's handwritten GOOL operations sit at their retail addresses
again (the 12 opcode table entries that renderer reads are the same in both executables), and the turtle-woods
schedule differs from game frame 3,666 instead, 142 game frames later; that difference has another cause, not looked
into yet.

The handwritten code gets equivalent C too (docs/WORKFLOW.md, "The handwritten code"): `src/game/80039D2C.c`, compiled
by the equivalent build only, whose functions `configure.py` leaves out of that build's copy of the handwritten code.
The first six are the ones compiled code calls with the usual convention and nothing handwritten relies on: the sine
and cosine, the two sums of squares, the integer square root and an empty function the renderers' table at
0x8005B75C points to (its C compiles to the same `jr $ra`). Each agrees on 2 seeds of 2,000 trials; the new-game, demo
and memory-card schedules agree with retail at every frame, and turtle-woods with the equivalent build without them at
every frame. Two were tried and set aside:

- `Sys_Memcpy`: its retail code jumps into code for a negative count, which random trials pass (60% of them disagreed).
  It went in later, under a contract (below).
- `Gfx_ResetOt`: it agrees on every trial, but leaves `$v0` alone, which its handwritten caller returns (docs/TARGET.md,
  "What a caller keeps"). Its prototype is now `void`.

Trying them showed the names of `Sys_Memcpy`'s tails the wrong way round (docs/TARGET.md, "Tails after their
function"), now corrected. `tools/progress.py` counts the handwritten functions apart: 6 of 98 equivalent.

equiv.py then took a function's contract with its callers (docs/WORKFLOW.md, "A function's contract"): lines of
nothing in the build that keep the trials to what the game passes, each with its evidence beside it, printed with the
verdict and refused by `tools/match.py` in a run's C: `EQUIV_RANGE`, `EQUIV_NONNULL`, `EQUIV_DISJOINT`, `EQUIV_SCRATCH`
(the handwritten code's save area, the scratchpad's first 16 words, which every user writes before reading) and
`EQUIV_RUNS` (a routine the retail code branches into, still in the build for its other callers), and
`HANDWRITTEN_PARTS` for the tails only one function reaches, which go with it. Without a contract equiv.py draws as
before. Three more went in with them: `Sys_Memcpy` (counts 0 to 0xFFFF, buffers never NULL nor overlapping: 2,000 of
2,000 trials on 2 seeds, all compared), `func_8003DA00`, which descends an octree to the leaf holding a point (its save
area), and `Zone_PlotWallEntry`, which marks a cell of a scratchpad grid and ORs a shape into its rows (it runs
`Zone_PlotWall`, which `Zone_PlotQueryWalls` still calls). `Zone_PlotWallEntry`'s prototype is now `void Zone_PlotWallEntry();`: the
matched `func_8002CB1C` calls it with no arguments, passing its own on in `$a0` and `$a1`. The new-game, demo and
memory-card schedules agree with retail at every frame with all nine. `tools/progress.py`: 13 of 98 handwritten
functions equivalent, `Sys_Memcpy`'s four tails counted.

Looked at and left in assembly: `func_8003D040` (a box query over records whose sizes come from headers; a box before
any header uses sizes left in `$s4`-`$t9` by its caller, 1 trial in 2,000, so it waits for the records' producer, likely
`Zone_QueryOctree`); `Gfx_DrawObjects` and `func_80041D14` (a driver calling renderers with `$v1`, `$fp` and `$gp` set,
and a callback stored for one); `func_8003EAD4` (a second entry, `D_8003EBC4`, in its middle, which `func_8003FD14` and
the renderers' table at 0x8005B6EC reach). The GOOL interpreter (`Gool_Interpret` and 10 routines, 0x1128 bytes) holds
its opcode handlers as labels the data's opcode table points to, the table `func_80042818` also reads as bytes.

The turtle-woods schedule then differed from game frame 3,666 in the equivalent build without handwritten C, and from
4,429 with it, whether or not the handwritten code kept its retail addresses. Bisected by file with the handwritten
code and the data pinned at their retail addresses, every half of the equivalent files gave the same 3,666 and the
same 2,335 differing frames, and the build with them all as assembly agreed: no one function. `tools/run_game.py
--dump` (RAM at chosen game frames) showed the runs apart by game frame 3,400 in counters of vsyncs and in a block at
`D_8005CC18` + 0xE94 that looks like the sound's: the level's sound and disc reads go on in real time, so a build of
another speed plays otherwise, as the memory card did. Paced from game frame 2,900 to 6,000 (5 vsyncs each), retail
agrees with itself, and the equivalent build with retail at all 6,001 game frames, without handwritten C and with it.

The executable may now outgrow the retail file: `tools/patch_disc.py` writes it after the disc image's last sector and
points the directory record and the volume's size to it (the BIOS finds it by name, no other file moves), checked by a
copy 64 KB longer, which plays the new-game schedule as the build does (its header still loads the build's size, so
the heap stayed as it was). A longer build leaves the heap, from the end of `.bss` to the stack, as much smaller:
`configure.py` stops past 64 KB over (`MAX_GROWTH`), a limit to raise once a build that long has played every
schedule.

Three more, with what they needed. The collision lists `func_80029538` builds start each zone's part with a header
`Zone_QueryOctree` writes first, so a box never comes before one; equiv.py now also runs a disagreeing trial's retail code
with the registers it didn't set zeroed (flipped ones gave the same box), which leaves `func_8003D040`'s headerless
trials uncompared, and it agrees on 2,000 of 2,000 on 2 seeds. `Zone_PlotQueryWalls`, which marks the boxes' edges in the
scratchpad's grid, went in with `Zone_PlotWall`, its routine and `Zone_PlotWallEntry`'s, now C both share. `func_8003D3CC`
sets the GTE up to draw an object (its translation, culled by depth, and a rotation from its angles times its scale)
through `mvmva`, which `tools/gte.py` and `include/gte.h` gained (`tools/gte.py` `nclip`, `dpcs` and `dpct` too, the
handwritten code's other commands); its retail code puts a `ctc2` in a delay slot, which equiv.py now runs. A draft
with its camera correction off by one disagrees on 110 trials: the path is tried. The handwritten file's `.rodata` (an
initialiser's zeros, the GTE model's constants) had moved all the code after it by up to 0x34 bytes, the GOOL
operations among them, and turtle-woods differed from game frame 3,526; linked after the file's code instead, nothing
before it moves, and `ninja nonmatching` says when the handwritten code isn't at its retail address. The executable is
then 64 bytes past the retail file: written after the disc image's last sector, with the heap that much smaller, it
displays retail's image on all four schedules at every frame. `tools/progress.py`: 17 of 98 handwritten functions
equivalent.

`Zone_FindFloorY`, which averages the tops of a collision list's boxes under a height, asking a callback about some,
needed two more things of equiv.py: `EQUIV_CALLBACK` (its one callback, `func_8002A1B8`, takes two arguments and writes
neither `$t8` nor `$t9`, which the retail code keeps a header's halvings in across the call), and a frame address told
only while `$sp` is the stack's (the retail code keeps the origin's x in it). Its C writes the scratchpad words the
retail code keeps its state in around the call, 0x1F800070 to 0x1F8000A3 among them, the opcode table's copy. It agrees
on 2,000 of 2,000 trials on 2 seeds; every equivalent function (209) still agrees on 500 trials of seed 1, and the four
schedules at every frame, the executable 240 bytes past the retail file. 18 of 98 handwritten functions equivalent.

The sort lists' deltas (docs/TARGET.md, "The rest of the disc"): `Gfx_DecodeSortListForward` applies one forward, its two streams'
runs removed and inserted into the other buffer, then its swaps; `Gfx_DecodeSortListBackward` backward, its swaps undone last first,
then the streams' roles exchanged. One C merge serves both, the tails `func_8003B89C` to `func_8003B91C` (the copy's
unrolled ends) going in with them. Random trials needed what the disc shows: lists that fit `Gfx_AllocSortLists`'s
0xBE4-byte buffers, deltas that fit their page, and the three buffers apart (`EQUIV_BUFFER`, `EQUIV_HALF`), else the
copy runs over the delta it reads and the two sides read it at different times. A near swap's base where no short or
long swap comes before is a register's leftover (the caller's, the copy's jump target, a flipped entry's address),
which no delta on the disc reaches: `EQUIV_ASSUME`, a condition in the C's body that ends a trial uncompared. The
backward copy by halfwords runs past the retail code's instruction limit where its unrolled word copy doesn't: the
candidate's limit is now four times the retail code's. Both agree on 2,000 of 2,000 trials on 2 seeds, about 45%
compared (the rest of random deltas loop to the limit); every equivalent function (211) still agrees on 500 trials
of seed 1, and the four schedules at every frame, the executable within the retail file again. 25 of 98 handwritten
functions equivalent.

`Zone_QueryOctree` writes a zone's part of a collision list: its header, then a box for each leaf of the zone's octree
that an object's box overlaps (docs/TARGET.md, "The rest of the disc"). The retail code walks the tree with the nodes
above in 7 registers and resumes each through two jump tables; the C recurses, a node's children in index order (x the
highest bit), each visited where the box reaches its half along every axis the level splits. Its contract bounds each
axis's halvings at 7, what the retail code's registers hold and the disc's most; `EQUIV_ASSUME` leaves out a node with
children where no axis splits, on which the retail code loops forever and which no zone has. It agrees on 2,000 of
2,000 trials on 2 seeds (about 83% compared); two slips, an overlap test off at its bound and the children's bits
reversed, disagree on 1 and 7 of 300. The four schedules agree at every frame. 26 of 98 handwritten functions
equivalent.

`Gfx_DrawStreaks` draws a scene's streaks (docs/TARGET.md, "The rest of the disc"): points of a box about the view,
wrapped, from the same generator's bytes every frame, each a shaded line from where the point was from the view's last
position to where it is from the current one, projected with RTPT, culled when its ends wrap apart or lie past one side
of the screen, and all linked into one slot of the ordering table before a closing draw mode. Its count is bounded by
the disc's, 1 to 1,500. Three things changed in equiv.py and the build for it. The C's instruction limit grows by 4,000
for each GTE command the retail run gave, since the model takes about 2,300 for an RTPT (1,017 streaks: 2,428,386
instructions to the retail code's 73,118). No word drawn from the code's constants points into a scratch area: a frame's
next primitive at 0x1F800000 had the retail code's lines overwrite the `$sp` it saves there. And the equivalent build
runs each GTE command as the GTE's instruction, equiv.py's candidate on the model (`GTE_MODEL`, docs/WORKFLOW.md, "The
GTE in C"): on the model the build showed the opening's game frame 846 at vsync 11,994 where retail did at 2,721, and
the model's code cost each file including it about 1,800 bytes. Without it `src/game/800158E0.c` came out 0x794 bytes
shorter and took the handwritten code that much below its retail address, and turtle-woods differed from game frame
3,526 in 11 frames, as when the GOOL operations' addresses moved before: the equivalent build's linker script now pads
the code before the handwritten copy to retail's address when it is shorter (docs/WORKFLOW.md, "The handwritten code").
The executable has 5,008 bytes of the retail file's room left (2,352 before). It agrees on 2,000 of 2,000 trials on 2
seeds (about 80% compared); without the cull past the screen's side or without the wrap check along z it disagrees on 22
and 67 of 300. Every equivalent function (213) still agrees on 500 trials of seed 1, and the four schedules at every
frame. 27 of 98 handwritten functions equivalent.

`func_80045218` draws a scene's dots (property 0x2AA's, docs/TARGET.md): the streaks' generator again, each point a 1x1
tile of a random grey, projected with RTPS, its x and z kept 0x70 from the view (x only from above, as the retail code
masks it to a halfword first), culled past the screen's bottom or right, all in one slot before a closing texture page.
Its count is bounded by the disc's, 1,000. Its extent as splat saw it ran on past its return into a routine of its own,
whose address two renderers store: `func_800454A4` now, the handwritten code 99 functions. It agrees on 2,000 of 2,000
trials on 2 seeds (about 95% compared); the gap test off at its bound or no cull past the right side disagree on 41 and
78 of 300. The four schedules agree at every frame. 28 of 99 handwritten functions equivalent.

The remaining entries are GTE renderers and drivers; with the GTE's own commands in the build their C need not be
larger than their assembly (`func_80045218`'s is 48 bytes smaller).

The world renderer (docs/TARGET.md, "The world renderer"): `Gfx_DrawWorlds` walks the zone's sort list from its last
entry, each a polygon of one of the worlds `Sys_MainLoop` copies into the scratchpad from 0x280, loads its vertices into
the GTE and jumps to the scene's projector, which runs RTPT while it decodes the next polygon, culls, picks the ordering
table's slot by depth and jumps to the scene's drawer, which looks the texture up, writes the primitive, links it into
the table built from the top down and jumps back into the dispatcher's loop. All of it shares registers, so it went into
C as one group of 43 handwritten functions: the dispatcher, the 10 modes' five entries (`func_80011E24`'s default and
`func_80020A24`'s 9: fog, depth fog, tint, pulse, lamps, glow, waves, sway, and the light around Crash) and the routines
they reach. Steps: the modes' 32 entries split into functions of their own (`config/symbol_addrs.txt`, `func_8004400C`
too, an object renderers' routine past a drawer; 132 functions); equiv.py's contract gained `EQUIV_WORDS` (rows of words
an argument points to: the scene's mode) and `EQUIV_KEEPS` (a callee's kept registers), and a group's parts the C
defines run as its own where the C calls their retail addresses; the GTE model and the build gained DPCS and DPCL
(`tools/gte.py` too); the C written and judged mode by mode, then landed whole. In C the entries are functions taking a
`WorldPass`, the retail code's registers, and the scratchpad's words keep the retail code's state (`WorldScratch`, its
offsets checked as it compiles). Four things the random trials found: a texture's words read before the primitive over
them is written, and the next polygon's record read after the current one's first colour, as the retail code does (in a
trial, records and primitives may share memory); and the glowing mode's first corner's colour put in the scratchpad
before the lamps are tried, the polygon black or not; and the three arguments apart (`EQUIV_BUFFER`, the sort list and
the frame heap buffers, the scene a global), since a list inside the scene had its count's bound rewrite a mode's
pointer. It agrees on 2,000 of 2,000 trials on 2 seeds (about 52% compared). The light's path barely runs in random
trials (the player and the worlds' positions are random words); a run with them at 0 and its maps' and grid's buffers in
the arena, outside equiv.py, takes it: the C agrees on 1,000 of 1,000 on 2 seeds, while one with the base triangle a
slot off disagrees on 26 of 1,000. In the game the C renderer is slower (3 vsyncs a game frame where retail takes 2,
after its loops' locals were kept in registers): Cortex's lips, which follow his speech's sound in real time, moved
otherwise from game frame 1,380, so new-game and turtle-woods are paced from game frame 1,100 and memory-card to its
end; then the four schedules agree at every frame. The executable has 3,296 bytes of the retail file's room left. 71 of
132 handwritten functions equivalent.

Four small handwritten functions: `Gfx_ResetOt` (an ordering table's slots each linked to the next, the last ended),
`func_8003BC34` (a halfword copy), `Sys_TimerHandler` (root counter 2's interrupt, the game's ticks) and `func_8003EAD4`
(vectors shifted down, rotated by the GTE, shifted back and moved: a box's corners for `func_8002A8D0` and
`func_8002CC50`), its entry `func_8003EBC4` split into a function of its own (133 functions). Each agrees on 2,000 of
2,000 trials on 2 seeds, with the counts the callers pass bounded (`EQUIV_RANGE`: a table's slots, the box's eight
corners). `Sys_TimerHandler`'s trials agree only on its store faulting at the same address: its counter, at 0x8003BEA4,
lies in the handwritten code's pages, which equiv.py keeps read-only; the schedules run it. The four schedules agree
at every frame. The executable has 3,696 bytes of the retail file's room left. 75 of 133 handwritten functions
equivalent.

The object renderers' matrix builders: `func_8004400C` and the three entries after it (`func_80044178`,
`func_8004427C`, `func_80044334`), which `func_8001AA80` calls through the table at 0x8005B650 with an object's
matrix, angles, scale and factors, split into functions of their own (with `func_800443EC`, a packet's first step; 137
functions). Each turns the GTE's rotation by the angles in its own order (z then x; y, x, then y by the y angle less
the z; y, x, then z; z, x, then y) and scales it, unless the scale's words or together to 1.0. The C stands for the
routines they share with the other object renderers (`func_80043270`'s cosine and sine, `func_80042FD0`'s product),
which stay for those; it reads the sine table as `func_80043270` does, a value it doesn't negate as the halfword
loads it, since the trials' table holds random words. `func_80044178`'s code from 0x80044238, its angles negated, has
no caller. Each agrees on 2,000 of 2,000 trials on 2 seeds (`EQUIV_RUNS` for the routines they jump into); the four
schedules agree at every frame. The C is 644 bytes larger than the code it replaces: the executable has 3,056 bytes
of the retail file's room left. 79 of 137 handwritten functions equivalent.

The object renderers (docs/TARGET.md, "The object renderers"): `Gfx_DrawObjects` and the 81 routines its packets reach,
in C as one group (`DrawPass`, `DrawScratch`, `DrawPacket`), their entries split into functions of their own first (172
handwritten functions; configure.py's count of `%gp_rel` accesses follows splat's). The GTE model and the build gained
NCLIP and DPCT. The random trials keep to the packets the game builds (`EQUIV_WORDS` rows from the tables at 0x8005B690
to 0x8005B7AC; a sprite's steps assumed with the sprites' drawers, a model's with the models', compared by the retail
addresses the rows hold) and to what never traps in the game (`EQUIV_ASSUME`: the retail code's `addi`, `add` and `neg`
trap on overflow; a model's two frames align alike): 2,000 of 2,000 trials agree on 2 seeds. They reach the sprites and
the steps; models only with their data built behind the packet, which a copy of equiv.py does (valid strips that end,
cache and colour indices in the scratchpad, a camera and object in view, the scene's lamps near the object): the C
agrees on 1,000 of 1,000 on 2 seeds, kind 5's sprites on as many, and a mutation in each drawer is caught. What the
trials found: the packet's command read per triangle; records shifting only their coordinates; `$a2`, the texture's
offset, left by the colour routines (the screen points and-ed, the facing) for the next call; the clipped pieces' first
corner's colour the triangle's own in `func_800454A4`; the sine table read as `func_80043270` reads it; the unaligned
vertex bytes read by words; and equiv.py's candidate code, a primitive's random address able to overwrite it, kept
read-only. The four schedules agree at every frame; the C draws slower (the demo's 4,800 game frames by vsync 22,628,
retail's by 14,130, its hot helpers inline; 26,246 without), so memory-card and demo run longer. A kind 5 list's first
two loads keep their order (a compiler barrier, where a wild pointer faults), and a colour routine culling off the
screen's top or bottom leaves the and-ed points in `$a2`, off its sides not. The executable is 3,344 bytes longer than
the retail file, the heap that much smaller. 161 of 172 handwritten functions equivalent.

The GOOL interpreter (docs/TARGET.md, "The GOOL interpreter"): `Gool_Interpret` and the 10 routines its operations
share, in C as one group: the machine its registers hold (`GoolVm`), the operands' decoders kept as functions of their
own (inline at each use, the interpreter took 27,888 bytes), and a `switch` over the 79 opcodes in place of the table,
whose entries keep the retail operations' addresses (the C defines their labels there), so the scratchpad's copy that
the world renderer reads past its own table is retail's wherever the code lies, and configure.py's padding of the
handwritten code to its retail address is gone. Operation 73's native code (8,572 blocks in the disc's GOOL entries,
read entry by entry) runs through an inline call with the registers it reads; a division by 0 gets the CPU's results,
where GCC's code traps. equiv.py gained `EQUIV_TABLE` (the scratchpad's word 0x5C points to the opcode table, its words
retail's): its random trials of random words agree, nearly all ended by the C's assumptions (an opcode past the table,
native code). A copy of equiv.py that builds a machine behind each trial's object (valid instructions that end,
subroutines, links, frames in the object, native blocks of the disc's forms after operation 73, run on both sides)
agrees on 2,000 of 2,000 trials on 2 seeds, and catches a mutation in each opcode's case and in each helper's result but
those of a zero divisor and of the lowest word by -1, which they don't reach. What the trials found: GCC 2.7.2 leaves
`$fp` unsaved around the native call (docs/COMPILER.md). The four schedules agree at every frame, turtle-woods' far rain
included, at nearly the speed they had (new-game's 3,450 game frames by vsync 16,979, 3 more; the demo's by 22,725, 97
more). The executable is 4,416 bytes longer than the retail file, the heap that much smaller. 172 of 172 handwritten
functions equivalent.

Next: the handwritten code's C made readable with the rest (section 5); matching goes on in the loop.

## 5. Readable C (in progress)

The C proven equal to the game, matched or equivalent, still names nearly everything by its address. This milestone
gives it names and types without moving a byte: matched C keeps the retail SHA-1, and a batch of renames or types leaves
`build/nonmatching/SCUS_941.54.exe` byte for byte as it was (two clean `ninja nonmatching` builds give one SHA-1), so
neither `tools/equiv.py` nor `tools/run_game.py` has to run again. A batch that changes it runs equiv.py (2 seeds of
2,000 trials) on the functions it changed, and run_game.py.

How (docs/WORKFLOW.md, "Naming"): names in `config/symbol_addrs.txt`, each with its evidence, given by
`tools/rename.py`; shared types in `include/game/<subsystem>.h`, included by `include/game.h`, named with their subsystem
(`GoolObject`, `NsfEntry`); a name only on evidence of grade A (an SDK call, a string, a format constant, what
`tools/run_game.py` shows) or B and C (the same use by two independent callers, and a reference's note the assembly
confirms). The matching loop's records stay keyed by the address name.

Steps, in order:

- **R0, the tools (done).** `tools/symbols.py` reads splat's symbol table (`.splat/splat_symbols.csv`): each name's
  address and kind (game function, SDK function, data, label), for `configure.py`, `tools/match.py`, `tools/rename.py`
  and `tools/progress.py`. `tools/match.py` takes any name the table gives a game function or data where it took
  `func_`/`D_` names, tells an SDK function by its address, keeps `include/game.h`'s prototypes in address order, lets
  the shared headers' types into `include/game.h`'s prototypes and refuses a candidate defining one otherwise, and finds
  a function by either name; its records stay keyed by the address name. On the tree without names, `next`,
  `next --equivalent` and `report` print what they did before, and its patterns find in the 509 C files of `src/`, the
  notes and `include/game.h` exactly what the old ones did. `tools/rename.py` names a batch and keeps it only when both
  builds hold. Proven by a scratch batch of six, dropped afterwards: an equivalent function, a matched one, one still
  plain `INCLUDE_ASM` (what `next` offers), one the handwritten code loads with `lui`/`ori`, an `.sdata` variable (its
  line's `size:` kept) and an `.sbss` one. Both builds held, the equivalent executable stayed byte for byte, `next`
  and `next --equivalent` offered the new name, `report` printed the same, and `land --equivalent` of two landed
  functions whose notes' C calls the old name stayed a no-op; a second batch corrected two names and their evidence.
  A build failure injected after the rename put every file back. `tools/progress.py` starts at 0 of 564 game functions and 0 of
  1,112 data symbols named, 127 structures in `src/game/`, 192 fields read at a raw offset.
- **R1, anchor names (done).** 69 names of grade A, each with its evidence in `config/symbol_addrs.txt`: 56 game
  functions and 13 data symbols, the equivalent executable byte for byte as before. `main` and the system's init, game
  loop, shutdown and timer (`Sys_`), the CD reads (`Cd_`, the two prefixes added for them), the heap, the graphics init,
  frame end and buffers, the pads' init, the two random generators, the sine and cosine (the table checked against
  round(4096 sin)), the sound and sequencer calls, the memory card's, and the NSF's EIDs, file names, level open and
  close, page reads and relocation. `main` needed its C's explicit `__main()` call dropped (docs/COMPILER.md, "`main`").
  Left for R2 and R3, without a grade A proof yet: the GOOL interpreter's and its operands' entry points, the player's
  pointer; left as code reading only: the heap's init and growth, the game loop's exit flag.
- **R2, `GoolObject` (done).** The game object's one structure, `include/game/object.h`, in place of 17 partial
  structures and most raw offset reads in 21 files; every field `unk_<offset>` with the width and signedness the code
  reads, no name proven yet. The retail SHA-1 holds and the equivalent executable is byte for byte as before; where a
  member moved code (docs/COMPILER.md, "Structure members and plain pointers") the access stays raw with a comment.
  Left: the types 0x08, 0x0C and 0x18 point to (R3), the tree nodes of `800158E0.c`, which serve both 8-byte list heads
  and objects, and about 800 accesses through equivalent C's offset macros (`FIELD`, `AT`, ...), part of rewriting
  equivalent C for readability. `tools/progress.py`: 155 structures in `src/game/`, 1 in `include/game/`, 155 fields
  read at a raw offset (172 and 240 before).
- **R3, the NSF types (done).** `include/game/nsf.h`: the page (`NsfPage`, the magic 0x1234 the relocation checks, the
  texture pages `LoadImage` takes whole), the entry (`NsfEntry`: its EID, the type `Nsf_EntryType` reads, its items),
  the NSD (`Nsd`, `NsdRecord`: 256 buckets by `(EID >> 15) & 0xFF`, the records from 0x520), the paging slots and the
  level state at 0x80067588 (`NsfPageSlot`, `NsfLevel`), and the layout's constants; `GoolObject`'s 0x0C and 0x18 are
  entries. 28 local structures fewer, used by matched and equivalent functions of 14 files, the SHA-1 and the equivalent
  executable as before; raw where the spelling moves code (docs/COMPILER.md, "Address arithmetic"). Left: the level
  header at `NsfLevel.unk_14`, `g_LevelFiles`' bitfields, globals that are views of one structure (joining them changes
  code). `tools/progress.py`: 127 structures in `src/game/`, 7 in `include/game/`, 114 fields read at a raw offset.
- **R4 (done before R1 to R3)**: the equivalent passes on the functions left as assembly (section 4); none is left.
- **R5, the other subsystems (in progress).** A first batch: 47 game functions, 11 data symbols and 22 of `GoolObject`'s
  fields, for the GOOL interpreter, the objects and the zones. The table of handlers by entry type at 0x8005B1D8 carries
  each type's four-character tag ("ZDAT", "OPAT", "GOOL", ...) beside its handlers for a level's open and close and a
  page's load and drop, so those handlers' names are of grade A; the interpreter's loop (`Gool_Interpret`, handwritten)
  dispatches through the 79-entry table at 0x8005C514 by an instruction's top byte, each entry a stub naming its C
  handler, so a handler takes its opcode's mnemonic (CrashEdit's list, the reference). `GoolObject`'s links at 0x40
  (self, parent, sibling, child, creator, player, collider, interrupter) are an enum. A second batch: 17 game
  functions, 14 data symbols, `NsfPageSlot`'s state and reference count, and `include/game/entity.h`, for the entity
  properties, the spawn queue, the paging's reference counts and the camera. An entity's properties are laid out as
  CrashEdit reads them (8-byte headers sorted by id from 0x10, sparse rows, meta values, a row count), which
  `Ent_GetProperty` searches; the ids the code passes it (positions 0x4B, SLST 0x103, draw lists 0x13B and 0x13C, load
  lists 0x208 and 0x209) are format constants that name the camera's walk along a path (`Cam_MoveAlongPath`,
  `Zone_QueueDrawLists`, `Zone_ApplyLoadLists`). The opcodes CHNK and COLL name their handlers, CHNK's kinds the
  entries' opening and closing (`Nsf_OpenEntry`, `Nsf_CloseEntry`); the Crash 1 decomp's slot functions, the same
  code, confirm the page slots' count and release. The camera's functions then took a prefix of their own, `Cam_`
  (`Cam_MoveAlongPath`, `Cam_Update`, `Cam_GetPathPoint`); the view stays `Gfx_`. Left without the evidence: the collision routines COLL calls (CrashEdit describes none of
  its modes), the page functions with one caller (func_80013F5C, func_80013FD8, func_80014078, func_8001456C,
  func_80014918), func_80014148, the camera's handlers by path kind and by mode. `tools/progress.py`: 120 of 564 game
  functions and 38 of 1,112 data symbols named, 131 structures in `src/game/`, 8 in `include/game/`. A third batch: 40
  game functions. Three debug messages carry the camera's own function names ("CamProcessMessage()",
  "CamPullForwardBothDirs()", "CamPushBackBothDirs()"), and the queue the first empties gives its producer and consumer
  their names (`Cam_QueueMessage`, `Cam_ProcessMessages`). The entry-type table's records are 0x1C bytes: the tag, then
  handlers at 0x8 (Nsf_OpenLevel's start), 0xC (its end), 0x10 (page in), 0x14 (page out) and 0x18 (level close); 13
  more handlers take their record's role (NONE's frame buffers, TGEO's and WGEO's page-in, SLST's sort lists, CPAT's
  camera reset, ADIO's `Snd_Shutdown`, PBAK's playback, VIDO's video), and the two functions calling them per entry are
  `Nsf_PageIn` and `Nsf_PageOut`, with the slot update that prints "ERROR: frame retry loading %s" by `Nsf_PageRefName`
  ("p%d", "[%X]"). By their SDK calls: the view's transforms (`Gfx_WorldToView`, `Gfx_WorldToScreen`, `Gfx_ResetView`,
  `Gfx_DrawFade`, `Math_MatrixFromAngles`, `Math_AngleToXZ`), the memory card's state machine (`Save_Update` and its
  steps by `_card_info`, `_card_load`, open and read, and "BASCUS-94154"), and the sound's play, voice allocation and
  parameters (GOOL's SNDP and SNDA) and per-frame update. Left without the evidence: BINF's and OPAT's handlers, the
  TGEO and WGEO page-in workers, the save writer (func_80035444) and slot commands (func_8003670C), the VRAM transfers
  (func_800176DC). `tools/progress.py`: 160 of 564 game functions named. A fourth batch: 11 game functions, 11 data
  symbols and two types. The memory card's writer (`Save_WriteFile`: "%s%s%02d%06d", "BASCUS-94154", the "SC" header
  titled "Crash Bandicoot 2", open, write, erase) and its helpers, among them `Save_TitleToShiftJis` (the header's
  full-width Shift-JIS title) and `Save_UnformatCard` (0xFF, 0xFF where `Save_IsCardFormatted` reads "MC"); the pads'
  update and records (`Input_Update`, `g_Pads`: the receive buffers Input_Init gives InitPAD); the sound's panning, its
  volumes (`Snd_SetSfxVolume`, `Snd_SetMusicVolume` over the SEPs SsSepOpen opened), voices and sequence; the memory
  card's state and strings; `CamMessage` and `SndVoice` for the local structures. The camera's handlers by path kind and
  by mode and the collision's (COLL's five modes) each have one caller, and neither CrashEdit, which knows property 0x29
  only as "mode", nor a string describes them: they keep their addresses. `tools/progress.py`: 171 of 564 game functions
  and 49 of 1,112 data symbols named. Next: the objects' and the GOOL interpreter's remaining functions, the rendering's
  (the draw lists, the models' and scenery's page-in workers), wherever a string, an SDK call or a format constant names
  them. A fifth batch: 24 game functions, 7 data symbols and 8 of `GoolObject`'s fields, from CrashEdit's GOOL tables.
  Its comments on the MISC opcode's kinds and subkinds name what `Gool_OpMisc` calls for each: the distances (exact,
  approximate, horizontal), the entity's perm flag (`Ent_SetPermFlag`, `Ent_ClearPermFlag`), the zone at a point, the
  kill of the objects outside the current zone, the continue point's save and load (`Sys_SaveCheckpoint`,
  `Sys_LoadCheckpoint`), the MIDI fade and playback, the game info's reset, the nearest object and the memory card's
  commands (`Save_RunCommand`); CHNK's kinds 4 and 5 the page counts. Its `ObjectFields` (word indexes from 0x40) agree
  with every field named before (`frame_time`, `state_time`, `entity`, `path_length`, `state_flags`, the hooks), and
  give eight more their names where the code shows the same use (`rot`, passed with `scale` to the model's drawer;
  `vel`; `target_rot`, set from ratan2; `status_a`, `status_b`; `misc`, CHNK's and the memory card's results;
  `event_received`; `voice`, `Snd_PlaySound`'s handle), the compiler finding each access through `GoolObject` alone; its
  `ControllerButtons` make 0x800 Start. Besides: the texture pages' binding to models and scenery
  (`Gfx_BindTexturePage`, which `Nsf_PageIn` calls once a texture page is in VRAM), the demo playback's start
  (`Input_LoadDemo`, `Input_StartPlayback`, the records from 0x1090 of `g_DemoBlock`), the game's and the level's init,
  `g_LevelRequest` (the "dead" state), `g_Paused` and `g_Quit`. `tools/progress.py`: 195 of 564 game functions and 56 of
  1,112 data symbols named. Next: the same tables' remaining uses, and the drawing (the handwritten renderers' C
  callers). A sixth batch, now that the interpreter is C: CrashEdit's GOOL instruction list names the opcodes the C
  numbered and their routines, the opcode table's entries giving each its routine: 19 LOOP (`Gool_Loop`), 34 SEEK and
  37 DSEK (`Gool_Seek`, `Gool_SeekAngle`), 48 DBG, "print A and B" (`Gool_OpDebug`, empty in the release), 58 VEC
  (`Gool_OpVector`) and 70 SPWF, "force spawn" (`Gool_OpSpawn`'s flag to `Obj_Spawn`); its object colours
  (`ObjectColors2`: mod1 to mod3, modfinal, a red, green and blue per vertex for 3, a final one) name `GoolObject`'s 16
  halfwords at 0x20, `colors`, which COLR and COLW (35, 36) reach and func_800186D8 reads from the fifth as a glyph's
  corners' scales. The Crash 1 catalog's GoolTranslateInGop and GoolTranslateOutGop (an operand's and a destination's
  address) name the interpreter's two decoders. Opcodes 72 and 74 to 77 CrashEdit doesn't know. `tools/progress.py`:
  221 of 638 game functions named. A seventh batch, what the port reaches first: the heap and the frame buffers.
  libsn's start-up (`__SN_ENTRY_POINT`) calls the game's allocator's init just before `main`, where PsyQ's calls
  `InitHeap`, with the heap's base and size it stores (`Heap_Init`, `g_HeapBase`, `g_HeapSize`) and computes from
  `g_RamSize` (the stack's top, less 8) and `g_StackSize`; the init gives the allocator's state its names (`g_HeapEnd`,
  `g_HeapBreak`, `g_HeapStarted`). `include/game/gfx.h` types the frame buffer, `GfxFrame`, in place of three local
  structures and the raw offsets of ten files: its index, primitives and their cursor, its timing in `g_TimerTicks`
  (the shown time, rounded to vertical blanks, is what the objects' motion scales by), the ordering table DrawOTag
  takes, the `DISPENV` and `DRAWENV` (now in `include/sdk.h`) PutDispEnv and PutDrawEnv take, and the object packets'
  queue that `Gfx_QueueObject` fills and `Gfx_DrawObjects` draws; `GfxFrameBuffers` is `g_FrameBuffers`. The two
  functions resetting a frame take their names from it (`Gfx_ResetFrame`, `Gfx_ClearObjectQueue`). Three accesses stay
  raw, the member moving GCC 2.7.2's scheduling (`GFX_FRAME_START_TICKS`, `GFX_FRAME_SHOWN_TICKS`,
  `GFX_FRAME_QUEUE_BUFFER`). The SHA-1 holds and the equivalent executable is byte for byte as before; the 33 files
  still compile for 32-bit x86. Left: `D_800675AC` (the frames before the game draws again, inside the level state
  at 0x80067588) and `D_8006CB94`, the flags `Gfx_EndFrame` takes from `D_8006CB80` each frame, whose bits serve the
  pads, the collision and the memory card's screens alike. `tools/progress.py`: 224 of 638 game functions and 63 of
  1,075 data symbols named, 144 structures in `src/game/`, 14 in `include/game/`, 118 fields read at a raw offset.
  An eighth batch, the SDK the port reimplements: the 91 functions in the parts of PsyQ's libraries that 4.0's
  signatures leave unknown, compared one by one with every version's (`tools/find_psyq.py`'s signatures, split at their
  functions). 39 have every byte but the relocated ones equal to one function of a later or earlier version alone, from
  12 to 382 words (libspu from 4.1 on: `SpuSetKey`, `SpuSetVoiceAttr`, `SpuSetCommonAttr`, the transfers, the reverb,
  `SpuMalloc`; libsnd's `_SsVm*` from 3.7 and 4.0); 15 more take their object's order where that object's bytes are
  the closest version's (`SSSTOP`, `SSCRES`, `SSDECRES`, `SSVOL`, `MIDIREAD`), or the use that splits a tie
  (`SpuWrite` from `SpuRead`: it calls `_spu_Fw`, and the game writes its sound pages with it). The game's libspu and
  libsnd are thus later than the rest of its 4.0 SDK. Left: 37 functions without such a match, among them the 8 to 11
  word wrappers that several objects share.
- **R6, the handwritten code's names (in progress).** The Crash 1 decomp's catalog (docs/REFERENCES.md, its
  docs/names.md, after c1's psx/r3000a.c) describes the same engine's handwritten routines, and where one here has the
  same layout and the code does the same, it gives the name (grade C, with the callers' same use): the memory copy
  (`Sys_Memcpy`, src, dst, bytes) and the ordering table's forward link (`Gfx_ResetOt`), each with the five tails for 0
  to 4 words left just before it, as in Crash 1; and the squared magnitudes after the tick handler
  (`Math_SqrMagnitude2`, 22 words, `Math_SqrMagnitude3`, 34, the sizes and order of Crash 1's RSqrMagnitude2 and
  RSqrMagnitude3). The integer square root (func_80041A78) is plain in the code but has no reference. Its renderers (the
  WGEO shaders, the SVTX transform) are reached through tables of labels rather than symbols here (0x8005B650,
  0x8005B690, 0x8005B708, 0x8005B75C), which splat and `tools/rename.py` don't name. `tools/progress.py`: 209 of 564
  game functions named. The equivalent C reads named fields where it read offsets, file by file, kept only when the
  equivalent executable stays byte for byte the same: the GOOL operations (`80037698.c`, through `OBJ(p)->field`), the
  object update and state change (`8001BFDC.c`), the object physics (`8001D63C.c`), `80029884.c`, the entity spawn and
  the nearest search, field by field, each kept only when the executable is unchanged. Some reads through the structure
  make GCC write other code than the offset did (`vel`, `status_a` in `8001D63C.c`), so those stay offsets; fields still
  `unk_` keep the files' named offsets. Where a block of `GoolObject` serves two uses, as 0x84 to 0xA4 read as a
  matrix's rows in Gfx_QueueObject, the offsets stay too. 0x108 took CrashEdit's name, `display_mode`, the modes
  func_8001BFDC switches on. A second batch, the handwritten code's own now that its equivalent C states what each
  routine does: the collision routines from 0x8003BF90 are the Crash 1 catalog's run in the same order (the octree
  query, the walls' plotter, the wall brush's 3-word entry and body, the floor's height; the query, the entry, the
  brush and the floor of the same sizes there, 575, 3, 71 and 205 words), `Zone_QueryOctree` to `Zone_FindFloorY`, and
  the sort lists' deltas its forward and backward decoders (`Gfx_DecodeSortListForward`, `Gfx_DecodeSortListBackward`).
  `func_8003D040`, after the floor in both, keeps its address: the catalog's ceiling query is 153 words, it 171, and
  its C (the lowest y or the average of three kinds' boxes) shows no ceiling. `tools/progress.py`: 210 of 638 game
  functions named. A third batch, the drawing, by what `tools/run_game.py` shows with a renderer's equivalent C
  emptied (a build of the experiment only): the scenery gone, the objects drawn over black (`Gfx_DrawWorlds`); Crash
  and Cortex's head gone, the scenery left (`Gfx_DrawObjects`); the demo's rain and the stars behind the intro's
  Universal logo gone (`Gfx_DrawStreaks`); the objects gone again with the packet list's builder emptied, which
  `Obj_Update` calls for each object (`Gfx_QueueObject`). `func_80045218`, the dots, changes no frame of new-game or
  demo emptied, and the game stops at game frame 4 without `func_8001AA80` (an object's matrix by the table at
  0x8005B650): both keep their addresses. 214 of 638 game functions named.
- **R7, the C made portable (done)**: what a host's compiler makes of the C, what is left for a host, and the
  macros the port redefines (section 6, "The macros").

Left for later: renaming the source files, grouping neighbouring globals into structures (it changes the code GCC
writes: docs/COMPILER.md, "Small data"), rewriting equivalent C for readability, the GOOL opcodes' names, the GTE
routines.

## 6. Beyond matching

With the game in C, a native port (the way Crash Twinsanity's PS2 decomp got its PC port) becomes possible. Not before the
code is.

The equivalent build is the whole game in C: every game function is matched or equivalent, and so are the handwritten
code's 172. What stays assembly is PsyQ 4.0's libraries and the split's data.

### Two repositories

The port lives in a repository of its own, which takes this one as a git submodule pinned at a commit. Each repository
keeps its own promise:

- **This repository** keeps the PS1 executable byte for byte, and is the only source of the game's C. Everything the
  port needs from the C lands here under that rule: a macro the port redefines, with the PS1's value by default. The
  PS1 build stays the reference a port's behaviour is compared against.
- **The port's repository** holds the platform:
  - the window, input and audio;
  - the renderer of the GPU's packets;
  - the SPU and libsnd's sequencer;
  - the CD read from the player's disc image, and the memory card in files;
  - the PsyQ functions the game calls, written for the host;
  - the interpreter of the native code;
  - the build that compiles this repository's `src/` for the host.

  It holds no copy of the C, so the two never diverge.

### What a host's compiler makes of the C

Each file of `src/game/` was compiled for x86 with the host's GCC 14.2:
`gcc -m32 -std=gnu89 -fsyntax-only -DNON_MATCHING -DGTE_MODEL -Iinclude`, and the same with `-m64`.

- **32-bit.** All 33 files compile with no error once `func_80027420` is declared before its first call.
  - 18 functions are used before their declaration and declared otherwise later in the same file.
  - `bzero` and `bcopy` are declared otherwise than GCC's built-ins.
  - 54 warnings are about function pointers: a function stored in a pointer of another type (40), or called
    through one (14).
  - `80039D2C.c`'s layout checks are constant only to GCC's folding.
  - A structure's trailing array of one element, read past it, let GCC at `-O2` assume a loop over an extern table
    runs once (`Gfx_Init`'s frame buffers, the page slots, `D_8006CB6C`'s items). Done: such arrays are `[0]`, the
    page slots' table its larger length (docs/COMPILER.md, "A zero-length array"), `g_FrameBuffers` its two frames.
- **64-bit.** The build has 736 casts between a pointer and an integer of another size, 396 to a pointer and 340 from
  one, in 28 files. 50 of `80039D2C.c`'s scratchpad layout checks fail, because its structures hold pointers.
- **Data from the disc.** Beyond the casts, the disc's data holds 32-bit slots the game overwrites with pointers:
  - `Nsf_RelocatePage` turns a page's and an entry's offsets into pointers in place (`NsfPage.entries`,
    `NsfEntry.items`).
  - `Nsf_OpenLevel` does the same with the NSD's buckets.
  - The GOOL entries' native code reads the object at fixed offsets.

  A 32-bit build keeps every one of these layouts; a 64-bit one needs 32-bit addresses wherever such a slot is read.

### What is left for a host, by kind

- **The GTE.**
  - `include/gte.h` models in C, under `GTE_MODEL`, the commands the C uses: RTPS, RTPT, MVMVA, DPCS, DPCL, DPCT and
    NCLIP.
  - The register moves stay MIPS instructions even there: `mfc2`, `mtc2`, `cfc2`, `ctc2`, `lwc2` and `swc2`, 590 uses in
    `800158E0.c` and `80039D2C.c`, which equiv.py runs on its emulator. A host needs them on a `GteRegs` in memory.
  - The 14 libgte functions the game calls remain: `InitGeom`, `MulMatrix`, `RotTrans`, `RotTransPers`, `ScaleMatrix`,
    `TransMatrix`, `SetRotMatrix`, `SetTransMatrix`, `SetColorMatrix`, `SetFarColor`, `SetGeomOffset`,
    `SetGeomScreen`, `ReadSZfifo3` and `ratan2`.
- **Other inline assembly, all in `80039D2C.c`:**
  - operation 73's call into native code (`gool_native`);
  - the `RETAIL_ADDRESS` absolute symbols;
  - a compiler barrier.
- **The native code.** Operation 73 runs the disc's MIPS blocks. They use 32 integer instructions, reach memory only
  through registers, and build no address of the executable (docs/TARGET.md). An interpreter of those 32 runs them on a
  32-bit host's pointers as they are.
- **The scratchpad.** Preprocessed, the C names the 1 KB at 0x1F800000 by number 142 times, in 12 files, under macros
  of their own (`WORLD_SCRATCH`, `DRAW_SCRATCH`, `GOOL_SCRATCH`, `SCRATCH_REG`, ...) and as literals (0x1F800040 in
  `Gool_ResolveRef`). The world renderer reads past its table into the copy of the opcode table there (docs/TARGET.md,
  "Code addresses read as data"). So a host's scratchpad has retail's layout, at an address the C would take from one
  place.
- **Addresses as values.**
  - Two tests take a value for a pointer only in KSEG0's first 8 MB (`& 0xFF800003` equal to 0x80000000): an
    animation's entry in `func_8001A4A4`, and a GOOL reference in `Gool_OpMisc`.
  - The GPU's ordering table links keep an address's low 24 bits (`OT_LINK_MASK` and `PRIM_ADDRESS_MASK` in
    `80039D2C.c` and `800158E0.c`), as the GPU's DMA reads them.
  - `func_80014C48` computes the top of RAM from 0x80000000.
  - A pointer tells itself from an error by its sign. The errors are small negative numbers (`SUCCESS` -0xFF, others
    -0x10, -0x12), and a KSEG0 address is negative below them. So `Obj_Init` takes `Nsf_OpenEntry`'s entry for an error
    unless it is below -0xFE (`RESULT_OK_LIMIT`). A grep finds 31 comparisons with -0xFE or -0xFF, or with constants
    named for a limit or an error; sign tests written otherwise, such as the handwritten code's `bltz` on pointers,
    escape it.

  The last point decides: crash2-pc's build at 0x08000000 had `Obj_Init` leave an object without its state, and
  `Obj_Update` read below its null stack; at 0x80000000 it went on. So the game's memory sits at those addresses,
  where every one of these tests holds as written.
- **The data.** The C reads 89 symbols of the split's `.data` and 380 of its `.bss`, and includes 19 blocks of
  `.rodata`: strings and constants, with no labels of code. splat's output there is directives only (`.word`, `.short`,
  `.byte`, `.asciz`, `.align`). A host's assembler takes it once `.word` means 4 bytes and `.align` a power of two. It
  stays generated from the player's executable, never committed.
- **The libraries.** The C calls 126 functions of PsyQ 4.0:
  - libapi 23, libgpu 20, libgte 14, libsnd 15, libc 13, libcd 9, libcard 9, libetc 4, libspu 3, libsn 1;
  - 15 more in libsnd's and libspu's objects that 4.0's signatures don't find: by later versions' (`SpuSetKey`,
    `SpuSetVoiceAttr`, `SpuWrite`, `SsSepStop`, `SsSepSetVol`, ... in `config/symbol_addrs.txt`).

  What the game asks of them:
  - **CD:** data only. Reads at double speed (`CdlSetloc`, `CdlSeekL`, `CdlStop`, `CdRead`) from the sectors the
    level table and the NSD give; no XA and no CD audio among the calls.
  - **Video:** the game builds the GPU's packets itself and `DrawOTag` sends them; textures go to VRAM by `LoadImage`,
    `MoveImage` and `StoreImage`. A host interprets GP0 packets into a 1024×512 VRAM.
  - **Sound:** libsnd's SEQ player (`SsSep*`) on the VABs the game hands it (`SsVabFakeHead`, `SsVabFakeBody`), and
    libspu's reverb.
  - **Timing:** `Sys_TimerHandler` on root counter 2's interrupt (target 0x1000), plus `VSync` waits.
  - **Memory card:** events, and the BIOS's file calls on `bu00:` (`open`, `read`, `write`, `erase`, `format`,
    `firstfile`, `nextfile`).
  - **Pad:** `InitPAD`'s two buffers.
- **Calls a host checks.** A host whose calls check their signatures (wasm32, which the port's 64-bit build goes
  through) needs each call's declaration to be its callee's. Done: the 17 functions called before their declaration
  declared first, `SpuIsTransferCompleted`'s value read through an `s32` declaration, `func_80023D7C` returning the value its
  callers read, the 4 functions declared as data for their addresses declared as functions (the default build's SHA-1
  and the equivalent executable the same byte for byte). Left to such a host: calls whose arguments differ from the
  definition's (`RotTrans`'s fourth, `func_8002CA1C`'s `$a0`) and pointers to functions of other signatures called
  through one type (`D_8005B1E0`'s handlers as `void (*)(void)`).

### The macros

`include/platform.h` holds them, each with the PS1's value unless the build defines it first:

- `SCRATCHPAD`, the scratchpad's address. Every use of the scratchpad in compiled code goes through it, the GOOL
  interpreter's words by their names in `include/game.h` (`SCRATCH_GOOL_IMMEDIATES`, `SCRATCH_GOOL_TABLE`,
  `SCRATCH_GOOL_HANDLERS`). equiv.py's contracts keep their numbers (`SAVE_AREA`, `LIGHT_RETURN`, ...), since they
  describe the retail code's own addresses.
- `RAM_BASE` and `RAM_ADDRESS(value)`: the two tests for a pointer into RAM, and the top of RAM `func_80014C48`
  computes.
- `OT_LINK(address)`: the 15 places where the GPU's ordering table or a primitive links to the next.
- With no PS1 default: the GTE's six register moves (`include/gte.h` defines them unless the build has), and
  `GOOL_RUN_NATIVE`, operation 73's native code on a host's interpreter in place of the inline call.
- `FRAME_ENDING()`: on entering `Gfx_EndFrame`, where `tools/run_game.lua` sets its game clock; a host keeps the same
  clock there, for its frames to compare with the PS1's.
- `PASSED_ON(arguments...)`: a call's arguments that the PS1 code passes on in the registers they arrived in, unnamed.
  `func_8002CB1C` calls `Zone_PlotWallEntry` with none, its own x and y left in `$a0` and `$a1`; a host names them.
  Named there, GCC 2.7.2 writes other code. `func_8002BF58` called `Zone_FindAt` with the zone alone, and the point
  passed the same way: there, naming it (`arg1`) gives the same bytes. A host's build ran into it, `Zone_FindAt`
  reading the point from its stack. A scan of the preprocessed C finds no other call with fewer arguments than the
  game's definition of the function takes.
- `CALLER_LEFT(function)`, `CALLER_LEFT_VALUE(variable, value)` and `LEFT_RETURN(value)`: a path on which the PS1
  code reads what a register was left holding, empty on the PS1. A host notes the path, gives the variable a value
  of its own (four functions: `Heap_Realloc`, `func_80017380`, `Obj_SpawnEntity`, `func_8001F054`) and returns where
  the C has no return statement (`Obj_SetParent` the 2 retail's `$v0` holds, `Snd_ControlSequence`, whose callers
  read none, 0). With them, clang 23 at `-O2` (wasm32) finds no read without a value (`-Wsometimes-uninitialized`)
  and no missing return (`-Wreturn-type`) in `src/game/`.
- `ASM_SYMBOL(name)`: a C symbol's name in the assembler's code, which `RETAIL_ADDRESS` (`80039D2C.c`) defines for
  the opcode table's retail addresses. As it is on the PS1; 32-bit Windows' C symbols take a leading underscore, which
  the port's Windows build adds.
- `GOOL_HOST_OPERATION(object, word)`, with no PS1 default: what `Gool_Interpret` runs for an opcode the game's code
  holds none of (past the 79 it has), the object's stack and frame stored as for the game's own operations. The port
  patches the menus' GOOL code in memory with such words, each doing the replaced instruction's work besides its own
  (an options screen in the game's menus). The equivalent build compiles the default case as before.
- With no PS1 default, in the GTE model (`include/gte.h`): `GTE_PROJECTED(sxy, view, h, ofx, ofy)`, a vertex's
  projection before its rounding (the port's steady polygons), and `GTE_HORIZONTAL(offset)`, a projection's offset
  across, the port narrowing it for a 16:9 picture.
- `WORLD_SORT_LIST(list)`, the sort list `Gfx_DrawWorlds` draws (the camera's), and `FRAME_PRIMITIVES_ALLOC(bytes)` and
  `FRAME_PRIMITIVES_FREE(buffer)`, a frame buffer's primitives from the heap: a host may draw more of the world into
  larger buffers of its own (the port tried it at 16:9 and dropped it: crash2-pc's docs/ROADMAP.md, milestone 12).
- `FRAME_BETWEEN`, 0 on the PS1: a frame of the picture alone, `Sys_MainLoop` running none of the game's logic,
  `Obj_Update` none of an object's, `Gfx_EndFrame` stepping none of its clocks and handing the frame to
  `FRAME_BETWEEN_SHOWN()`; `FRAME_ENDED()` at a game frame's end. The port draws one between each two of the game's
  for 60 frames a second, the game's logic at 30.

Both executables stay byte for byte the same with them, and equiv.py agrees on 300 trials of each function they touched
that it tests (`Gool_ResolveRef`, `Gool_OpMisc`, `func_8001A4A4`, `Gfx_ResetOt`, `Gfx_DrawStreaks`). With a host's
definitions of all of them, the 33 files of `src/game/` compile into 32-bit x86 objects with no error (GCC 14.2,
`-m32 -std=gnu89 -O1 -DNON_MATCHING -DGTE_MODEL`). Their link then needs the split's data (470 symbols, the handwritten
code's `g_TimerTicks` among them), the PsyQ functions they call, and the host's own definitions.

### The port's decisions

They belong to the port's repository. Each changes its design and is costly to reverse:

1. **The pointers' width.**
   - A 32-bit build keeps every layout and runs the native code as it is.
   - A 64-bit build needs 32-bit slots wherever the disc's data or the native code holds a pointer.
2. **Where the game's memory sits.**
   - At retail's addresses: the KSEG0 tests, the 24-bit links and the pointers' sign hold as written.
   - Anywhere: the tests and the links each become a macro, and so does every test of a pointer's sign.

   crash2-pc took retail's addresses: the sign tests can't all be found (above).
3. **The backends.** The renderer for the GPU's packets, and the mixer for libsnd's sequences and the SPU's voices.
