# func_80019BBC

Recursively frees an object: calls func_8001CC10 (when `arg1` is set), recurses over the child list (`+0x4C` head, `+0x48`
next), calls func_80032808, clears bit 0 of the `D_8006B1C4` slot indexed by `obj[0xB8] >> 8`, zeroes `obj[0]`, and, when
the object is the current one (`D_8005F38C`), relinks the `D_8006CB58` structure, clears `D_8006CB10`'s `+0x44/+0x48/+0x4C`
and `D_8005F38C`; otherwise calls func_800189CC. Returns -0xFF on every path (`s32`; game.h declares it `void`, to fix).

Matched: 0 of 93 words differ.

What closed the gap (from 55 differing words):
- `D_8005F38C` defined in the file (`$gp` access).
- `cur = D_8005F38C` read before the table store, `arg0[0] = 0` before the compare: puts the load and the delay-slot store as retail.
- `D_8006CB10` declared `extern s32 *D_8006CB10[]` and stored through `D_8006CB10[0][n]`: reproduces the reload before each store.
- Wrapping the relink-and-clear block in `do { } while (0)` (found by the permuter) fixes the store order before `D_8005F38C = 0`.
