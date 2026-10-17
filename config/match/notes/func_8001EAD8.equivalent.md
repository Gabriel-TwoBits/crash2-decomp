# func_8001EAD8

Builds two 3x3 s16 rotation matrices from `-D_80060808` and `-D_80060914` (`func_8003D2EC` sine, `func_8003D360` cosine),
multiplies them with `MulMatrix(a, b)` (result in `a`), writes the transpose of `a` into `b`, and calls
`func_80018194(arg0, b, arg1)`.

Retail leaves -0xFF in `$v0` after the call; the declared signature is `void`, kept as is (no known caller uses it).
Result: attempt1.c (equivalent.c) agrees on 500 of 500 trials on seeds 0 and 1.
