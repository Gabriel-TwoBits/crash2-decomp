# func_8002FCF8 notes

Level-load setup: sets D_8006CD14 = 2, points D_8005F374 at D_8005F3D8 + 0x1090, calls func_8003156C(block[1]), copies
block fields to D_80060944, D_80063454->0xC and the six words at 0x1074 to D_8005F38C->8->0x30, sets D_8006CC84 = -1,
calls func_8002F6C8(block + 0x10) and clears D_8006CC00 and D_8006CBF0.

Fix over the earlier draft: func_8002F6C8 takes one argument (game.h prototype `void func_8002F6C8(u8 *)`), and the
callee prototypes are no longer redeclared. D_8005F3D8 stays s32, cast through BLOCK.

Result: 500 of 500 trials agree on seeds 0 and 1. Nothing disagrees.
