# Equivalent C for $FUNCTION

You are decompiling one function of Crash Bandicoot 2 (PlayStation, USA) on the equivalent track: write C that does what
`$FUNCTION` (`target.s`) does, for every input, and reads like this project's C. Its bytes don't have to match: the
matching loop already gave it up. Read `BRIEF.md` first: its address, size, callers, callees, the data it touches, and
m2c's draft.

It is compiled with PsyQ's GCC 2.7.2 at `-O2 -G8 -mcpu=r8000`, with `NON_MATCHING` defined. C89: declarations at the start
of a block, no `//` comments, no C99 features.

## Rules

- Work only in this directory. Edit `base.c`, or write new `*.c` files here; nothing else. Never edit `target.s`.
- Plain C only: no inline assembly (`asm`, `__asm__`), no `INCLUDE_ASM`, no `M2C_` macro or type (no `m2c_macros.h`), no
  `#if`/`#ifdef` of its own.
- `./score.sh FILE.c` is the only judge, and it judges the file as it would land. It first puts it into a copy of the
  function's file (`placed/<file>.c`, beside the `placed/game.h` landing would write) the way landing does: in a
  `NON_MATCHING` block, with a copy of a declaration the file already has dropped and new prototypes of basic types moved
  into `game.h`. A file landing would refuse prints `not landable as it is: ...` with why; a declaration that conflicts
  with the file's, or with another file's calls, is a compile error naming a line of `placed/` (read it there). Then
  `tools/equiv.py` runs that copy's function and the retail one in an emulator from the same random memory and
  arguments, trial by trial, and compares how each ended, the return value, the callee-saved registers, every call out
  (target and arguments, in order) and every word of memory changed. It prints the first disagreeing trial with its
  arguments and differences, then `N of M trials agree`. `./score.sh FILE.c --seed 1` runs other trials;
  `--trials 2000` more of them.
- The C must do what the assembly does, not what passes the tester. Callees aren't run (each call is recorded and gets a
  made-up return value), random inputs rarely reach every path, and trials ending at the instruction limit or in the
  same exception aren't compared. None of that is room to leave a path out, guess a callee's effect, or simplify a
  condition the trials happen not to reach: every branch of the assembly has its counterpart in the C.

## Style

The C lands in the function's file (BRIEF.md's "File") as the reference for what the function does, so it reads like
this project's C (docs/WORKFLOW.md, "Naming"):
- Constants get names (`#define` or `enum`), not bare numbers, but for the obvious ones (`0`, `1`, `-1`).
- Every `if`/`else if` chain ends with an `else`, every `switch` has a `default`, every path of a function that returns a
  value returns one. No nested `?:`.
- Name no function, variable or shared type: keep the names BRIEF.md and the files use (splat's `func_`/`D_` ones, or
  those given since). Naming is a separate, reviewed step (docs/WORKFLOW.md, "Naming"). Locals and parameters say what
  they hold where the code makes it clear; a field is `unk_<offset>`. A structure of the function's own is named after
  its function or data (`Unk80014148`), not `Entry` or `Obj`: it lands beside other functions' types, and one name
  defined twice fails the build.
- Comments are short and say what isn't obvious from the code.

## The loop

1. `./score.sh base.c` once, after only the fixes it needs to compile (types, `?` placeholders, `M2C_` macros): the
   baseline. When BRIEF.md has an earlier run, score its `earlier.c` too and start from the one with more agreeing
   trials.
2. Work out what the function does from the assembly, the draft, its callers and callees (BRIEF.md), then write that C
   plainly.
3. Score after every change. Keep each attempt in its own file (`attempt1.c`, `attempt2.c`, ...), and copy the best so far
   to `best.c` each time more trials agree: the session has a time limit, and what isn't in a file when it ends is lost.
4. Read each disagreement against `target.s`: the trial's arguments and what differed point at the path the C gets wrong.
5. Stop when every trial agrees on two seeds (`./score.sh FILE.c` and `./score.sh FILE.c --seed 1`), then on landing's
   own check (`./score.sh FILE.c --seed 1 --trials 2000` and `--seed 2 --trials 2000`: a rare path one of those reaches
   refuses the landing); after five attempts in a row that don't raise the count; or when `score.sh` shows 2 minutes
   left, so that `best.c` and `NOTES.md` are written.

## When you stop

- Every trial agrees on landing's check: copy the file to `equivalent.c`. It goes into its file as it is, so:
  - `#include "common.h"`, `"sdk.h"` (PsyQ's functions) and `"game.h"` (the game's), then the types and declarations
    the function needs, then the function.
  - A function `include/game.h` or its file declares is called through that prototype, not declared again (BRIEF.md
    lists them). A callee neither declares gets a prototype in `equivalent.c`, with its true signature from the callee's
    assembly and its parameter list (`(void)` for none, never `()`): landing moves it into `game.h` when it uses only
    basic types.
  - What BRIEF.md lists as declared already for the function and its data holds: the function's signature and each
    variable's type are those (matched callers compile against them); `score.sh` fails to compile on a conflict.
  - A structure its file already defines is copied from the file as it is, never defined otherwise; landing keeps the
    file's own definition, above the function's block when it was further down.
  - The shared types in `include/game/*.h`, which `game.h` includes, are used as they are: never defined again, never
    defined otherwise.
  - Data is `extern` (`extern s32 D_8005F304;`), never defined: the build resolves it to the split's symbol. A string or
    constant in `.rodata` (`D_800100D0`) too (`extern char D_800100D0[];`), never a literal.
- Otherwise: copy the best attempt to `best.c`.
- Either way, write `NOTES.md`: what the function does, what was tried, the final count, and what still disagrees. Short.
