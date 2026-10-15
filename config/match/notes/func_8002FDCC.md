# func_8002FDCC

Initializes a 2-word buffer {0, 0x6400} on the stack, stores a magic (0x6396347F) into `*arg0`, copies word 4 of
`func_80014260(arg0, 1, 1)`'s result into D_8005F3D8, calls func_80019800(&D_8006CB20, 4, 8, 2, buf, 1), then func_8002FCF8().

Final: best.c, 6 of 35 words differ. The stack data must be a 2-element array (m2c's two scalars made the 0x6400 store a dead store).
Remaining difference: scheduling of the `sw` to `*arg0`, `lw 0x10(v0)` and the `lui/addiu $a0` argument setup (retail puts the store
and load first, compiled hoists the `$a0` setup). Reordering the statements made it worse (10); a 4-minute permuter run reached 5.
