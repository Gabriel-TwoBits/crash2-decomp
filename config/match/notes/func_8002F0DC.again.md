# Sys_ResetGameInfo (func_8002F0DC)

When `arg0` is nonzero: zeroes ~30 globals of 8002E884.c, sets `D_8006CC84`/`D_8006CDB4` to -1 and
`D_8006CBC0`/`D_8006CCD4` to 0x63, copies `D_8006CBEC` into `D_8006CBD0`, clears `D_80069718` and `g_PermFlags`
(0x200 bytes each) with `memset`, then zeroes `D_8006CBCC`.

Not matched. `best.c` (= earlier.c) scores 4 of 83 words differ, 2 instructions without a counterpart. The only
difference is the order of two independent setups right after `beqz`: retail loads `D_8006CBEC` into `$a3` first,
then builds `$a0 = &D_80069718`; every compiled variant does the reverse.

Tried this run (all 4 differ unless noted):
- `D_8006CBEC` as `s32[]`, `void *` (with `D_8006CBD0` as `void *`), `D_80069718` as `u8[]` (attempt_b, c, g);
- the copy `D_8006CBD0 = D_8006CBEC` as the first statement (the store stays last, the load still second) (h);
- an empty `if (t) {}` after the load, a redundant `if (arg0 == 0) return;` inside the body, an early return (i, j, k);
- the memset pointer in an inner-block local (m), memset wrapped in a `static inline` helper (p);
- all the globals read and written as one `s32[]` based at `D_8006CB84` (n): same order, so not alias analysis;
- the load before the `if` (o): 5 differ, the load lands before `beqz`;
- the body duplicated in `if (t) {...} else {...}` to mimic the permuter's hint (l): 94 differ, not merged;
- `D_8006CCD4 = D_8006CBC0 = 0x63` (q), locals declared in the other order (r).
- decomp-permuter, 5 minutes: again only the invalid candidate that tests an uninitialised local around two copies
  of the body. Discarded.

The tie looks like the scheduler's ordering of two equal-priority insns; nothing in valid C tried so far moves it.
