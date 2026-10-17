# func_8002FCF8

Level-load setup: sets `D_8006CD14 = 2`, points `D_8005F374` at `D_8005F3D8 + 0x1090`, calls `func_8003156C(block[1])`,
copies `block->0xC` to `D_80060944`, `block->0x1070` to `D_80063454->0xC` and the six words at `block + 0x1074` to
`D_8005F38C->8->0x30`, sets `D_8006CC84 = -1`, calls `func_8002F6C8(block + 0x10, ...)` (a1..a3 are the words at 0x1088,
0x107C, 0x1080, still live in the registers) and clears `D_8006CC00` and `D_8006CBF0`.

Attempt 1 (typed structs, plain field copies): 500 of 500 trials agree on seeds 0 and 1. Nothing still disagrees.
Note: the retail code reads `D_8005F3D8` again after the first call; the C does the same.
