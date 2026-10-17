# func_8001CEDC notes

Dispatches on `b->kind` (offset 0xC) through jtbl_800102D8: kinds 1 and 2 skip the type check; 3, 4 and 5 first require
the object's type word (`a->p->q->kind`) to be 0x300 or 0x400. Kind 1 and 4 test two `func_8002C324` results with
`func_80030EC0` (two `func_8002CA1C` releases), 2 and 3 test `a + 0x60` against a `func_8002C324` box with
`func_80030D98`, and 5 builds a min/max box from `b->e` and tests it with `func_80030EC0`; kind 5 also returns early,
bumping `count`, when `count >= 3 && count % 5 != 0`. Kind 0 or out of range goes straight to the common tail. The
tail increments `count`, calls `func_8001CC10`, and the function always returns -0xFF.

The earlier run mapped the jump table wrongly. The first draft here also did `s32 *` arithmetic where the assembly
adds 8 bytes.

Final: every trial agrees on seeds 0 and 1 (500) and on landing's check (seeds 1 and 2, 2000 trials each).
