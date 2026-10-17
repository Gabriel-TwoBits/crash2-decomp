# Matching $FUNCTION

You are decompiling one function of Crash Bandicoot 2 (PlayStation, USA): write C that, compiled with the original compiler,
gives exactly the bytes of `$FUNCTION` (`target.s`). Read `BRIEF.md` first: its address, size, callers, callees, the data it
touches, and m2c's draft.

The compiler is PsyQ's GCC 2.7.2 at `-O2 -G8 -mcpu=r8000` (`score.sh` uses them), assembled the way ASPSX 2.34 does.
C89: declarations at the start of a block, no `//` comments, no C99 features.

## Rules

- Work only in this directory. Edit `base.c`, or write new `*.c` files here; nothing else.
- Never edit `target.s` or `target.o`, never use inline assembly (`asm`, `__asm__`), never pin registers
  (`register ... asm("$s0")`). A match has to come from plain C.
- `./score.sh FILE.c` is the only judge: it compiles the file and prints the retail and compiled instructions side by side,
  `!` on each differing word, then how many differ. Matched means `0 of N words differ`. The line before it counts the
  retail instructions with no counterpart once registers are set aside and the two sides aligned: the shape of the code.
  One register allocated otherwise changes most words of a large function, so that line is the one to bring down first;
  at 0, only registers differ.
- `./permute.sh FILE.c MINUTES` runs decomp-permuter on a near miss and prints its best candidates with their real count
  from the probe. The permuter's own score ignores `nop`s, so a permuter "0" is not a match until `./score.sh` says so.

## The loop

1. `./score.sh base.c` once, unchanged: the baseline. m2c's draft may need small fixes to compile (types, `?` placeholders);
   make only those before the first score. When BRIEF.md has an earlier run, score its `earlier.c` too and start from the
   closer of the two.
2. Work out what the function does from the draft, its callers and callees (BRIEF.md), then write that C plainly. Fix the
   control flow first (the branches and loops line up), then the register and scheduling differences.
3. Score after every change. Keep each attempt in its own file (`attempt1.c`, `attempt2.c`, ...), and copy the best so far
   to `best.c` each time the count drops: the session has a time limit, and what isn't in a file when it ends is lost.
4. While instructions are left without a counterpart, keep reworking the shape: the loops (`for`, `while`, `do`, `goto`),
   the conditions and their order, early returns, a helper's body written inline, a value kept in a local or read again.
   A large function rarely lines up in the first attempts, and the time is there for it. Stop when it matches; when only
   registers and scheduling differ and the permuter can't close them in 5 minutes; after five attempts in a row that
   lower neither count; or when `score.sh` shows 2 minutes left, so that `best.c` and `NOTES.md` are written.

What tends to matter with this GCC: `goto`-built loops are not hoisted like `for`/`while` ones (a constant kept in a saved
register means the original was a `for`/`while`); the order of statements and of declared locals changes register
allocation; a local array or struct sets the frame size; `x / 4` (signed) differs from `x >> 2`; an unused temporary can be
what restores the original's register; a global's address kept in a saved register (`lui`/`addiu`, then `0x0($sN)`)
means an array, `extern s32 D_X[]` used as `D_X[0]`.

## When you stop

- Matched: copy the matching file to `match.c`. It goes into its file (BRIEF.md's "File") as it is, so:
  - `#include "common.h"`, `"sdk.h"` (PsyQ's functions) and `"game.h"` (the game's), then the types and declarations
    the function needs, then the function. No `m2c_macros.h`: replace every `M2C_` type or macro with a real type.
  - A function `include/game.h` or its file declares is called through that prototype, not declared again (BRIEF.md
    lists them). A callee neither declares gets a prototype in `match.c`; landing moves it into `game.h` when it uses
    only basic types (`s32`, `void *`, ...), so make it the callee's true signature, from the callee's assembly, with
    its parameter list (`(void)` for none, never `()`).
  - A structure its file already defines is copied from the file as it is (landing drops the copy), never defined
    otherwise: `match.c` compiles alone.
  - The shared types in `include/game/*.h`, which `game.h` includes, are used as they are: never defined again, never
    defined otherwise.
  - Data is `extern`, except a variable the retail code reads through `$gp`: this compiler only does that for a variable
    defined in the same file, so define it. One in `.sdata` is defined at the top of the function's file already: copy
    that definition as it is. One in `.sbss` (`asm/data/bss.bss.s`) is defined without a value (`s32 D_8005F304;`).
  - A string or constant in `.rodata` (`D_800100D0`) stays `extern` (`extern char D_800100D0[];`), never a literal: its
    file includes it from the split. A `switch` is fine: its jump table comes with the function.
  - Name no function, variable or shared type: keep the names BRIEF.md and the files use (splat's `func_`/`D_` ones,
    or those given since). Naming is a separate, reviewed step (docs/WORKFLOW.md, "Naming"). A structure of the
    function's own is named after the function or data it belongs to (`Unk80014148`), not `Entry` or `Obj`: it lands
    in a file beside other functions' types, and one name defined twice fails the build.
- Not matched: copy the best attempt to `best.c`.
- Either way, write `NOTES.md`: what the function does, what was tried, the final count, and what still differs. Short.
