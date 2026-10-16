# func_8002FDCC

Clears D_800675AC, calls `func_80014260(arg0, 1, 1)`, stores the magic 0x6396347F into `*arg0`, copies word 4 of the
result into D_8005F3D8, builds a 2-word stack buffer {0, 0x6400}, calls `func_80019800(&D_8006CB20, 4, 8, 2, buf, 1)`
and then `func_8002FCF8()`.

Not matched. The `best.c` count is the permuter's best (the score run below confirms it).

Tried:
- earlier.c (buf[2] array): 5 of 35 differ.
- attempt2 (load into a local before the store): 8. attempt3 (load before the store, in source order): 16.
- 5-minute permuter run on attempt1: 4 of 35 (temporaries around `*arg0 = magic`).

Still differs: scheduling of the `lui/addiu $a0` for `&D_8006CB20`. Retail emits it after `sw magic` and `lw 0x10($v0)`;
the compiled code hoists it above them.
