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
   `include/game.h`, the SDK's in `include/sdk.h`. A variable it reads through `$gp` is defined in that file: `.sdata` at
   the top of it, with its value; `.sbss` as a tentative definition (`s32 D_8005F304;`), which resolves to the split's
   symbol. `configure.py` splits again (splat leaves out the assembly of a function written in C), then `ninja` must
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

Only the code moves. The game reads some of its data by absolute address (docs/TARGET.md, "Addresses from the disc"):
with no equivalent function, `.data` moved by 8 bytes displays otherwise from frame 1,390 of `tools/run_game.py` and
crashes in the first warp room. So the equivalent build pads the end of the code to `.data`'s retail start (the main
segment's data subsegment in `config/SCUS_941.54.yaml`), and `ninja nonmatching` stops, with the number of bytes over,
when the equivalent code outgrows the retail code, or when `.data` or the end of `.bss` lands elsewhere.

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
declares as a pointer). A call out of the function is recorded (target, arguments as the callee's prototype counts
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
are compared); a candidate reaching the limit where the retail code returned disagrees. A disagreeing trial's retail code runs again with
every bit flipped in the registers that aren't arguments: when its result changes, it depends on registers its caller
left (a path reading one it never set, which C can't name: `func_80017380` uses the caller's `$s0` for an entry type it
doesn't handle), and the trial agrees, reported and not compared. Two runs stopped by the same address error compare
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
  beyond the arguments it passes.
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
- GTE instructions (coprocessor 2) aren't emulated: such a function stops with `unsupported: ...` and exit status 2.

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

equiv.py judges one function at a time; this plays the equivalent executable as a whole, in the game. It needs the
`ninja nonmatching` build and the player's disc image (a MODE2/2352 `.cue` and `.bin`, never copied into the repository),
and writes into `build/` only:

1. `tools/patch_disc.py` copies the image to `build/disc/nonmatching.bin` with `build/nonmatching/SCUS_941.54.exe` in
   place of `SCUS_941.54`: the file's sectors rewritten with their EDC and ECC, its directory record given the size. An
   executable that outgrows the file's 160 sectors (327,680 bytes, the retail size, which `ninja` pads to) is refused.
   Written with the retail executable, the copy is the original image byte for byte.
2. PCSX-Redux (`tools/setup_toolchain.py` fetches it) boots the retail image and the copy at once, headless, unthrottled,
   on its own BIOS (OpenBIOS, no Sony BIOS needed). `tools/run_game.lua` presses the same buttons at the same frames in
   both: the title's menu, New Game, the intro skipped, Cortex's speech in the first warp room waited out, into Turtle
   Woods, run, jump, spin; 8,000 frames, about 15 seconds. At every frame it records a hash of the displayed image, the
   program counter and the last exception's code, and at eight checkpoints the image.
3. A run is healthy when it reaches its last frame (one without a new frame for 20 seconds is stopped: the game crashed
   or hung), its program counter stays in RAM or the BIOS, it takes no exception but interrupts and system calls, and
   its display isn't frozen over its last 300 frames. The two runs agree when every frame's image is the same; the
   checkpoints go side by side to `build/run_game/<checkpoint>.png`. Exit status 0 only when both hold.

The emulator is deterministic: two retail runs (`--against retail`) agree on all 8,000 frames, throttled or not (only the
program counter sampled at each vsync moves by a few cycles), so a difference comes from the executable. Not
necessarily from its C, though: the game is sensitive to timing (the retail executable under PCSX-Redux's interpreter
instead of its recompiler displays otherwise from frame 2,004) and to where its data sits (docs/ROADMAP.md, section 4).

Slower or faster functions add up: with enough equivalents in, a load ends a frame later and the inputs then land on
another state of the game. Such a difference is told from a wrong function by splitting the equivalents (by source
file, a block's declarations being shared with its neighbours: `#ifdef NON_MATCHING` turned to `#if 0` for the others):
when each group alone agrees on every frame, every function ran right and only their sum's timing differs.

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
prefixes `Nsf`, `Gool`, `Obj`, `Zone`, `Gfx`, `Snd`, `Input`, `Save`, `Heap`, `Math`, `Rand` (no PsyQ name has one
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
