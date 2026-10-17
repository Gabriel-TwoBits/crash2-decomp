# func_8002DFA8 — equivalent attempt

Walks the 8-byte-entry table at D_80067FE4 (count at +0x1004, origin x/y/z at +0x1008/+0x100C/+0x1010). A type-0 entry
sets the segment delta (`<<8`) and the shift limits from the next entry (which is skipped). Any other entry of a drawable
kind (not 0/5; kinds 2/6 only for mode 0 or only for mode 1) gets a Y range check per mode, then both endpoints are made
relative to `arg2`, scaled by `(x * 4) / 8192` (32-bit wrap-around, truncating) and passed to func_8002CB68. Returns the
number of calls.

Final: `best.c` (from `earlier.c`) agrees on 371 of 500 trials (seed 0) and 378 of 500 (seed 1).

Changes from `earlier.c`: the scaling is a `SCALE` macro with an unsigned shift so the multiply wraps like retail
(+5 trials).

Still disagrees: trials where the retail code reads `$s1/$s6/$s7/$fp` or the `0x30..0x38($sp)` slots before a type-0
entry set them. Those hold the caller's register values / stale stack in retail; the C zero-initialises them, so the
paths cannot be reproduced. Initialising to zero made no difference to the count. Not confirmed that this explains every
remaining disagreement.
