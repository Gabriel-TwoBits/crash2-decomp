# func_8001E9EC

Converts a fixed-point vector (`arg1`, 3 words) to world space: divides each component by 16, calls `func_80018194`
with the matrix pointer at `arg0+8`, scales the result by 16 and adds the position at `arg0+0x60..0x68`, storing into `arg2`.

Attempt 1: 500 of 500 trials agree on seeds 0 and 1. Nothing disagrees.
