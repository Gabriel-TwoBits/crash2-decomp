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
addresses left to the labels that define them, the end of `.bss` where it is linked. Words of data splat left as
numbers (addresses it found no symbol for) keep their retail values and go stale once anything before them changes size.

A function is called equivalent only once `tools/equiv.py`, a differential tester against the retail function (to be
written), agrees on every trial.

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
