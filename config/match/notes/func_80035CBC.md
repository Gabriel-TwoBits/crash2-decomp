# func_80035CBC

Builds a path (strcpy + strcat of D_8005F284 into a local buffer), calls firstfile on it with an entry pointer, then walks
entries of 0x28 bytes with nextfile while they match, up to 16, and returns the count (0 if firstfile's result differs).

Not matched: best.c (a1.c) is at 11 of 35 words differ. The target sets s0 = 0 before firstfile and does `addiu s0,s0,1`
after the bne; the compiler folds this to `ori s0,$zero,1` and moves the increment out of the beq delay slot. Tried: for(;;),
while, do-while, `return n` inside the loop (26-32 differ, mostly s0/s1 swaps), permuter 4 minutes (no gain).
