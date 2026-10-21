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
  - `Math_Sin` (`0x8003D2EC`), a sine lookup with the shape of PsyQ libgte's `rsin` (a quarter-wave table of 0x401 `s16`
    at `g_SinTable`), looks compiled but is part of it: besides its `b`s, it keeps a branch to the very next instruction,
    which GCC removes at `-O1` and up, and `-O0` code looks nothing like it.

  splat marks 57 of its 97 functions handwritten, and none of the other 40 shows the compiler that built the rest. The
  build assembles it as it is (`asm/game/80039D2C.s`). splat marks two functions before it handwritten too
  (`func_8001856C`, `func_800186D8`). None of these can match from C: the matching decomp keeps them as assembly, and a
  native port rewrites them in C that does the same work.
- **Tails after their function.** `Sys_Memcpy` copies `a2` halfwords (the card directory's 0x28-byte entries go with
  0x14) and ends on a dispatch to `D_8003BB94 + a2 * 32` for its last 1 to 4, into the four 32-byte tails right after it
  (`Sys_Memcpy1` to `Sys_Memcpy4`, which use its `$a0`, `$a1` and `$at`); a count below 0 jumps backwards into code. The
  five tails before it use `$s1`, `$s2` and `$t6`, and are `Gfx_DecodeSortListBackward`'s: it ends on the same dispatch, from
  `D_8003B87C`. The Crash 1 decomp's catalog puts the tails before their functions, which first named these the other
  way round. A halfword loop at `0x8003BC34`, after the tails, is reached by no dispatch (`a2` is at most 4 there).
  `Gfx_ResetOt` has no tails: 64 entries a pass, the terminator stored last.
- **What a caller keeps.** `Gfx_DrawWorlds` never sets `$v0` and returns what its last call left, which
  `800111B0.c` stores in `D_8006CCB8`; on its path through `Gfx_ResetOt`, which leaves `$v0` alone, that is
  `func_8002FB84`'s result. A handwritten caller can rely on any register its callee's retail code doesn't write.

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

The sort lists (SLST entries, type 4) give a zone's drawing order, one per camera position: an item whose second
halfword has bit 0 clear is a whole list (`func_80029800` copies it), else a delta the handwritten `Gfx_DecodeSortListForward`
applies forward and `Gfx_DecodeSortListBackward` backward, into one of `Gfx_AllocSortLists`'s two 0xBE4-byte buffers. Read from
the disc's NSF pages (every one uncompressed, magic 0x1234): 3,980 whole lists, the largest of 1,026 entries; 61,960
deltas, the largest 0x638 bytes, no header end past 794. Of their swaps, 1,315,110 flip bits, 313,226 are short,
52,141 long and 62,033 near, a near one always after a short or long one (47,227 and 14,806), never first or after a
flip: the retail code takes a near swap's base from the swap before it, where a flip leaves an address instead.

A zone's collision octree is its entry's (type 7) second item: origin and size per axis, the root node at 0x1C, then
how many times each axis halves (`Octree` in `src/game/80039D2C.c`). `Zone_QueryOctree` walks one into a collision list,
keeping the nodes above a leaf in 7 registers (a `break` past them), and loops forever on a node with children where
no axis halves. Read from the disc's 1,843 zones: x and y halve at most 7 times, z 6, the deepest leaf is 7 levels down,
and no node with children sits past every axis's halvings; sizes reach 15,221 (a halfword holds them, as the list's
header does) and origins lie between -234,962 and 417,902 (shifted by 8 they still fit a word).

A camera path's property 0x1B5 sets the streaks `Gfx_DrawStreaks` draws, which
`func_80020A24` alone interpolates into `D_80062A6C` to `D_80062A90`: on the disc, 490 rows of 6 signed halfwords in 6
NSF files, the fourth (the count) 1 to 1,500, the fifth (the box's y offset) -4,000 to 0, the sixth (the slot in the
ordering table) 10 to 1,904. Property 0x2AA sets the dots `func_80045218` draws (`D_80062A94` to `D_80062AA0`): 65 rows
of 4, the count always 1,000, the y offset 0 to 2,000, the slot 0 to 900, the y shift 0 to 2.

## The world renderer

`Gfx_DrawWorlds` draws the zone's world polygons each frame (`Sys_MainLoop` and `func_80018434` call it, with the sort
list, the frame being built, `D_80062A18` and the textures' clock `D_80060944`). What its code shows:

