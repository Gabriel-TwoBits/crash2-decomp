# func_8002FCF8 notes

Level-load setup: sets D_8006CD14 = 2, points D_8005F374 at D_8005F3D8 + 0x1090, calls func_8003156C(block[1]), copies
block fields to D_80060944, D_80063454->0xC and the six words at 0x1074 to D_8005F38C->8->0x30, sets D_8006CC84 = -1,
calls func_8002F6C8(block + 0x10) and clears D_8006CC00 and D_8006CBF0.

The earlier run was refused on one trial: it reloaded D_8005F38C and its field after each store, so a store into
D_80063454->0xC (or into the destination) could change a later pointer. This version follows the assembly's
order: D_8005F38C is read before the store to D_80063454->0xC, its ->8 field after it, and the destination
pointer is held in a local for all six stores.

Result: 500/500 (seed 0), 2000/2000 (seeds 1 and 2). Nothing disagrees.
