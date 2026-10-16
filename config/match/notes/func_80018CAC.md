# func_80018CAC

Walks a linked list (next at +0x48) calling func_80018AA8(node, arg1, arg2). Head is arg0+4 when arg0->kind == 2, else arg0+0x4C. Returns -0xFF.

Not matched: best.c (a3.c), 6 of 31 words differ. Control flow and registers match; only prologue scheduling differs
(retail stores $ra/$s2 then loads kind before moving a1 into $s1; ours moves a1 first).
Tried: while, for, ternary (22 differ), if/else with do-while, 5 min of permuter (no improvement).
