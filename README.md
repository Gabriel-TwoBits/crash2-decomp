# Crash Bandicoot 2 decompilation

A decompilation of **Crash Bandicoot 2: Cortex Strikes Back** for the PlayStation (USA, SCUS-94154): the game's
executable rewritten in C. Where it can, the C built with the original compiler gives the retail bytes exactly; where it
can't, it behaves like the original, tested against it function by function.

> [!IMPORTANT]
> **No game data is included.** You need your own copy of the game. Crash Bandicoot is a trademark of Activision; PlayStation
> of Sony Interactive Entertainment. This is a fan project, not affiliated with or endorsed by either.

## Status

The executable is split and rebuilt byte for byte, the game's code compiled as C with the original compiler, its
unmatched functions included from the split. A second build (`ninja nonmatching`) compiles the equivalent C in their place.
The milestones are in [docs/ROADMAP.md](docs/ROADMAP.md).

| | |
| --- | --- |
| Target | `SCUS_941.54`, SHA-1 `aabdade44a4ca863f71224daef51c45f34fb1bf8` ([docs/TARGET.md](docs/TARGET.md)) |
| SDK | PsyQ 4.0, its libraries identified and split apart |
| Compiler | PsyQ GCC 2.7.2, `-O2 -G8 -mcpu=r8000`, ASPSX 2.30 to 2.49 ([docs/COMPILER.md](docs/COMPILER.md)) |
| Game code in C | 270 of 466 compiled functions match byte for byte, 34,248 of 166,780 bytes (20.53%), in 32 source files: `tools/progress.py`. The other 196 have behaviour-equivalent C, tested, not matched (132,532 bytes, 79.47%; docs/WORKFLOW.md, "The equivalent track"). The 172 functions from `0x80039D2C` on are assembly source, kept as such in the matching build ([docs/TARGET.md](docs/TARGET.md)), all 172 with equivalent C |

## Why

A decompilation is the game's source as it could have been written: readable, documented, and checked against the
original, by its bytes where the C matches and by running both side by side where it doesn't. It is what makes fixes,
mods and native ports possible without emulating anything. Crash 2 has recompilations
([Crash2Recomp](https://github.com/Zumbo06/Crash2Recomp)) and a machine translation ([c2c](https://github.com/ughman/c2c)),
which run the game, but no decompilation.

## How it's made: AI-assisted

This project uses AI, openly. Most functions are matched by an AI agent (Claude, run through Claude Code) working in a loop
(`tools/match.py`, docs/WORKFLOW.md): it reads a function's assembly and m2c's draft, writes C, and tries again against the
diff. The tools, docs and build were also written with AI assistance, directed and reviewed by the maintainer.

What makes that safe is that nothing rests on the AI's word. A function counts as matched only when the original compiler
turns its C into exactly the retail bytes, and the whole executable still has the retail SHA-1; the scripts check both, and
the build fails otherwise. A function that doesn't match counts as equivalent only when `tools/equiv.py` runs its C and the
retail code on the same random inputs and every compared trial agrees. So matched C is proven equal to the game and
equivalent C is tested against it, whoever wrote it. What isn't proven is the meaning: names, types and comments the
agent chose can be wrong, and corrections are welcome.

## Getting started

[CONTRIBUTING.md](CONTRIBUTING.md) has the setup and the rules, [docs/WORKFLOW.md](docs/WORKFLOW.md) how a function gets
matched or made equivalent, and [docs/REFERENCES.md](docs/REFERENCES.md) the projects and documentation this one builds on.

## Licence

GNU General Public License, version 3 or later ([LICENSE](LICENSE)), for what is written here. The game belongs to its
owners.
