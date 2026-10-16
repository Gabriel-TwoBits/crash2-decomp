# func_8001E9EC

Converts a vector (arg1, 3 s32) to 1/16 units (signed /16), calls func_80018194(in, arg0[2], out), scales out by 16 and
adds arg0's s32 fields at 0x60/0x64/0x68, storing the result into arg2.

Not matched: best.c (= earlier.c) differs in 5 of 59 words, all in the prologue. Target: `sw ra; sw s1; sw s0;
lw v0,0(a1); addu s0,a0` (saves in order, the first load ahead of the `p` copy). Compiled: `sw s0; addu s0,a0; sw ra;
sw s1; lw v0,0(a1)` (the scheduler hoists the `p` copy and its save).

Tried, all 5 except where noted: `p[2]` read into a local first (declaration or statement); `a[0]` read into a temp
before `p[2]` (attempt3); explicit `if (x < 0) x += 15` (attempt6); `p` copied to a local pointer (attempt5);
`*d++` stores (attempt4, 14); a struct-typed version with `p[2]` inline (attempt2, 53); `p[2]` read after `in[0]`
(attempt1, 53: the prologue matches but the load leaves the first block). `d` copied to a local (attempt7). decomp-permuter on earlier.c (base 75) ran 6 minutes; its best candidate differs in 11 words, worse than the base's 5.
