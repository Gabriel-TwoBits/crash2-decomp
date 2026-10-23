# Working on a function

The unit of work is one function: its C written until, compiled with the original compiler, it gives the retail bytes.

## The loop

`tools/match.py` runs this loop with an agent: `next` picks a function, `run` prepares its scratch directory and lets
headless Claude Code work in it, `land` puts a verified match into its file in `src/game/` and commits it, `loop --count N`
repeats. By hand, the steps are below.

### Running the loop unattended

From the repository's root, in a terminal that stays open (tmux, screen, or `nohup ... &`):

    .venv/bin/python tools/match.py loop --count 50 --minutes 15 --model claude-sonnet-5-5 --retry-model claude-opus-5-5

Each function gets Sonnet, then Opus if Sonnet came within 10% of a match (elsewhere Opus rarely matched, at several
times the cost). A function given up under other compiler flags than the build's is tried again first, from its earlier
notes. A match lands and is committed (`feat: match NAME`) only when the build's SHA-1 holds; the other runs' notes are
committed at the end (`chore: record the matching runs`). Nothing is pushed. Afterwards, `tools/match.py report` sums it
up, and `git log` shows what landed. Runs use the user's Claude usage: a function costs between $0.10 and a few dollars
at API prices. `config/match/skip.txt` lists functions not to try, with why.

`--again` makes a second pass over the functions landed as equivalent C, each once, the closest first-pass attempt
first: the agent also gets the landed C (`landed_equivalent.c.txt`, whose logic equiv.py vouched for) beside m2c's draft
and the first pass's notes. A match replaces the NON_MATCHING block; a miss leaves the function equivalent, its notes in
`config/match/notes/<function>.again.md` and `.c`.

1. **Pick** a function from the split (`asm/`). Smaller ones and ones whose callees are already in C go
   first; a function's callers and callees give its meaning.
2. **Draft** it with m2c from its assembly, with the context the project already has (`include/`'s types and declarations).
3. **Refine** the draft until it compiles and matches: types, control flow, the order of statements, temporaries. This is
   where an AI agent works best, with the draft, the target assembly, its callers' and callees' signatures and similar
   matched functions as context, and the diff after each attempt. `--diff` also counts the retail instructions left
   without a counterpart once registers are set aside: the code's shape, brought to 0 before the registers. Measured elsewhere (Mizuchi, on GBA and N64 games): about
   three quarters of functions matched, most of them in the first three attempts; later attempts rarely did.
4. **Permute** a near miss with decomp-permuter, which tries random rewrites and keeps those that score better. It often
   closes the last register swap.
5. **Check** with `tools/probe_compiler.py`: no word of the function may differ (relocated fields masked), and once it is in
   `src/` the whole executable's SHA-1 must still be the retail one (`ninja`).
6. **Record** it: the C replaces the function's `INCLUDE_ASM` line in its file in `src/game/` (docs/TARGET.md, "The game's
   source files"), with names (below) for what became clear and its declarations: a game function's prototype once, in
   `include/game.h` (in the file when another file gives the function other types, a structure of its own), the SDK's in
   `include/sdk.h`. A variable it reads through `$gp` is defined in that file: `.sdata` at the top of it, with its value;
   `.sbss` as a tentative definition (`s32 D_8005F304;`), which resolves to the split's symbol. `configure.py` splits again (splat leaves out the assembly of a function written in C), then `ninja` must
   pass. A function that can't be matched yet stays as assembly in the build.

## What counts as matched

Only the probe's zero differing words and the unchanged SHA-1. "Equivalent" code that compiles to other bytes is not matched, however
obviously equivalent. The assembly is never edited to fit the C, and the build never swaps a function's object for the retail
one.

## The equivalent track

A function too costly to match can get C that behaves the same, kept out of the default build:

```c
#ifdef NON_MATCHING
s32 func_X(char *text) { /* behaviour-equivalent C */ }
#else
INCLUDE_ASM("asm/nonmatchings/game/<file>", func_X);
#endif
```

The block keeps exactly that shape, with no conditional of its own inside: `tools/progress.py` counts the function as
equivalent, never as in C, and `tools/match.py land` replaces the whole block once the function matches. splat extracts
the assembly of every `INCLUDE_ASM` line whatever surrounds it, so `ninja` still builds and checks the retail bytes.

`ninja nonmatching` builds `build/nonmatching/SCUS_941.54.exe` with `NON_MATCHING` defined: its own C objects, the
default build's assembly, no SHA-1 check. With no equivalent function it is the retail executable byte for byte. A
function of another size moves what follows it; the linker script is sequential, so the link holds, and `configure.py`
writes a variant of splat's for it: the header's entry point and `.text` size taken from the link, splat's pinned
addresses left to the labels that define them, the end of `.bss` where it is linked. Every address the split's data
holds is a symbol, relocated in both builds: `config/symbol_addrs.txt` names those splat found no symbol at, and
`configure.py` stops on a word left as a number that points into the image. The handwritten code
(`asm/game/80039D2C.s`) loads the addresses of 20 of the game's functions as `lui`/`ori` pairs of numbers, which no
relocation writes: the equivalent build assembles a copy, `build/nonmatching/asm/game/80039D2C.s`, where each of the 21
pairs is a `lui`/`addiu` of the function's symbol (found by address in splat's `.splat/splat_symbols.csv`), and
`configure.py` stops unless it finds all 21 and leaves no such number.

