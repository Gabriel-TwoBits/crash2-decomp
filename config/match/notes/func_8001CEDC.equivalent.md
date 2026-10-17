# func_8001CEDC notes

Dispatch on `b->kind` (0xC): 0, 4, 5 and anything above 5 go to the common tail; 1 tests object-vs-object overlap
(func_80030EC0), 2 point-in-box (func_80030D98), 3 builds a box from `b->e` (+-0x60/0x90 pairs) and tests it, then from the
third hit on every 5th skips the tail. Cases 1..3 need `a->p->q->kind` to be 0x300 or 0x400. Tail: `b->count++`, then
`func_8001CC10(b->e, a, b->f4, b->f8, b->f14)`. Always returns -0xFF.

best.c = earlier.c without its own `func_8002C324` prototype (it conflicted with game.h's `s32 *`). Re-read against
target.s branch by branch (case 1 does not null-check the second lookup; a nonzero test result continues to the tail).

Not scored: score.sh stops with `placed/8001CEDC.c:92: 'func_8001CEDC' redeclared as different kind of symbol`. The file has
`extern char func_8001CEDC;` (used as `&func_8001CEDC` at lines 103/116), which conflicts with any C definition of the
name. Landing needs that declaration changed first. Final count: 0 of 0 trials (no run).
