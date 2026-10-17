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

- **The game's code ends in assembly.** From `0x80039D2C` to the SDK (`0x80046884`) the code is assembly source, not
  compiled C:
  - Its first functions (`0x80039D2C` to `0x80039F9C`) take the GOOL interpreter's registers as inputs (`$s0`, `$s3`,
    `$s4`, `$s6`, `$s7`, `$fp`), set by nobody they could be compiled with. The interpreter around `func_8003A014` keeps
    its operand stack in `$s6` and has entries reached from its dispatch (`func_8003A680` pops operands off `$s6` and
    branches back into `func_8003A014`).
  - From `0x80042FD0` on, GTE routines with their own conventions: inputs in `$t0`-`$t4` or `$v1`, returns through
    `jr $t8`, one falling through into the next (`func_80042FD0`, `func_800436C8`).
  - Its unconditional branches are all `b` (`bgez $zero`): 49 of its 97 functions have one and none has a `j`, while
    before it 242 of 466 functions have a `j` and none a `b`. No GCC from 2.5.7 to 2.8.1 writes `b` for one (their
    `jump` pattern is `j` only), at any of the flags tried (docs/COMPILER.md).
  - `func_8003D2EC`, a sine lookup with the shape of PsyQ libgte's `rsin` (a quarter-wave table of 0x401 `s16` at
    `D_8005BCB0`), looks compiled but is part of it: besides its `b`s, it keeps a branch to the very next instruction,
    which GCC removes at `-O1` and up, and `-O0` code looks nothing like it.

  splat marks 57 of its 97 functions handwritten, and none of the other 40 shows the compiler that built the rest. The
  build assembles it as it is (`asm/game/80039D2C.s`). splat marks two functions before it handwritten too
  (`func_8001856C`, `func_800186D8`). None of these can match from C: the matching decomp keeps them as assembly, and a
  native port rewrites them in C that does the same work.

## The game's source files

The game's code is split into 32 C files, `src/game/<first function>.c`, by what each function's instructions show of the
file they were compiled in:

- **Small data.** At `-G8` every variable of 8 bytes or less goes to `.sdata` (initialised) or `.sbss`, and the original
  assembler addresses one through `$gp` only from the file that defines it; from any other it is `lui`/`%lo`. So the
  functions reading a variable through `$gp` share a file, and one reading it through `lui` is in another: `D_8005F1A4` is
  read through `$gp` from `func_800123F0` to `func_8001521C`, through `lui` by `func_80012170` and `func_80015614`. Each
  file's `.sdata` follows the previous file's, so the variables' order is the files' order.
- **Short strings.** GCC puts a string literal of 8 bytes or less in `.sdata` too, local to its file, so the function
  taking its address is in the file that holds it. An address (`la`) never goes through `$gp`, even in that file.
- **Commons.** From `0x8005F320` the `.sbss` follows no file's order: uninitialised globals the linker placed. A file with
  a tentative definition (`int x;`) reads one through `$gp`, a file declaring it `extern` through `lui`, and several files
  can do the first. Below `0x8005F320` the `.sbss` is in file order (statics, most likely).
- **Rodata** (jump tables, longer strings) is in link order too: a function whose rodata lies before an earlier
  function's shares its file.

Handwritten code (from `0x8003A014` on, mostly) reads any variable through `$gp` and says nothing about files.

