# func_8001EAD8

Builds two 3x3 s16 rotation matrices (`a` from `-D_80060808`, `b` from `-D_80060914`, via `func_8003D2EC`/`func_8003D360`),
multiplies them with `MulMatrix(a, b)`, copies `a` into `b` transposed, calls `func_80018194(arg0, b, arg1)` and returns -0xFF.

Not matched. Best: `best.c`, 44 of 85 words differ (permuter output; its last-checked count was not re-run with `./score.sh`
after copying the source by hand).

Tried: m2c's scalar locals (frame shrinks to 0x28, locals promoted to registers: 83 differ); arrays `a[9]`, `b[9]` with
`pad[7]` between them (frame 0x68 matches: 72 differ); permuter 4 minutes (44).

Still differs: register allocation. Retail keeps the sine in `$s0`, the constant 0x1000 in `$s1`, `&b` in `$s2`, `arg0` in
`$s3`, `arg1` in `$s4`; the draft swaps these. Unexplored: declaration order of `s`/`c`, and whether the final copy reads
through a pointer.
