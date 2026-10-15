# func_80018CAC

Walks a linked list (next at +0x48) and calls func_80018AA8(node, arg1, arg2) on each node. The head is arg0->child (+4)
when arg0->kind (+0) == 2, else arg0->first (+0x4C). Returns -0xFF.

Not matched: best.c, 6 of 31 words differ. Control flow and registers match; only the prologue scheduling differs
(retail saves $ra/$s2 before moving a1 into $s1 and loads kind between; ours moves a1/a2 first).
Tried: while/for, if-else vs default-then-override, raw s32* vs struct, 4 min of permuter (no improvement).
