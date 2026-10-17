# func_8002FCF8

Level-load setup: sets state 2, points D_8005F374 at D_8005F3D8 + 0x1090, calls func_8003156C(block[1]), copies block fields to D_80060944, D_80063454->0xC and the six words at 0x1074 to D_8005F38C->8->0x30, sets D_8006CC84 = -1, calls func_8002F6C8(block + 0x10, ...) and clears D_8006CC00 and D_8006CBF0.

earlier.c failed to compile: the file declares D_8005F3D8 as s32. attempt1.c keeps it s32 and casts through a BLOCK macro (the block pointer is re-read after calls, as in the retail code).

Result: 500 of 500 trials agree on seeds 0 and 1 (about 385 of them by the same exception, calls compared only). Nothing still disagrees.