| File | Functions | With evidence | Evidence and data | Could start at |
| --- | --- | --- | --- | --- |
| `800111B0` | 9 | `80011800` | .sdata F17C-F190 (F17C: the first word of .sdata), .sbss F2C4, F2C8 | `800111B0` |
| `80011E24` | 7 | `80011E98`-`80011F1C` | .sdata F198, F1A0; `func_80011E24` reads F17C through `lui` | `80011E24` |
| `80012010` | 9 | `80012010`-`80012170` | rodata 0x80010000-0x80010068 | `80011F78` |
| `800123F0` | 46 | `800123F0`-`8001521C` | .sdata F1A4-F1D0; rodata 0x800100D0-0x800101B0 | `80012250` |
| `80015614` | 3 | `80015614`-`800157A0` | the common F3DC; `func_80015614` reads F1A4 through `lui` | `80015344` |
| `800158E0` | 46 | `800158E0`-`8001645C` | .sdata F1D4-F1E8; the commons F374, F3D8; rodata 0x8001029C | `80015828` |
| `80019098` | 10 | `80019098`-`80019BBC` | the common F38C, read through `lui` from `8001A4A4` on | `80016A60` |
| `8001A014` | 12 | `8001A014`-`8001A23C` | the common F3B8 | `80019D30` |
| `8001BFDC` | 7 | `8001BFDC` | rodata 0x800102BC | `8001A4A4` |
| `8001CEDC` | 5 | `8001CEDC` | rodata 0x800102D8 | `8001C29C` |
| `8001D63C` | 18 | `8001D63C` | .sdata F1EC, F1F0 | `8001D194` |
| `8001F9C4` | 6 | `8001F9C4` | .sdata F1F4, F1F8; the commons F3B0, F3EC; rodata 0x800102EC | `8001E3DC` |
| `800200A8` | 2 | `800200A8` | the commons F390, F398, F3D0 | `8001FDDC` |
| `80020A24` | 20 | `80020A24` | .sdata F1FC | `80020998` |
| `80023B04` | 19 | `80023B04`-`800260F4` | rodata 0x80010380-0x80010438 | `800217F8` |
| `8002648C` | 4 | `8002648C` | rodata 0x80010468 | `80026334` |
| `80026CA0` | 33 | `80026CA0` | rodata 0x80010480 | `80026764` |
| `80029884` | 27 | `80029884`-`8002E280` | .sdata F204; .sbss F2D0; the commons F378, F3C4, F3E8; rodata 0x800104AC | `80026EF4` |
| `8002E884` | 7 | `8002E884` | .sdata F208 | `8002E628` |
| `8002FB84` | 7 | `8002FB84` | .sdata F210, F214 | `8002EE48` |
| `8002FE58` | 23 | `8002FE58` | .sdata F218 | `8002FC78` |
| `8003147C` | 3 | `8003147C`-`8003156C` | .sdata F220 | `8002FF54` |
| `80031578` | 4 | `80031578`-`80031658` | .sdata F224 | `80031578` |
| `800316EC` | 1 | `800316EC` | .sdata F228-F240 | `80031668` |
| `80031AE8` | 26 | `80031AE8` | rodata 0x80010544 | `80031AE8` |
| `800324A8` | 23 | `800324A8`-`800344B0` | .sdata F244-F260; .sbss F2E0-F308; the commons F39C, F3E0, F3E4; rodata 0x80010590-0x80010668 | `800321D4` |
| `800345C8` | 64 | `800345C8`-`80036C40` | .sdata F264-F28C; .sbss F314-F31C; the commons F3A0-F3CC; rodata 0x8001067C-0x80010704 | `800345C8` |
| `80036D0C` | 1 | `80036D0C` | .sdata F290; rodata 0x80010750 (below) | `80036CF8` |
| `80036E30` | 5 | `80036E30` | .sdata F294-F2A0 | `80036E30` |
| `80037698` | 2 | `80037698`-`8003817C` | rodata 0x80010804-0x800108F8 | `80036FAC` |
| `80038700` | 5 | `80038700` | rodata 0x80010918 | `80038700` |
| `80038F00` | 12 | `80038F00` | rodata 0x80010938; ends where the game's assembly part starts (above) | `80038AFC` |

F-numbers are addresses `0x8005Fxxx`. Functions with no evidence between two files go with the earlier one, and so does
small data no function reads through `$gp`: a file starts at its first function with evidence, and could start as early as
the last column. Neighbouring files with nothing setting them apart may be one file in the original; they stay apart until
a function shows otherwise. Whatever the boundaries, the bytes are the same: they matter to a function written in C, which
reads through `$gp` exactly the data its file defines.

## The game's read-only data

From `0x80010000` to `0x80010950` the `.rodata` is the game's files', in their order; the libraries' follows. In each file
that has both, its constants (strings) come first, then its functions' jump tables in function order: the order GCC writes
them in (docs/COMPILER.md, "Read-only data"). So `config/SCUS_941.54.yaml` gives each of these files its own `.rodata`
subsegment, splat moves each jump table into its function's assembly (with `INCLUDE_ASM`, or with the `switch` once the
function is in C), and the constants, marked `force_not_migration` in `config/symbol_addrs.txt`, are included at the top
of their file with `INCLUDE_RODATA`, in address order. C reads them as `extern`.

`D_80010750` (`0x80010750`-`0x80010804`) is eleven strings no code reads, `"----stack pop"`, `"-----ireg ref"`,
`"-----pool ref"` and the like: the kinds of operand `func_80036D0C` resolves, so it goes with that file. Any file from
`800345C8` to `80036E30` gives the same bytes.

## The rest of the disc

Most of the game is data, not code. The `S0` to `S3` folders hold 38 levels' and other areas' `.NSF` files (the entries:
models, textures, sounds, and the GOOL bytecode that drives the objects) with their `.NSD` indexes. The executable holds the
engine: the GOOL interpreter, rendering, collision, the paging of NSF entries from the disc, and the SDK. CrashEdit documents
the formats (docs/REFERENCES.md).

## Addresses from the disc

The disc's data seems to name some of the executable's variables by address. Heap objects read through `CdGetSector`
hold 0x8005F340, inside `D_8005F338` in `.bss`, and keep it whatever the executable's layout; the disc image holds that
word 21 times. Moving the data breaks the game where moving the code doesn't (`tools/run_game.py`, PCSX-Redux):

- With the default build's C and 8 bytes of padding at the start of `.data`, the display differs from retail's from
  frame 1,390 and the game crashes in the first warp room (frame 4,124, a jump to 0x68); 32 bytes crash at frame 1,941.
  The same padding in the code gives the same frames and crashes, since it moves the data too.
- Moving only the heap's start changes nothing. With the data in place, the equivalent build's code, 568 bytes shorter
  and everything after its first equivalent function moved, displays what retail does up to a difference of timing
  (docs/ROADMAP.md, section 4).

The equivalent build keeps the data at its retail addresses (docs/WORKFLOW.md, "The equivalent track"). Which entries
hold such addresses, and what the engine does with them, is still open.
