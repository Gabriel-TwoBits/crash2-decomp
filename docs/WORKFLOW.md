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

A function is called equivalent only once `tools/equiv.py`, a differential tester against the retail function, agrees on
every trial.

### Testing an equivalent function

    .venv/bin/python tools/equiv.py src/game/<file>.c FUNCTION [--trials 500] [--seed 0]

It compiles the file as `ninja nonmatching` does and relocates FUNCTION's code into a scratch area: every name the
retail executable knows (`build/SCUS_941.54.elf`, so the build comes first) resolves to its retail address, the file's
own `.sdata`/`.sbss` variables and `$gp` included; jump tables and literals without a name go to scratch copies. Then
Unicorn runs the candidate and the retail function from the same state, trial by trial: RAM and the scratchpad filled
with random words (zeros, small numbers, pointers into an arena of such words, the function's own constants), each word
of the executable's data either its retail value or random, random arguments (an arena pointer for a parameter the C
declares as a pointer). A call out of the function is recorded (target, arguments as the callee's prototype counts
them, a pointer into the caller's frame by the words there) and answered with a made-up return value, not run.

Compared after each trial: how the run ended (returned, an exception, the instruction limit), `$v0` (unless the function
is `void`), the callee-saved registers, the calls in order, and every word either side changed outside its own stack
frame. Both runs reaching the limit (500,000 instructions or 1,000 calls), or taking the same exception, agree (reported;
after an exception only the calls are compared). The first disagreement is printed with its trial's arguments; the last
line is `N of M trials agree`, and the exit status 0 only when all do. A 500-trial run takes about 2 seconds, up to 15
for a function spinning on what a callee would change.

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
- GTE instructions (coprocessor 2) aren't emulated: such a function stops with `unsupported: ...` and exit status 2.

### Landing an equivalent function

    .venv/bin/python tools/match.py land FUNCTION --equivalent

The candidate is `build/match/FUNCTION/equivalent.c`, else the notes' C (`config/match/notes/FUNCTION.equivalent.c`, then
`FUNCTION.c`). Its declarations are handled as a match's (a prototype of basic types moves into `include/game.h`, a copy
of what the file has goes), and the rest goes into the block in place of the `INCLUDE_ASM` line; a function that already
has a block gets its C replaced. Refused before building: m2c's macros, inline assembly, an `INCLUDE_ASM` line, a
conditional of its own, and a definition the file has further down, which moved into the block would leave the default
build. Then `configure.py`, `ninja` (the SHA-1), `ninja nonmatching`, and `tools/equiv.py` on the landed file, not the
candidate (the file's declarations and `include/game.h` compile it), at 2,000 trials and seed 1: every trial has to
agree, and at least 5% of them compared, not agreeing by both reaching the limit or taking the same exception. A failure
restores the file and `include/game.h` and is recorded as `equivalent, not landed` with its cause (a function whose block
stays keeps `equivalent`); a landing is recorded as `equivalent` with equiv.py's count (`2000 of 2000 trials agree, 1119
compared`) and committed as `feat: add equivalent C for FUNCTION` (`feat: improve the equivalent C of FUNCTION` for a
replacement).

Most refusals of the matching runs' notes come from the nonmatching build, where the C meets its file's declarations for
the first time: a variable or the function itself declared with another type there than in the candidate.

### The loop's equivalent mode

    .venv/bin/python tools/match.py loop --equivalent --count N --minutes 15 --model claude-sonnet-5-5

It takes the smallest function the matching loop gave up on (`gave up`, `rejected`, or notes' C that didn't land as
equivalent) that is still plain `INCLUDE_ASM`, has no GTE instruction and has had no equivalent run. The agent's
instructions are `tools/match/EQUIVALENT.md`; its `score.sh` runs equiv.py on a draft (`./score.sh FILE.c --seed 1`), and
its starting points are the assembly, m2c's draft and the matching run's notes and C. It stops when every trial agrees
on two seeds, after five attempts without progress, or two minutes before the limit, leaving `equivalent.c` or `best.c`.
`tools/match.py` checks `equivalent.c` itself (500 trials, seed 0, `equivalent, agrees`), then lands it as above. A run's
notes and C go to `config/match/notes/FUNCTION.equivalent.md` and `.c`, beside the matching run's. `run --equivalent`,
`prepare --equivalent` and `next --equivalent` do one step of it. The matching loop leaves equivalent functions alone.

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