- The sort list's entries are drawn from the last to the first; a list of one entry draws nothing and leaves the
  ordering table reset (`Gfx_ResetOt`). An entry is a world (bits 13 to 15), a kind (bits 11 and 12) and an index:
  kind 0 a triangle of the world's triangle table (its word below the table's address, its halfword above), the others
  an 8-byte record of its quad table, read as the triangle of its first three corners, the triangle of its last three
  (its texture's corners turned) or the quad.
- `Sys_MainLoop` copies the worlds from the zone's first item into the scratchpad from 0x280, 32 bytes each: the
  world's position from the camera (a VXY pair and z), its triangle, quad, vertex, colour, animation and texture tables.
  A vertex's word holds x and y (a VXY pair, less their low 4 bits), its halfword z (less its low 4); those 12 bits
  and the word's bits 16 and 17 index its colour; bits 18 and 19 are what the modes read.
- A polygon's material is its first word's low byte and its second's low 4 bits: 0 draws it shaded only (G3, G4),
  otherwise textured (GT3, GT4, opaque unless the texture's page blends). Bit 11 makes it an animation's index: a frame
  by the clock, or (bit 31 of the animation's word) a level by the polygon's depth, 2 bits a level at a shift from the
  byte table at `D_8005C724`, which the renderer copies to the scratchpad at 0xD0 (and reads past for the farthest
  polygons: "Code addresses read as data").
- The polygons come sorted, far first: the ordering table is built from its top slot down, a polygon linked into the
  lowest slot so far unless its own depth's slot is lower, the slots between linked to the ones above.
- The code is pipelined: the projector reads the next polygon's record while the GTE projects the current one, and
  the dispatcher, projectors and drawers share registers, jumping into one another. The C keeps that one read's order,
  where a record and the frame's primitives may share memory in random trials.

The scene chooses one of 10 modes (`func_80011E24`'s default, `func_80020A24`'s 9 by the camera's flags), five entries
each: a setup the dispatcher calls once, a triangle and a quad projector, a triangle and a quad drawer.

| Mode | Chosen by | What it does |
| --- | --- | --- |
| default | no flag | projects, culls off the screen, draws shaded or textured polygons |
| fog | 0x200000 with `D_8006CDC4` | fogs each corner by the scene's depth (DPCS), bright or dark by bit 18 |
| depth fog | 0x20, or 0x200000 without | fogs corners past a depth by how far (property 0x1DE), black flat polygons where all are black |
| tint | 0x8000 | moves bit 19's corners toward a colour |
| pulse | 0x40 | pulses bit 19's corners with the clock, or dims by depth when the first vertex has bit 18 |
| lamps | 0x100 | lights corners from two objects' positions (DPCS, DPCL), tints bit 19's, keeps bit 18's |
| glow | 4 or 0x800000 | dims corners by their distance from two objects, a polygon far from both black and flat |
| waves | 0x200 | moves bit 18's vertices along y by a table at scratchpad 0x100, dims polygons whose vertices all have bit 19 |
| sway | 8 | moves bit 18's vertices by a sine (property 0x1B8), recolours bit 19's |
| light | the game's flag 0x1000000 | divides polygons near the player into a lit grid of 45 points from two 16 by 16 maps |

The retail code keeps its state in the scratchpad between 0x60 and 0x230, over the GOOL opcode table's copy; the C
keeps the same words (`WorldScratch`).

## The object renderers

`Gfx_DrawObjects` draws the frame's packet list (`Sys_MainLoop` calls it with the list `func_8001ABF8` builds in the
frame, the frame and `D_80062A18`). What its code shows:

- A packet holds the next packet, three steps the root calls in turn, the object, its matrix (`func_8004400C` and the
  three builders after it, through the table at 0x8005B650, turn it by the object's angles and scale it) and what the
  steps read. The first step (the table at 0x8005B690, by the object's kind) puts the object's rotation and
  translation into the GTE: in the camera's view, facing the screen (its second row scaled by -0.625, its third
  negated), on the screen's plane, lit (a light matrix from two vectors), fading past the scene's fog start. The second
  (0x8005B75C) sets the level and colour shift and mask the drawers read, or how much the scene's two lamps light the
  object. The third is the drawer (0x8005B708), the second and third paired by mode; morphing models (two frames
  blended) take the next index's pair.
- A sprite (kind 2) is a textured quad of 200 << its size about the object; kind 5 a list of quads per frame. A model
  (kind 1) is triangle strips: a command word per vertex (from the scratchpad's cache, or read from the frame's
  stream: three bytes, or delta fields of 1 to 8 bits as a table of widths gives them), its colour from the model's
  table or the scratchpad's copy, its triangle's texture (an animation's frame by the clock), and whether the strip
  turns about its first vertex. A visibility test (whether a corner of the frame's box is on the screen) and a far
  colour step run first; each triangle goes through a colour routine (G3 or GT3 faded toward the far colour by DPCT,
  translucent, culled off the screen's edges, glowing by depth, or a lit FT3 with a highlight from `D_8005C650`).
- The drawers: whole; clipped at a level (the parts below and above at two depths of the ordering table, the cut
  points interpolated, above's colours shifted and masked); reflected (the cut points white, the reflection darkened
  and moved 3 pixels); one side kept; mirrored (each triangle again reflected in a level, its facing turned); lit (the
  vertices turned by the object's rotation and cut in the world's axes, each piece projected).
- The steps share registers and the scratchpad's words from 0 to 0x140 (and the vertex cache from 0x140), which the C
  keeps (`DrawPass`, `DrawScratch`); a colour routine's leftover `$a2`, which the next call reads as the texture's
  offset, too. The handwritten code's `$gp` is the object (its `%gp_rel` accesses are its fields).

## The GOOL interpreter

`Gool_Interpret` runs an object's GOOL code from its pc until an instruction ends the run (its callers: `Obj_Update`,
`Gool_ChangeState`, `Gool_SendEvent`, `func_8001D22C`). What its code shows:

- The machine lives in registers its operations share: the object (`$s0`), the run's mode and argument (`$s1`, `$s2`),
  the constant pools of the object's GOOL entry (`unk_10`'s item 2) and of its state's (`state_exe`'s), the pc, the
  stack and the frame (`$s5` to `$s7`, stored back to the object around each call into the game's C and at the end),
  and the scratchpad's base in `$fp`. The stack and the frames lie in the object: a frame (`Gool_PushFrame`'s too)
  holds the caller's mode and pc, then the stack's start and the caller's frame as offsets from the object's
  registers (0x40).
- An instruction is a word: the operation in its top byte, then two 12-bit operands (A in bits 12 to 23, read first,
  and B) or an 11-bit destination. An operand is a constant pool's word, a value the instruction holds (9 bits times
  256 or 8 bits times 16), a frame's word, 0 or 1, or a register: the object's own (0x40 + index * 4, 0x1F the stack's
  top) or one of a linked object's (its 8 links at 0x40).
- 79 operations, by the table at 0x8005C514 (`g_GoolOpcodeHandlers`, which `Gool_InitScratchpad` copies to the
  scratchpad's 0x60 and points the word at 0x5C to): arithmetic, comparisons and bitwise operations pushing their
  result, moves to a destination, seeks, the sine, an ease, a random number (the generator of `Gfx_DrawStreaks`'s
  streaks, its words at `D_8005F29C`), branches, calls and returns, and 27 that call 23 functions of the game's C
  (`Gool_Op*`), most with the object and the instruction, the 10 that may end the run with the mode and the argument
  too.
- Operation 73 runs MIPS code that follows the instruction in the GOOL entry, on the machine's registers. The disc's
  GOOL entries hold 8,572 such blocks (read entry by entry, every path from each block's start followed to its return):
  they read only `$s0`, `$s3`, `$s6`, `$s7`, `$fp` (the scratchpad's 0x50 to 0x58) and `$ra`, write the stack, `$s5`
  and temporaries, call nothing, and return with the address to go on from in `$s5` (`jalr $s5, $ra`, 7,226 of them) or
  0 to return (`jr $ra`, `ori $s5, $zero, 0` in its delay slot, 1,346). The C calls them with those registers set.
- Division is the CPU's: by 0 a quotient of -1 (1 for a negative dividend) and the dividend left over, where GCC's
  code traps (`break 7`), so the C computes those results itself. equiv.py's emulator divides by 0 otherwise (the
  dividend as the quotient, a remainder of 0), so its trials leave a zero divisor out.
- A return with no caller's frame ends the run with -0x19; an event's handler's return with -0x1A, or `SUCCESS` with
  0xFF stored at the argument when the mode's bit 0x20 accepts the event.

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
bits) that selects 2 bits of a texture word, which pick the 12-byte entry the polygon is drawn with. `Gfx_DrawWorlds`
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
- In frame 3,523, the writes to scratchpad 0xD0 to 0xF7 before the renderer's 301 reads come from `Gfx_DrawWorlds`'s
  copy, 0xD0 to 0xEF only. At the end of frame 3,524, 47 of the scratchpad's words from 0x60 to 0x13C differ between
  the two runs, each a code address 0x14 apart, most of them the opcode table's entries.
- Restoring retail's word at 0xF4 around the read alone (`D_8003A704`, set before the `lbu` at the shifted address and
  put back after it), the shifted build displays retail's image at every one of the 6,001 game frames.

A build that moves the handwritten GOOL operations draws far polygons with other texture bits; an equivalent rewrite of
`func_80042818` reproduces retail only by reading the same bytes past the table. The equivalent build keeps the
table's entries: its C defines the GOOL operations' labels at their retail addresses (`RETAIL_ADDRESS` in
`src/game/80039D2C.c`), so the table and its copy hold retail's words wherever the code lies, and `Gool_Interpret`
runs each opcode's operation itself, not through the table.
