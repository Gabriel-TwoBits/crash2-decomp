# Crash Bandicoot 2 decompilation

A matching decompilation of **Crash Bandicoot 2: Cortex Strikes Back** for the PlayStation (USA, SCUS-94154): the game's
executable rewritten in C that, built with the original compiler, gives the retail executable byte for byte.

> [!IMPORTANT]
> **No game data is included.** You need your own copy of the game. Crash Bandicoot is a trademark of Activision; PlayStation
> of Sony Interactive Entertainment. This is a fan project, not affiliated with or endorsed by either.

## Status

The executable is split and rebuilt byte for byte, the game's code compiled as C with the original compiler, its
unmatched functions included from the split. The milestones are in [docs/ROADMAP.md](docs/ROADMAP.md).

| | |
| --- | --- |
| Target | `SCUS_941.54`, SHA-1 `aabdade44a4ca863f71224daef51c45f34fb1bf8` ([docs/TARGET.md](docs/TARGET.md)) |
| SDK | PsyQ 4.0, its libraries identified and split apart |
| Compiler | PsyQ GCC 2.6.0 to 2.7.2, `-O2 -G8`, ASPSX 2.30 to 2.49 ([docs/COMPILER.md](docs/COMPILER.md)) |
| Game code in C | 17 of 561 functions, 284 of 218,836 bytes (0.13%): `tools/progress.py` |

## Why

A matching decompilation is the game's source as it could have been written: readable, documented, and proven equal to
the original by its bytes. It is what makes fixes, mods and native ports possible without emulating anything. Crash 2 has
recompilations ([Crash2Recomp](https://github.com/Zumbo06/Crash2Recomp)) and a machine translation
([c2c](https://github.com/ughman/c2c)), which run the game, but no decompilation.

## Getting started

[CONTRIBUTING.md](CONTRIBUTING.md) has the setup and the rules, [docs/WORKFLOW.md](docs/WORKFLOW.md) how a function gets
matched, and [docs/REFERENCES.md](docs/REFERENCES.md) the projects and documentation this one builds on.

## Licence

GNU General Public License, version 3 or later ([LICENSE](LICENSE)), for what is written here. The game belongs to its
owners.
