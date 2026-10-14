# The target

Every matching effort targets exactly one build: the North American retail release.

| | |
| --- | --- |
| Game | Crash Bandicoot 2: Cortex Strikes Back (PlayStation) |
| Release | USA, SCUS-94154 |
| Disc image | One data track, MODE2/2352, 192,814,608 bytes. SHA-1 `3d37fe4288a83b01f1a0edbf78e76e0fede6bc22`, CRC32 `395C0916` (the CRC32 ScriptHawk identifies this release by) |
| `SCUS_941.54` | 327,680 bytes, SHA-1 `aabdade44a4ca863f71224daef51c45f34fb1bf8` |
| `SYSTEM.CNF` | 66 bytes, SHA-1 `1c83a4e02b90bffa66580555ae884b0566b203cc` |

`tools/extract_disc.py` takes both files out of a disc image into `orig/USA/` and checks these hashes. The disc image and
everything taken from it stay out of the repository (`.gitignore`).

## The executable

A PS-X EXE: a 2048-byte header, then one block loaded at a fixed address.

| | |
| --- | --- |
| Entry point | `0x80049B2C` |
| Load address | `0x80010000` |
| Size loaded | `0x4F800` (325,632 bytes) |
| Initial stack | `0x801FFFF0` (the header's and `SYSTEM.CNF`'s `STACK`) |
| `$gp` | `0x8005F17C`, set by the startup code at `0x80049BA8` (the header's field is 0). 413 instructions address through it, so the game was compiled with a small data threshold (`-G`), most likely `-G8` |
| Region string | `Sony Computer Entertainment Inc. for North America area` |

Strings left in it:

- `c:/src/crash2/target/`: the build folder at Naughty Dog.
- PsyQ library versions: `bios.c 1.86` (1997-03-28), `intr.c 1.76` (1997-02-12), `sys.c 1.129` (1996-12-25), and
  "Library Programs (c) 1993-1997 Sony Computer Entertainment Inc.". The SDK is newer than the Crash Bandicoot 1 decomp's
  (`sys.c 1.120`, `bios.c 1.71`, about PsyQ 3.5); docs/COMPILER.md has what follows from it.

About 1,000 `jr $ra` instructions: on the order of a thousand functions, the SDK's included. No symbols survive in the
retail executable, and no build of the game with them is known.

## What the code holds

Found by the matching runs (each run's notes are in `config/match/notes/<function>.md`; summed up here):

- **Handwritten assembly in the game's code.** splat marks 58 functions handwritten. More are, unmarked:
  - The GOOL interpreter around `func_8003A014` keeps its operand stack in `$s6` and has entries reached from its dispatch
    (`func_8003A680` is one: it pops operands off `$s6` and branches back into `func_8003A014`).
  - From `0x80042FD0` on, GTE routines with their own register conventions: inputs in `$t0`-`$t4` or `$v1`, returns through
    `jr $t8`, one falling through into the next (`func_80042FD0`, `func_800436C8`).

  None of these can match from C. A native port rewrites them in C that does the same work; the matching decomp keeps
  them as assembly.
- **`func_8003D2EC`** is a sine lookup with the shape of PsyQ libgte's `rsin` (a quarter-wave table of 0x401 `s16` at
  `D_8005BCB0`). Its unconditional branches are `b` where GCC 2.7.2 here writes `j`: another compiler or assembler setting
  for that part of the code, not settled (docs/COMPILER.md).

## The rest of the disc

Most of the game is data, not code. The `S0` to `S3` folders hold 38 levels' and other areas' `.NSF` files (the entries:
models, textures, sounds, and the GOOL bytecode that drives the objects) with their `.NSD` indexes. The executable holds the
engine: the GOOL interpreter, rendering, collision, the paging of NSF entries from the disc, and the SDK. CrashEdit documents
the formats (docs/REFERENCES.md).
