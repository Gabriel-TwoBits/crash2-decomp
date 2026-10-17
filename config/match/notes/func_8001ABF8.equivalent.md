# func_8001ABF8

Queues a render node into the pool at `D_80063458` (+0x2088 list head, +0x2090 bump pointer) for the object `arg0`, when
the pool is inside its window (or `arg0` is the player `D_8005F38C`). Dispatch on the byte at `arg0->unk_E8`:
- 5 (table): runs `func_80018474` per entry, queues a 0x30 node.
- 2 (sprite): `func_80018474`, queues a 0x2C node with texture bits from `arg0+0x20`.
- 4 (anim): `func_8001BD84` with sign -1/0/1 from flags at 0xAC.
- 1 (model): queues a 0x64 node; for variants 4/5 builds a rotated axis and a normalized light direction
  (`func_80041A78` as the square root), shade 12 and 6/8/19 have their own fields; 0x40 gets bit 31 from `arg0->0x78`.
- others: nothing.

Written from m2c's draft, checked against the assembly (note `ly = ly + ry + rz` in the second normalization, and the
-0xB50/0/0xB50 default light axis). Field accesses use the `FLD` macro with byte offsets, as the structures are unknown.

Result (attempt1.c = best.c): seed 0: 500 of 500; seed 1: 499 of 500. Trial 234 of seed 1: retail ends by access to
unmapped 0x80471300, the candidate by a TLB miss on load (another address): both fault, in a different load order.
Not found which load; no behavioural path is missing as far as the assembly reading shows.
A 3000-trial run on seed 7 gave 2999 of 3000, the same kind of disagreement (retail: unmapped load at 0x8045ff60; candidate: address error on load).
