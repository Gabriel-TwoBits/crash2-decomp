# func_8002FCF8

Initialises a level-load state: sets D_8006CD14 = 2, points D_8005F374 at D_8005F3D8 + 0x1090, calls func_8003156C(D_8005F3D8[1]), copies
fields 0xC and 0x1070..0x1088 of the D_8005F3D8 block into D_80060944, D_80063454[3] and D_8005F38C[2]+0x30..0x44, sets D_8006CC84 = -1,
calls func_8002F6C8(D_8005F3D8 + 0x10, ...) and clears D_8006CC00 and D_8006CBF0.

Not matched. Best: best.c (attempt3), 42 of 53 words differ. Baseline m2c draft: 33 of 53 as written (with M2C macros), but the
typed rewrites scored 42-47.

Tried: s32* typed globals (43), char* typed globals (42), a local copy of D_8005F3D8 before the call (47), a struct-free pointer-arithmetic
form (47).
Still differs: the target loads D_8005F3D8 once at entry into $v0 (reused for the 0x1090 add and field 4) and keeps the second copy in
$v1 after the call, with $a0-$a3 as load temporaries before the stores. Register assignment and load/store scheduling differ throughout.
A sed-based attempt5 (local char *p) was blocked by the permission prompt and not scored.
