# func_80035CBC

Builds a path (strcpy + strcat of D_8005F284 into a 0x80-byte local), calls firstfile on it with an entry pointer, then walks
entries of 0x28 bytes with nextfile while they match, up to 16, and returns the count (0 if firstfile's result differs).

Not matched: best.c (attempt 1) is at 11 of 35 words differ. The target sets s0 = 0 before firstfile and increments it
(`addiu s0,s0,1`) at loop entry; every variant lets the compiler fold the entry count into `ori s0,$zero,1`, and the
loop shape (increment in the beq delay slot) is not reproduced. The m2c goto form scored 16, a `for (n++; ...)` form 33
(register swap s0/s1). Not tried: permuter.