The data follows the code. The handwritten code reads its structures through `$gp` at offsets splat writes as `%gp_rel`
of small data, which the linker would move with `.data` (docs/TARGET.md, "Addresses from the disc"): with no equivalent
function, `.data` moved by 8 bytes so displays otherwise from frame 1,390 of `tools/run_game.py` and crashes in the
first warp room. The equivalent build's copy writes those offsets (32 since the split by function,
`GP_RELATIVE_ACCESSES`) as the numbers their instructions hold, and `configure.py` stops unless it finds them all and
leaves no `%gp_rel`. `ninja nonmatching` prints the room left in the retail file's 327,680 bytes, or how far past them
the executable runs: `tools/patch_disc.py` then writes it after the image's last sector, and the heap, which runs from
the end of `.bss` to the stack, is that much smaller. It stops past 64 KB over (`MAX_GROWTH`), a limit to raise once a
build that long has played every schedule.

A function is called equivalent only once `tools/equiv.py`, a differential tester against the retail function, agrees on
every trial.

### Testing an equivalent function

    .venv/bin/python tools/equiv.py src/game/<file>.c FUNCTION [--trials 500] [--seed 0]

It compiles the file as `ninja nonmatching` does and relocates FUNCTION's code into a scratch area: every name the
retail executable knows (`build/SCUS_941.54.elf`, so the build comes first) resolves to its retail address, the file's
own `.sdata`/`.sbss` variables and `$gp` included; jump tables and literals without a name go to scratch copies. The
file's `static` functions come along as the candidate's own code, not calls out: they stand for code the retail function
has inline (`func_80029D0C`'s helper was compared as a call before). Then
Unicorn runs the candidate and the retail function from the same state, trial by trial: RAM and the scratchpad filled
with random words (zeros, small numbers, pointers into an arena of such words, the function's own constants), each word
of the executable's data either its retail value or random, random arguments (an arena pointer for a parameter the C
declares as a pointer, a value of its type for one declared narrower than a word: `u8`, `s16`, ...). A call out of the function is recorded (target, arguments as the callee's prototype counts
them, a pointer into the caller's frame by the words there) and answered with a made-up return value, not run. The
retail code and read-only data are read-only: a store through a wild pointer ends the run instead of rewriting the
retail function as it runs (`func_80015344`'s table index went negative into its own code, which then raised a
reserved-instruction exception, reported as an instruction Unicorn lacks).

Random words make most dereferenced words bad pointers, so a trial whose retail run ends on a memory access fault (an
address error, a TLB miss, unmapped memory, a store into the code) is repaired before the candidate runs. Another
emulator follows the retail run instruction by instruction, tracking which input word each register holds (a word of
memory as it was on entry, `$a0`-`$a3`, a register a callee returned) through moves, additions, shifts and
multiplications, and the word the faulting address came from is replaced: by an arena pointer where it was the
address's base, a small aligned offset where it was added to an address already in memory, a small count where it was
shifted or multiplied first (an offset or a count still faulting is set to zero once). The retail run then starts over,
up to 16 times. The candidate runs from the final repaired state, the same as the retail code's; repairs depend only on
the retail code, the seed and the trial.

Compared after each trial: how the run ended (returned, an exception, the instruction limit), `$v0` (unless the function
is `void`), the callee-saved registers, the calls in order, and every word either side changed outside its own stack
frame. A trial where the retail run reaches the limit (500,000 instructions or 1,000 calls) agrees whatever the
candidate does, as one where both take the same exception (reported, not compared; after an exception only the calls
are compared); a candidate reaching its limit, four times the retail code's (C may copy by halfwords where the retail
code moves words: `Gfx_DecodeSortListBackward`) and 4,000 instructions more for each GTE command the retail run gave (one
instruction there, about 2,300 for an RTPT on `include/gte.h`'s model: `Gfx_DrawStreaks`'s 1,017 streaks took 2,428,386
instructions in C, 73,118 in the retail code), where the retail code returned disagrees.
A disagreeing trial's retail code runs again with
every bit flipped in the registers that aren't arguments, then with them zeroed: when its result changes, it depends on
registers its caller left (a path reading one it never set, which C can't name: `func_80017380` uses the caller's `$s0`
for an entry type it doesn't handle; `func_8003D040` a box's sizes before its list's first header, two huge values and
their flips covering the box alike), and the trial agrees, reported and not compared. Two runs stopped by the same address error compare
where it stopped and their registers there. The first disagreement is printed with its trial's arguments and
repairs; the last line is `N of M trials agree`, and the exit status 0 only when all do. A 500-trial run takes 1 to 2
seconds, 5 to 25 for a function whose trials often reach the limit or fault after thousands of instructions (a repair
follows the retail run instruction by instruction, in Python: `func_8002F6C8`, 24 seconds).

Its limits, which a passing run doesn't lift:

- Random inputs are evidence, not a proof. A path behind several conditions on memory may never run: of seven mutants of
  matched functions, five were caught in 500 trials, one more in 5,000 (about 20 seconds), one not at all (a
  counter's bound, reached only when two other variables hold matching values, in `func_800237F8`). Run more trials for
  a large function.
- Callees don't run: what they write and return for real is missing, so C that relies on a callee's effect isn't checked
  beyond the arguments it passes. A made-up return value can take the caller down a path the game never runs; where the
  callee's code bounds what it returns, `CALLEE_RETURN_RANGES` in equiv.py says so, with the reading that proves it, and
  the caller's code for other values isn't checked (`func_8002FFFC`: 6 at most, which `func_8002CC50` uses as a count
  of 12 points).
- A frame address passed to a callee is compared by its first 4 words whatever the local's size; past a smaller local
  they may be other locals, which differently laid out C keeps elsewhere.
- One word per parameter (no 64-bit or structure arguments), and a variadic callee's named parameters only.
- A fault isn't repaired when it comes after 20,000 instructions, or through an address no single input word explains
  (a loop running past its table, a word the function stored itself): such trials still agree by the exception. A path
  behind an equality of two words stays rare (`func_8001F9C4` spins in `for (;;) {}` unless two memory words match, so
  most of its trials reach the limit).
- The limit counts instructions, which differ between the two sides: a loop of about 500,000 retail instructions may
  return in the candidate (`func_80015344`, seed 2, trial 1,553: a 27,000-row loop; at 5,000,000 both return and
  agree), so a retail run past the limit isn't judged, and the candidate's work there isn't checked.
- The GTE (coprocessor 2) runs on a model of it, `tools/gte.py`: its moves (`mfc2`, `cfc2`, `mtc2`, `ctc2`, `lwc2`,
  `swc2`) and the commands the C models, RTPS and RTPT; another command stops the run with `unsupported: ...` and exit
  status 2, as a GTE instruction in a delay slot does. Its 64 registers start random, in half the trials with a
  rotation, translation and projection the game would use (otherwise RTPS nearly always flags an error), and are compared
  after both runs. `mfc2`'s and `cfc2`'s load delay isn't modelled. The model is psx-spx's account of the GTE, not the
  hardware's: C and retail agreeing on it means they agree with each other on what psx-spx describes.

### A function's contract

Random trials pass what no caller does, and some retail code then does what no C can say: `Sys_Memcpy` jumps into
code for a negative count, and stores a block's words high to low, which a NULL or an overlapping buffer shows. The
equivalent C may state the function's contract with its callers, lines of nothing in the build (`include/common.h`):

    EQUIV_RANGE(Sys_Memcpy, 2, 0, MEMCPY_MAX_HALFWORDS)   /* $a2 between 0 and 0xFFFF, signed */
    EQUIV_NONNULL(Sys_Memcpy, 0)                         /* $a0 never NULL */
    EQUIV_DISJOINT(Sys_Memcpy, 0, 1, 2, 2)               /* $a0's and $a1's buffers, $a2 * 2 bytes, apart */
    EQUIV_BUFFER(Gfx_DecodeSortListForward, 2, SORT_LIST_BYTES)      /* $a2 a buffer of its own, apart from the others declared */
    EQUIV_HALF(Gfx_DecodeSortListForward, 1, 0, 0, SORT_LIST_ENTRIES) /* the halfword at $a1 between 0 and 0x5F0 */
    EQUIV_SCRATCH(func_8003DA00, SAVE_AREA, SAVE_AREA_BYTES) /* what it leaves there doesn't count */
    EQUIV_RUNS(Zone_PlotWallEntry, Zone_PlotWall)             /* the retail code branches into it as its own */
    EQUIV_CALLBACK(Zone_FindFloorY, 7, 2, t8, t9)          /* argument 7 a function of 2 arguments, writing neither */
    EQUIV_WORDS(Gfx_DrawWorlds, 2, 0x10, func_8003DD0C, ...) /* the 5 words at $a2 + 0x10 one such row (a line each) */
    EQUIV_KEEPS(Gfx_DrawWorlds, func_8003DD0C, v1)        /* a call to func_8003DD0C leaves $v1 */
    EQUIV_TABLE(Gool_Interpret, GOOL_TABLE_ADDRESS, g_GoolOpcodeHandlers) /* the word there points to the table */

equiv.py then keeps its trials to it (a value outside the range moved into it by its remainder, a NULL drawn for that
pointer left out, the second buffer moved past the first, a buffer of its own that overlaps one declared before it moved
past them all, the halfword moved into its range by its remainder wherever the argument points, the scratch left out
of the memory compared, the routine run as part of the retail function, which still has other callers in the build, a call through the callback compared on
its arguments only and leaving the registers named, which a call otherwise sets at random, one of the rows drawn and
written there, a name standing for its retail address, a call to the callee leaving the registers named, and the
word pointing to the table, whose words keep their retail values), and
prints it with the verdict, which
`tools/match.py` records. The scratch is the handwritten code's save area, the scratchpad's first 16 words: every
function that uses it writes it before reading it, and no compiled code touches it. Nothing a function reads lives
there either, so no word of a trial is drawn from the code's constants pointing into it: the retail code saves `$sp`
there first, and a frame whose next primitive was 0x1F800000 (the `lui` of the area's address, among the constants)
had `Gfx_DrawStreaks`'s lines overwrite it and the retail code fault on return, where the C, saving nothing there,
returned. A bound is a number or a `#define` of one in the file; a line it can't read stops it. A contract goes in by
hand, with its evidence in the comment above it: every caller, compiled and handwritten, and what each passes.
`tools/match.py` refuses C holding one from a run: the loop's agent never narrows its own trials.

A group of handwritten routines that share registers goes into C together, its routines the function's
`HANDWRITTEN_PARTS`; those the C file defines are the candidate's own code, as the retail ones are the retail
function's. A call to one resolves to the candidate's, and so does a call through a pointer the trial holds to its
retail address (the world renderer calls its scene's mode entries so): equiv.py runs the candidate's there instead of
answering a call out.

A condition on the data a function reads, which no argument names, goes in its body: `EQUIV_ASSUME(based);`, nothing in
the build. equiv.py compiles the candidate with `EQUIV_TRIAL` defined, where a failing condition calls `Equiv_Reject`,
which ends the trial: it agrees, not compared, and is counted apart. `Gfx_DecodeSortListForward` assumes a near swap follows a short
or long one, as every one of the disc's 62,033 does (docs/TARGET.md): otherwise the retail code's base is whatever a
register holds, the caller's, a jump target its copy left, or a flipped entry's address. Its evidence is the data
itself, read whole from the disc and counted, and it goes beside the line.

### The GTE in C

The game's routines written with GTE instructions get equivalent C through `include/gte.h`, a model of the GTE in C
included under `NON_MATCHING` only (its functions are `static`, which equiv.py runs as the candidate's own code, and GCC
2.7.2 emits them used or not). The C moves a register as the retail code does (`GTE_WRITE_DATA(GTE_VXY0, v)`,
`GTE_READ_CONTROL(GTE_FLAG, flag)`, `GTE_STORE_DATA(GTE_SXY2, quad->xy2)`: one instruction each, the register's number
part of it) and runs a command as `Gte_Rtpt()`. With `GTE_MODEL` defined, as equiv.py compiles the candidate and a
native port would, that is a call to the model, which reads what the command needs into a `GteRegs`, computes it in C,
and writes back what it changed; without it, in the equivalent build, it is the GTE's own instruction, two `nop`s after
the last move as PsyQ's macros wait. equiv.py thus judges the C with the model against the retail code on
`tools/gte.py`, and the build runs the same C with the commands the retail code gives. A projection is one instruction
there and about 2,300 on the model: with `Gfx_DrawStreaks`'s streaks on the model, the equivalent build showed the
opening's game frame 846 at vsync 11,994 (retail at 2,721), the schedule's end near; and the model's code
cost the build about 1,800 bytes for each file including it. The model agrees with `tools/gte.py` on 6,000 random
register files and commands (checked while writing it), and on every trial of the functions using it; a command it
lacks is added to both, from psx-spx. `tools/gte.py` also runs `mvmva` (the rotation, light and light colour matrices,
the translation or background colour added or none), `nclip`, `dpcs` and `dpct`, the handwritten code's other commands;
it refuses the garbage matrix and the far colour's wrongly added sum, which no code uses. The C has
`Gte_MultiplyMatrix` (`mvmva` as the game encodes it: sf 1, nothing added), for a file that defines `GTE_MVMVA` before
including the header, since GCC 2.7.2 emits every static function, and `Gte_Dpcs` and `Gte_Dpcl` (sf 1, lm 0) for one
that defines `GTE_DPCS`, ported from `tools/gte.py`'s (DPCL added to both from psx-spx: RGBC's components times IR1-IR3,
then DPCS's interpolation) and agreeing with it in the world renderer's modes. The handwritten code also puts a GTE instruction in
a branch's delay slot (`func_8003D3CC`'s `ctc2`), where equiv.py's hook can't stop: it runs the instruction on the model
as the branch starts, with the slot a `nop` meanwhile, when it moves no CPU register and the branch doesn't link into
what it reads.

### The handwritten code

The handwritten code (`asm/game/80039D2C.s`, docs/TARGET.md) can't match from C and has no `INCLUDE_ASM` lines: the
default build assembles it whole. Its equivalent C is `src/game/80039D2C.c`, which only `ninja nonmatching` compiles:
`configure.py` leaves each function the file defines (its `static` ones aside) out of the equivalent build's copy of the
handwritten code, from its `nonmatching` line to the next one, and links the file's code and read-only data right
after that copy, so the code before a replaced function stays where it was. The GOOL opcode table's entries, retail
code addresses a renderer reads as bytes (docs/TARGET.md, "Code addresses read as data"), keep retail's values: the
C defines the operations' labels at their retail addresses.
Routines only one function reaches, such as the tails its dispatch jumps
into, go with it: `HANDWRITTEN_PARTS(Sys_Memcpy, Sys_Memcpy1, ...)` in the file names them, and equiv.py runs them as
part of the retail function. `configure.py` (through `tools/handwritten.py`) stops on a function the handwritten code
doesn't have.

All 172 of its functions have equivalent C (`tools/progress.py`). Some are called from compiled code with the usual
convention; the others take values in registers the convention leaves alone (`$s3`, `$t5`, `$v1`) and call one another,
through tables too. A function went into C with every routine it alone calls, once nothing handwritten depended on
registers its retail code leaves untouched (`grep` the handwritten code for its name and its address), `$v0` included:
`Gfx_DrawWorlds` never sets it and returns what its last call left, `Gfx_ResetOt`'s caller's value, so `Gfx_ResetOt`
waited for it. A routine several shared stayed assembly until its last caller was C. Each function is tested as any
equivalent one (equiv.py, 2 seeds of 2,000 trials, then `tools/run_game.py` on every schedule), and `tools/progress.py`
counts them apart.

### Landing an equivalent function

    .venv/bin/python tools/match.py land FUNCTION --equivalent

The candidate is `build/match/FUNCTION/equivalent.c`, else the notes' C (`config/match/notes/FUNCTION.equivalent.c`, then
`FUNCTION.c`). Its declarations are handled as a match's (a prototype of basic types moves into `include/game.h`, a copy
of what the file has goes), and the rest goes into the block in place of the `INCLUDE_ASM` line; a function that already
has a block gets its C replaced. A structure the file defines further down moves above the block, outside it, where
both builds see it (a type emits no code). Refused before building: m2c's macros, inline assembly, an `INCLUDE_ASM`
line, a conditional of its own, and a variable the file defines further down (moved, it would change the data's
layout; into the block, it would leave the default build). Then `configure.py`, `ninja` (the SHA-1), `ninja nonmatching`, and `tools/equiv.py` on the landed file, not the
candidate (the file's declarations and `include/game.h` compile it), at 2,000 trials for each of seeds 1 and 2: every trial has to
agree, and at least 5% of them compared, not agreeing by the retail run reaching the limit or both taking the same
exception. A failure
restores the file and `include/game.h` and is recorded as `equivalent, not landed` with its cause (a function whose block
stays keeps `equivalent`); a landing is recorded as `equivalent` with equiv.py's count (`2000 of 2000 trials agree, 1119
compared`) and committed as `feat: add equivalent C for FUNCTION` (`feat: improve the equivalent C of FUNCTION` for a
replacement).

Most refusals of the matching runs' notes come from the nonmatching build, where the C meets its file's declarations for
the first time: a variable or the function itself declared with another type there than in the candidate. An agent's
draft meets them before landing (below).

### The loop's equivalent mode

    .venv/bin/python tools/match.py loop --equivalent --count N --minutes 15 --model claude-sonnet-5-5

It takes the smallest function the matching loop gave up on (`gave up`, `rejected`, or notes' C that didn't land as
equivalent), or one of `0x240` bytes or more not tried yet (the matching loop matched about one in seven to ten of
those, at US$7-10 a match), that is still plain `INCLUDE_ASM`, has no GTE instruction and hasn't been settled by an equivalent run. The agent's
instructions are `tools/match/EQUIVALENT.md`; its starting points are the assembly, m2c's draft and the matching run's
notes and C. Its `score.sh` (`tools/match.py score FUNCTION FILE.c [--seed N] [--trials N]`) judges a draft as it would
land: placed by the landing's own code into a copy of its file beside a copy of `include/game.h`
(`build/match/FUNCTION/placed/`, where a quoted `#include "game.h"` finds the copy first), the other files calling a
function whose prototype that copy changes compiled there too, then equiv.py on the placed file. A refusal prints
`not landable as it is: ...`, a declaration conflict is the compiler's error on a line of the copy: both are what
landing would stop at, seen while the agent can still fix them. It stops when every trial agrees
on two seeds, after five attempts without progress, or two minutes before the limit, leaving `equivalent.c` or `best.c`.
`tools/match.py` checks `equivalent.c` itself the same way (500 trials, seed 0, `equivalent, agrees`), then lands it as
above. A run's record says it was scored in place (`"scored": "in place"`); a function refused at landing after runs that
judged their draft alone, before `score.sh` placed it, is taken again, once. A run's
notes and C go to `config/match/notes/FUNCTION.equivalent.md` and `.c`, beside the matching run's. `run --equivalent`,
`prepare --equivalent` and `next --equivalent` do one step of it. The matching loop leaves equivalent functions alone.

### Running the game

    .venv/bin/python tools/run_game.py "path/to/Crash Bandicoot 2 - Cortex Strikes Back (USA).cue" [--against retail]
                                       [--baseline EXE] [--schedule {new-game,turtle-woods,memory-card,demo}]
                                       [--real-time] [--dump FRAME ...]

equiv.py judges one function at a time; this plays the equivalent executable as a whole, in the game. It needs the
`ninja nonmatching` build and the player's disc image (a MODE2/2352 `.cue` and `.bin`, never copied into the repository),
and writes into `build/` only:

1. `tools/patch_disc.py` copies the image to `build/disc/nonmatching.bin` with `build/nonmatching/SCUS_941.54.exe` in
   place of `SCUS_941.54`: the file's sectors rewritten with their EDC and ECC, its directory record given the size. An
   executable that outgrows the file's 160 sectors (327,680 bytes, the retail size, which `ninja` pads to) goes into
   new sectors after the image's last, given the address and subheaders a Form 1 file's take, and the directory record
   and the volume's size point to them; the BIOS finds the file by name, and no other file moves. Written with the
   retail executable, the copy is the original image byte for byte.
2. PCSX-Redux (`tools/setup_toolchain.py` fetches it) boots the retail image and the copy at once, headless,
   unthrottled, on its own BIOS (OpenBIOS, no Sony BIOS needed), and `tools/run_game.lua` plays both on the game clock.
   The game measures each frame with `g_TimerTicks`, which `Sys_TimerHandler` counts up from a root counter's interrupt,
   and moves its objects by it, so code of another speed plays another game. The script undoes each of the handler's
   increments before it happens and, on entering `Gfx_EndFrame`, sets `g_TimerTicks` to 34 times the game frame's
   number: every game frame lasts two vsyncs for the game however long it took. Presses and checkpoints are keyed to
   game frames (`Gfx_EndFrame`'s calls from power-on), and a game frame's image is the one on the display when the next
   call starts: New Game from the title's menu, the intro, Cortex's speech, into Turtle Woods from the first warp room,
   run, jump, spin; 3,450 game frames, about three minutes paced (below). `--schedule turtle-woods` runs on into Turtle Woods blindly,
   jumping and spinning, through deaths and respawns until the lives run out, 6,000 game frames. A renderer there reads
   code addresses' bytes (docs/TARGET.md, "Code addresses read as data"), so a build whose GOOL opcode table holds other
   addresses differs from game frame 3,524; and the level's sound and disc reads go on in real time, so a build of another
   speed played otherwise from game frame 3,666 until the schedule paced game frames 2,900 to 6,000 (5 vsyncs each,
   as below). Cortex's lips follow his speech's sound, which plays in real time too: with the world renderer in C,
   which takes 3 vsyncs a game frame where retail takes 2, the equivalent build drew another mouth from game frame 1,380,
   so new-game and turtle-woods are paced from game frame 1,100 on. `--schedule memory-card` picks Load Game from the title's menu and presses through the memory card's
   screens blindly, one button of a cycle every 30 game frames: on the empty card it goes through Load/Save, Save Game
   and Enter Name, saves, and comes back to the warp room, 3,500 game frames. The card answers in real time, so slower
   code reaches its screens in fewer game frames (`func_800186D8`'s equivalent, which draws the menu, takes 4 vsyncs a
   frame there where retail takes 3, and the equivalent build left the card's directory 50 game frames earlier); the
   schedule paces game frames 1,100 to 3,500 instead (to 3,000 until the world renderer in C made the warp room after
   the save play otherwise from game frame 3,339): the SDK's `VSync(0)` that `Gfx_EndFrame` calls first is given 10,
   so each lasts 10 vsyncs in both runs. The saves still differ by 12 bytes, by design: the game steps `Rand_Below` in
   the loops that wait for the card's events (`Save_TestSwEvents`, `Save_TestHwEvents`) and names the save with the next
   number. `--schedule demo` presses nothing: left at the title's menu, the game plays its attract demo, Turtle Woods
   then a river level, 4,800 game frames, about two minutes. The three symbols' addresses come from each executable's
   ELF (the default build's for retail); the breakpoints need the emulator's interpreter and debugger. At every vsync
   the script also records a hash of the displayed image, the program counter and the last exception's code, for the
   run's health. `--real-time` keeps the game's own clock instead and counts vsyncs: presses at fixed vsyncs, 8,000 of
   them, about 15 seconds on the emulator's recompiler. `--dump FRAME` writes each run's main RAM as that game frame is
   displayed (`ram_FRAME.bin` in its folder under `build/run_game/`), to find what the two runs hold differently before
   their images part.
3. A run is healthy when it reaches its last frame (one without a new vsync for 20 seconds is stopped: the game crashed
   or hung; on the game clock, one short of its last game frame at its schedule's vsync limit too), its program counter
   stays in RAM or the BIOS, it takes no exception but interrupts and system calls, and its display isn't frozen over
   its last 300 vsyncs. The two runs agree when every frame's image is the same; the checkpoints go side by side to
   `build/run_game/<checkpoint>.png`. Exit status 0 only when both hold.

The emulator is deterministic: two retail runs (`--against retail`) agree on every frame, on the game clock and in real
time (there throttled or not: only the program counter sampled at each vsync moves by a few cycles), so a difference
comes from the executable. That holds only with the emulated pads cut off from the host: on their default, Auto, they
read the host's gamepads (a wireless receiver, a virtual HID device), and a button the host reports lands in the pad
buffer (`D_80069920`) at a moment that varies from run to run; three pairs of retail runs then differed from frames
1,496 to 2,245. `tools/run_game.lua` sets both pads to the keyboard, which a headless emulator gets no events from.

On the game clock a difference comes from the C, not from its speed. The whole equivalent build displays retail's image
at all 3,451 game frames, 6 vsyncs behind at the end, where in real time it differs from vsync 4,035: one game frame
there takes 2 vsyncs instead of 3, the game moves by the shorter time, and every frame after is another. Half its files'
equivalents alone end 13 vsyncs behind retail and still agree on every game frame of a run through the attract demo. In
real time a difference may be only timing (the retail executable under PCSX-Redux's interpreter instead of its
recompiler displays otherwise from vsync 2,004).

A difference on the game clock is narrowed to a function by splitting the equivalents: by source file (each file's
object from the equivalent build or from the default one), then within the file by block (`#ifdef NON_MATCHING` turned
to `#if 0` for the others, a block's declarations being shared with its neighbours). Two runs at a time: with four the
interpreter's vsyncs come slower than the stall limit.

A run lasts at most about 32,700 vsyncs: the emulator's vsync event listener leaks Lua stack slots at each call (an
empty listener too) and overflows LuaJIT's stack there, a vsync's trace line lost. `tools/run_game.py` refuses a
schedule whose vsync limit passes 32,000.

A group may grow the executable past the retail file (by up to `MAX_GROWTH`, above). Then the group is compared
with itself without the new functions: `--baseline` runs that build in place of retail, and agreeing on every frame
means the new functions display as the retail code did wherever the game reaches them. Whether it reaches them at all
shows by sabotage: the same build with them drawing nothing differs (`func_8001856C` and `func_800186D8`, the title's
menu: 41 frames).

## Matching C and this project's style

The C has to compile to the retail bytes, so the original's shape wins over style: a missing `else`, an early `return` in
the middle, a temporary that looks useless, a `do { } while (0)` can each be what makes it match. Where style costs nothing
in the output it applies: constants get names (`#define` and `enum` produce no code), names say what things are, comments
explain what isn't obvious. Tools (Python) follow the usual rules fully.

## Naming

Splat names unknown things by address: `func_80012A4C`, `D_8005F17C`. They get real names once their purpose is clear,
not before:

| Kind | Convention | Example |
| --- | --- | --- |
| Functions | `Subsystem_PascalCase` | `Gool_Interpret`, `Nsf_PageIn` |
| Global variables | `g_PascalCase` | `g_CurrentLevel` |
| Types (structs, enums) | `PascalCase` | `GoolObject` |
| Struct fields, locals, parameters | `snake_case` | `velocity_y` |
| Constants, macros | `UPPER_SNAKE_CASE` | `GRAVITY` |
| Unknown fields | `unk_<offset>` | `unk_1C` |

A struct field's offset stays in a comment until the whole struct is known (`/* 0x1C */ s32 velocity_y;`). The SDK's
functions keep their PsyQ names (`LoadImage`, `CdRead`).

A name needs evidence, else the address name stays (a wrong name costs more than an address):

- **A**: a call into the SDK, a string, a format constant checked in this project's assembly, or what
  `tools/run_game.py` shows; or
- **B and C**: the same use by two independent callers or more, and a reference's note on the behaviour
  (docs/REFERENCES.md) that the assembly confirms.

Names are given with `tools/rename.py`, never by hand:

    .venv/bin/python tools/rename.py OLD NEW --evidence "what proves it"
    .venv/bin/python tools/rename.py --table FILE.tsv     one rename per line: old, new, evidence, tab-separated

The name goes into `config/symbol_addrs.txt`, from which splat writes it into the split: the symbol's line replaced, its
attributes kept (`size:`, `type:`), or a line added; the evidence a full-line comment above it (splat reads attributes
after `//` on the symbol's own line). Never a second name at an address: `configure.py`'s rewrite of the handwritten
code's `lui`/`ori` pairs stops on one. Each whole word of `src/` and `include/` is renamed, `INCLUDE_ASM` and
`INCLUDE_RODATA` lines included (splat then writes `asm/nonmatchings/game/<file>/NEW.s`). Refused: an unknown OLD, an
SDK function or a label, `D_8006F1F0` (`configure.py`'s end of `.bss`), a NEW already used (a symbol, a word of `src/`,
`include/` or `config/symbol_addrs.txt`) or off the conventions: a game function `Subsystem_PascalCase` with one of the
prefixes `Nsf`, `Gool`, `Obj`, `Zone`, `Gfx`, `Snd`, `Input`, `Save`, `Heap`, `Math`, `Rand`, `Sys`, `Cd`, `Ent`, `Cam` (no PsyQ name has one
followed by an underscore), or `main`; a variable `g_PascalCase`. Then `configure.py`, `ninja` and `ninja nonmatching`
run, and the batch stays only when the SHA-1 holds and the equivalent executable is byte for byte what it was (`ninja
nonmatching` is deterministic); otherwise every file goes back. `--allow-equivalent-change` keeps a batch that changes
the equivalent executable and lists the functions whose code changed (relocated fields masked), which `tools/equiv.py`
(2 seeds of 2,000 trials) and `tools/run_game.py` then judge. Nothing is committed: a batch is reviewed, then committed
as `refactor: name ...`.

The matching loop's records (the ledger, the notes, `skip.txt`, `build/match/`) stay keyed by the address name, and
`tools/match.py` takes either name; a candidate's C written before a rename lands under today's names. Docs citing a
function by its address keep doing so.

Shared types go in `include/game/<subsystem>.h` (types, enums and constants, no code), included by `include/game.h`,
which keeps the one list of prototypes. Their names carry the subsystem (`GoolObject`, `NsfEntry`): files have local
structures of their own, `Node`, `Ent` and `V` each in two or three. Landing treats them as the file's own: a candidate's copy
identical to the header's goes, one defined otherwise is refused (`defines GoolObject otherwise than
include/game/object.h`), and a prototype using them moves into `include/game.h` like one of basic types.
`tools/progress.py` counts the names given and the structures, and the fields still read at a raw offset.

## Tools

| Tool | What it does |
| --- | --- |
| splat (`splat64`) | Splits the executable into assembly and data by `config/SCUS_941.54.yaml` |
| PsyQ GCC (`tools/toolchain/gcc-*-psx`) | The original compiler, in each candidate version |
| maspsx | Rewrites GCC's assembly the way Sony's ASPSX assembled it, then runs GNU `as` |
| objdiff (`objdiff-cli`) | Compares built objects with the retail ones, per function. Fetched, not wired in yet: `tools/probe_compiler.py` and the SHA-1 check do its job for now |
| m2c | Drafts C from a function's assembly |
| decomp-permuter | Random rewrites of a near miss, scored against the target |
| decomp.me | The same compilers online, to share a function's attempt (scratches) |
| Unicorn (`unicorn`) | The CPU emulator under `tools/equiv.py`, which runs equivalent C against the retail function |
| PCSX-Redux (`tools/toolchain/pcsx-redux`) | The PlayStation emulator under `tools/run_game.py`, which plays the game with the retail and the equivalent executables |
