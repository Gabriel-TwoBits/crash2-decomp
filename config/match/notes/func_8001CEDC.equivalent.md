# func_8001CEDC

Dispatch on `b->kind` (0xC): 0, 4 and 5 go to the common tail; 1 tests object-vs-object overlap (func_80030EC0), 2 point-in-box
(func_80030D98), 3 builds a box from `b->e` (+-0x60/0x90 pairs) and tests it, then past 2 hits every 5th skips the tail.
Cases 1..3 require `a->p->q->kind` to be 0x300 or 0x400. Common tail: `b->count++`, then
`func_8001CC10(b->e, a, b->f4, b->f8, b->f14)`. Always returns -0xFF.

`best.c` is `earlier.c` with the callee declarations dropped (game.h has them) and the pointer arguments cast to `s32 *`.
Read against `target.s` by hand, every branch has its counterpart (case 1 does not null-check the second lookup, as in the asm).

Not scored: `score.sh` stops with
`placed/8001CEDC.c:93: 'func_8001CEDC' redeclared as different kind of symbol`. The file declares `extern char func_8001CEDC;`
after the function's block (it is used as `&func_8001CEDC` by func_8001D194), which conflicts with any C definition of the same
name. Landing needs that declaration changed (e.g. to `s32 func_8001CEDC(void *, void *);` or moved into the NON_MATCHING-aware part)
before this function can be tested by `tools/equiv.py`. Final count: 0 of 0 trials (no run).
