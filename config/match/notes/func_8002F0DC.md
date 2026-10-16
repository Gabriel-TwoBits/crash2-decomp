# func_8002F0DC

Reset of the 8002E884.c module state: when `arg0` is nonzero it zeroes about 30 globals, sets `D_8006CC84`/`D_8006CDB4` to -1,
`D_8006CBC0`/`D_8006CCD4` to 0x63, copies `D_8006CBEC` into `D_8006CBD0`, clears two 0x200-byte buffers
(`D_80069718`, `D_8006935C`) with `memset` and zeroes `D_8006CBCC`.

Not matched: `best.c` (earlier.c, m2c's draft with the externs fixed) scores 4 of 83 words differ under the current flags.

The only difference is the scheduling of the first four instructions after `beqz`: retail loads `D_8006CBEC` into `$a3`
(`lui`/`lw`) before building `$a0 = D_80069718` (`lui`/`addiu`); the compiled code does the reverse.

Tried, all 4 differ:
- statement order: load `D_8006CBEC` first into a temporary, or assign `D_8006CBD0 = D_8006CBEC` directly (attempt2-3);
- a local pointer for `memset`'s first argument, set before or after the load (attempt3);
- early `return` instead of the enclosing `if` (attempt5), a `do { } while (0)` body (attempt6);
- `arg0` reused as the temporary (attempt7), an empty `if (t)` after the load (attempt8), `s32 t = arg0; if (t != 0)`
  (attempt9).
- decomp-permuter, under a minute: again the zero-score result that reads an uninitialised `t` in a duplicated
  `if (t) {...} else {...}` (the same one the earlier run found and discarded). Not valid C; discarded. It hints that a
  basic-block or liveness change at the top of the body flips the scheduler's tie, but no valid form reproducing it was found.
