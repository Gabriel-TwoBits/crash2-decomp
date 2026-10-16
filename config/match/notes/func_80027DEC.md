# func_80027DEC

Matched: `match.c`, 0 of 84 words differ (`score.sh`, GCC 2.7.2).

Searches the sub-array of the object at `arg0->0x10` (count at +0x190, entries at +0x194, flags at +0x1B4) from the last
entry down: for each entry with flag 4 set, `r = func_80014260(&entry, 1, 0)`, and the first `r` for which
`func_80030D74(r, arg1)` is non-zero is returned. When nothing is found and `arg0` is not `D_800608CC`, the same search
runs on `D_800608CC`'s object. Returns -0x17 otherwise.

Earlier C (`earlier.c`) was 5 words off: the global landed in `$a0` and the -0x17 load filled the `beq` delay slot. The
permuter closed it by taking `&D_800608CC` into a local before the first loop and comparing `arg0` with `*global`;
that puts the global in `$v0` as in the retail code.

Prototype conflict for landing: `src/game/8002FE58.c` defines `void func_80030D74(s32 *arg0)`, but its assembly leaves
`$a1` and `$v0` untouched around the tail call to `func_80030CD8`, so its true signature takes a second argument and
returns `func_80030CD8`'s value. `match.c` declares `s32 func_80030D74(void *, s32)`; the definition there needs
updating (with `func_80030CD8`'s signature) before this prototype can move into `game.h`.
