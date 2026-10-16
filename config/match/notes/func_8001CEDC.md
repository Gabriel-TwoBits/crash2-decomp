# func_8001CEDC

Dispatch on `b->kind` (0xC): 0 and 4..5 fall to the common tail; 1 tests object-vs-object overlap (func_80030EC0), 2 point-in-box
(func_80030D98), 3 builds a box from `b->e` fields (+-0x60/0x90 pairs) and tests it, then every 5th hit past 2 skips the tail.
Cases 1..3 require `a->p->q->kind` to be 0x300 or 0x400. Common tail: `b->count++`, then `func_8001CC10(b->e, a, b->f4, b->f8, b->f14)`.
Always returns -0xFF.

Best: `best.c` (attempt2), 23 of 174 instructions without a counterpart, 120 of 174 words differ.
Explicit `case 4: case 5:` produced the jump table. Still differing: a reused temporary `v` is kept in `$s0` in cases 2/3
(retail uses `$v0`/`$a0`); in case 1 retail does `addiu $a1,$v0,8` after the null check on the first result. Separate temporaries per
case (attempt3) fixed the move ordering in case 1 but scored worse overall (26). Permuter not run.
