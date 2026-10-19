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

Handwritten code (from `0x8003A014` on) says nothing about files: it reads the small data through `lui`, and its `$gp`
is one more register, not the small data's base (below, "Addresses from the disc").

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

No address of the executable turned up in the disc's data, and none holds `.data` in place. What breaks the game when
`.data` moves is the handwritten code's `$gp`, which splat reads as the small data's base where it is a pointer to a
structure.

- **The disc.** Read entry by entry (the 38 `.NSF` files' pages but the texture pages, their entries' items), 7 aligned
  words in the whole disc equal a symbol's address (0x80010000 twice), in three entry types at unrelated items and
  offsets: chance bits. 0x8005F340, which heap objects read through `CdGetSector` hold, comes from one type 1 entry of
  `S0/S000003C.NSF`: 21 identical frames hold it 48 bytes in, past the frame's 0x18-byte header, in its vertex data.
  The `.NSD` files hold none.
- **The handwritten `$gp`.** From `0x8003A014` on, the code loads `$gp` from the stack, a structure or the scratchpad,
  computes with it, and reads through it at offsets from 0x0 to 0x118. splat writes 23 of those 43 accesses as
  `%gp_rel` of the small variable the offset would reach from the retail `_gp` (`g_RandSeed`, `D_8005F1DC` to
  `D_8005F1F0`), and leaves 20 as numbers. The bytes are the same at the retail layout; with `.data` moved and `_gp` at
  its retail value, the linker moves those 23 offsets with the variables and the code reads its structures off by the
  shift. At `0x80043788`, `sw $t9, %gp_rel(g_RandSeed)($gp)` writes into the heap, `$gp` 0x8009F8DC.
- **Evidence** (`tools/run_game.py`'s schedule in PCSX-Redux, the default build's C, the RAM dumped from the emulator):
  - In a retail run, no handwritten instruction reads 0x8005F17C to 0x8005F2A4 through `$gp` over the 8,000 frames (a
    read watch: its two handwritten readers load through `$at`).
  - `.data` 8 bytes later, the 23 accesses as they are: the display differs from frame 1,390 and the game stops between
    frames 4,038 and 4,178 (four runs). Against a run with `_gp` moved along, the data and heap are the same every five
    frames up to 1,385 (but for one word, 8 apart, at frame 1,355); in frame 1,385 the handwritten code's projected
    vertices differ, and the write at `0x80043788` lands 8 bytes off.
  - `.data` 8 bytes later, the 23 accesses written as their numbers: the same display as at the retail layout on all
    8,000 frames, and at frames 1,389, 1,500, 2,000 and 2,500 the same RAM as with `_gp` moved along but the word where
    the SDK saves `$gp`. The same display with 32 bytes and with 1 KB; moving `_gp` with the small data (8 bytes) gives
    it too.
  - The equivalent build without its padding before `.data` (`.data` 32 bytes earlier) and with the 23 numbers displays
    what the equivalent build at the retail layout does on all 8,000 frames.
- **What bounds the executable then**: its file's 160 sectors on the disc (327,680 bytes, the header's 2,048 included).
  The retail image's data ends at `0x8005F2A4`, 1,372 bytes before the file's end; `tools/patch_disc.py` refuses a
  larger executable (12 KB more was refused).

The equivalent build writes the 23 offsets as numbers and lets its data follow its code (docs/WORKFLOW.md, "The
equivalent track").

## Code addresses read as data

One handwritten renderer reads bytes of code addresses, so what it draws depends on where the code sits. The renderer
entry `func_80042818` (one of `func_80042420`'s, which `func_80011E24` stores) picks a polygon's texture level by its
depth: `(SZ1 + SZ2 + SZ3) >> 11` indexes a byte table at scratchpad 0xD0, and the byte is a shift (`srlv`, its low 5
bits) that selects 2 bits of a texture word, which pick the 12-byte entry the polygon is drawn with. `func_80041E5C`
copies the table there, 32 bytes from `D_8005C724` (shifts 29 down to 15), but the index can reach 95 with three far
vertices. Past 0xEF the scratchpad holds what `Gool_InitScratchpad` copied from 0x60 on, `g_GoolOpcodeHandlers`, the
handwritten GOOL operations' addresses: index 36 reads 0xF4, the low byte of entry 37, `D_8003A704`, a shift of 4 in
retail.

Evidence (`tools/run_game.py --schedule turtle-woods`, PCSX-Redux on the game clock):

- The equivalent build differs from retail from game frame 3,524, first in the heap at 0x8007A770, a polygon of the rain
  `func_80042818` writes from `0x800428F8`, the entry's second word 0x0211207F in retail, 0x0213207F there.
- The equivalent build's C at the retail data addresses and its handwritten code 0x14 bytes earlier differs at the same
  frame; `.data` moved alone doesn't, nor the code moved by 0x20 bytes (the low 5 bits of the byte kept), while 0x8,
  0x10 and 0x14 do.
- In frame 3,523, the writes to scratchpad 0xD0 to 0xF7 before the renderer's 301 reads come from `func_80041E5C`'s
  copy, 0xD0 to 0xEF only. At the end of frame 3,524, 47 of the scratchpad's words from 0x60 to 0x13C differ between
  the two runs, each a code address 0x14 apart, most of them the opcode table's entries.
- Restoring retail's word at 0xF4 around the read alone (`D_8003A704`, set before the `lbu` at the shifted address and
  put back after it), the shifted build displays retail's image at every one of the 6,001 game frames.

A build that moves the handwritten GOOL operations draws far polygons with other texture bits; an equivalent rewrite of
`func_80042818` reproduces retail only by reading the same bytes past the table.
