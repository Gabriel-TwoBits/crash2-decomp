# func_8002DFA8

Walks the 8-byte-entry table at D_80067FE4 (count at +0x1004, origin x/y/z at +0x1008/+0x100C/+0x1010). A type-0 entry
sets the segment delta (`<<8`) and the shift limits from the next entry (which is skipped). Any other entry of a drawable
kind (not 0/5; kinds 2/6 only for mode 0 or only for mode 1) gets a Y range check per mode, then both endpoints are made
relative to `arg2`, scaled by `(x * 4) / 8192` (32-bit wrap-around) and passed to func_8002CB68. Returns the call count.

Final: `equivalent.c` agrees on 500 of 500 trials on seeds 0 and 1. The earlier run's disagreements came from declaring
D_80067FE4 as a struct (conflicting with the file's `u8 D_80067FE4[]`); byte offsets fix it.

Caveat: retail reads `$s1/$s6/$s7/$fp` and the delta stack slots before any type-0 entry sets them (caller's values);
the C zero-initialises them. `arg3` is unused.
