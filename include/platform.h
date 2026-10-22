#ifndef PLATFORM_H
#define PLATFORM_H

/* What a host build (crash2-pc, docs/ROADMAP.md section 6) needs other than the PlayStation: each macro holds the
 * PlayStation's value unless the build defines it first. tools/equiv.py's contracts keep their numbers */

/* The scratchpad, the CPU's 1 KB of fast RAM, by its address */
#ifndef SCRATCHPAD
#define SCRATCHPAD 0x1F800000
#endif

/* KSEG0, where the CPU sees the RAM cached */
#ifndef RAM_BASE
#define RAM_BASE 0x80000000
#endif

/* A word-aligned address in KSEG0's first 8 MB, which the game tells apart from an EID or a tagged value */
#ifndef RAM_ADDRESS
#define RAM_ADDRESS_MASK 0xFF800003
#define RAM_ADDRESS(value) (((value) & RAM_ADDRESS_MASK) == RAM_BASE)
#endif

/* An ordering table's or a primitive's link to the next: the low 24 bits of its address, as the GPU's DMA reads them */
#ifndef OT_LINK
#define OT_LINK(address) ((u32)(address) & 0xFFFFFF)
#endif

/* On entering Gfx_EndFrame, which ends a game frame: where a host keeps a game clock, as tools/run_game.lua's
 * breakpoint does there */
#ifndef FRAME_ENDING
#define FRAME_ENDING()
#endif

/* A C symbol's name for the assembler: as it is on the PlayStation (MIPS ELF); a host whose C symbols take a prefix
 * (32-bit Windows' leading underscore) names them its way */
#ifndef ASM_SYMBOL
#define ASM_SYMBOL(name) #name
#endif

/* A call's arguments that the PlayStation's code passes on in the registers they arrived in, unnamed (the call has
 * none there), and a host names */
#ifndef PASSED_ON
#define PASSED_ON(arguments...)
#endif

/* Defined by a host only, with no PlayStation default:
 * - GTE_READ_DATA and the GTE's five other register moves (include/gte.h), on registers in the host's memory;
 * - GOOL_RUN_NATIVE(object, state_pool, &pc, &sp, frame), which runs operation 73's native code (MIPS) on the host's
 *   interpreter, where the PlayStation's CPU runs it itself (80039D2C.c, gool_native);
 * - GOOL_HOST_OPERATION(object, word), the operation of an opcode the game's code holds none of, which the host's
 *   patches of that code run (80039D2C.c, Gool_Interpret): the object's stack and frame stored, its pc on the next
 *   instruction;
 * - GTE_HORIZONTAL(offset), in the GTE model's projection (include/gte.h, which defaults it to the offset as it is):
 *   a vertex's horizontal offset from the screen's centre, which a host narrows to show more of the world across;
 * - GTE_PROJECTED(sxy, view, h, ofx, ofy), after the GTE model projects a vertex (include/gte.h, Gte_Perspective): the
 *   screen position it pushed, the vertex's view coordinates before their rounding (three s64 with 12 fraction bits),
 *   the projection's distance and offsets, for a host to keep the position's fraction and depth */

#endif /* PLATFORM_H */
