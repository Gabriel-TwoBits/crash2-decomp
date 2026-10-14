# Working on a function

The unit of work is one function: its C written until, compiled with the original compiler, it gives the retail bytes.

## The loop

1. **Pick** a function from the split (`asm/`). Smaller ones and ones whose callees are already in C go
   first; a function's callers and callees give its meaning.
2. **Draft** it with m2c from its assembly, with the context the project already has (`include/`'s types and declarations).
3. **Refine** the draft until it compiles and matches: types, control flow, the order of statements, temporaries. This is
   where an AI agent works best, with the draft, the target assembly, its callers' and callees' signatures and similar
   matched functions as context, and the diff after each attempt. Measured elsewhere (Mizuchi, on GBA and N64 games): about
   three quarters of functions matched, most of them in the first three attempts; later attempts rarely did.
4. **Permute** a near miss with decomp-permuter, which tries random rewrites and keeps those that score better. It often
   closes the last register swap.
5. **Check** with objdiff: the function's score must be 100% with relocations masked, and the whole executable's SHA-1 still
   the retail one.
6. **Record** it: the C goes into its source file in place of the assembly include, with names (below) for what became
   clear. A function that can't be matched yet stays as assembly in the build.

## What counts as matched

Only objdiff's 100% and the unchanged SHA-1. "Equivalent" code that compiles to other bytes is not matched, however
obviously equivalent. The assembly is never edited to fit the C, and the build never swaps a function's object for the retail
one.

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
| objdiff (`objdiff-cli`) | Compares built objects with the retail ones, per function, and reports progress |
| m2c | Drafts C from a function's assembly |
| decomp-permuter | Random rewrites of a near miss, scored against the target |
| decomp.me | The same compilers online, to share a function's attempt (scratches) |
