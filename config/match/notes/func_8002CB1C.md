# func_8002CB1C

Calls `func_8003CBE4()` only when both `|arg0|` and `|arg1|` are below 0x20.

- m2c draft and a plain `ABS` macro version: 8 and 4 words differ (the first `abs` negated in `$a0` instead of a copy in `$v0`).
- decomp-permuter (under 1 minute) found a match: a redundant `if (arg0 || arg1)` whose two branches both call `func_8003CBE4()`. It changes the first `abs` to use `$v0`.
- Final count: 0 of 19 words differ (`./score.sh match.c`). The redundant branch is an artefact; the original source shape is unknown.
