# func_8001E5FC

Steps `arg0` toward `arg1` by `arg2`: returns `arg1` when `arg2 > 0` and `|arg1 - arg0| < arg2`, otherwise
`arg0 + arg2` if `arg1 > arg0`, else `arg0 - arg2`.

- base.c (m2c): 19 of 17 words differ.
- attempt1.c (nested `if`/`else` on the sign of the difference): 15 differ; GCC merges the branches into one `bgez`.
- attempt2.c (`arg2 > 0 && (diff < 0 ? -diff : diff) < arg2`): **0 of 17 words differ**. The ternary absolute value
  is what produces the duplicated `slt`/`bnez` per sign and the `j` back to the shared tail.
- attempt3.c (same ternary written `diff >= 0 ? diff : -diff`): 16 differ.

Matched: match.c.
