# func_8001EC2C

Converts a world position (s32 x,y,z at arg0) into camera space: loads the rotation matrix (D_80060774, or D_800607D4 when
arg2 != 0) and the translation (D_80060130), subtracts one of two camera positions (D_800607F4/8/C or D_8006083C/40/44),
shifts by 8 into an s16 vector, calls RotTrans, then scales the result (x<<8, -(y<<8), z<<8) and returns -0xFF.

Not matched: best.c (attempt5.c) is 64 of 77 words differ.

Tried:
- m2c shape with plain locals: the compiler dropped the stack copy of arg0; locals as arrays (`s16 sv[3]; s32 wv[3]; s32 out[2]`)
  keep it and give the retail frame size (0x40).
- 4th RotTrans argument is the camera z global held in $a3 (not 0).
- Temporaries for the final three loads (attempt5) gave the best count.
- Interleaving the subtractions inside each branch (attempt6) was worse (66).

Still differs: the order of the global loads and subtractions inside the two branches (retail interleaves stack and global loads,
and shares the tail through `j`), the placement of the `addiu $a0` lui/addiu pairs relative to the delay-slot `nop`s, and the
final load/shift/store scheduling.
