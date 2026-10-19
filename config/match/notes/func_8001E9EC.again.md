# func_8001E9EC

Converts the vector at arg1 (3 s32) to 1/16 units (signed /16), calls func_80018194(in, *(s16 **)(arg0 + 8), out),
scales out by 16 and adds arg0's s32 fields at 0x60/0x64/0x68, storing the result into arg2.

Matched: match.c (= attempt14.c), 0 of 59 words differ.

The first pass and this one stalled at 5 words, all in the prologue (sched2 hoisted the arg0 copy and its save above
the other saves and the first load). What closes it: `p` and `d` must be pseudos of their own, not merged with the
parameters' pseudos. decomp-permuter (5 minutes on attempt1.c) found `arg0++; arg0--;` after the call (2 words left:
`sw ra`/`sw s1` swapped); writing to arg2 too (`arg0 = (u8 *)arg0 + 1; arg2 = (u8 *)arg2 + 1;`) fixes the save order.
The two writes are dead stores kept only for the match; a more natural form that keeps the parameters apart from
`p`/`d` was not found in the time left.

`*(s16 **)(arg0 + 8)` must be read before the first division (its branch ends the block), else 53 differ.
