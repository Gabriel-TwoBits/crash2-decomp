# func_8001FDDC

Not matched. Best: `best.c` (attempt7), 7 of 85 words differ.

## What it does

`obj` holds a pointer table at 0x10; entry 0 is a header with a base index (0x184), a count of grouped entries (0x188)
and a count of loose entries (0x18C). Each group of three entries starting at the base index gets `unk4 = obj`, and the
second and third get `unk8 =` the first. The loose entries after the groups get `unk4 = obj`. Then, if `D_800608CC` is
set and `func_8001FF50(D_800608CC, obj)` returns non-zero, `func_8001FFCC(obj)` is called. Returns -0xFF.

## What still differs

- Prologue: retail loads the 0x188 count into `$v0` and the `/ 3` constant into `$v1`; attempt7 swaps them (`$v1`/`$v0`),
  attempt8 (count through another local) gives `$a0`/`$v0`.
- Outer loop: retail copies `$a1` to `$a3` (the inner loop's `i * 3`) before `sw $s0, 4($a2)`; attempt7 after.

## Tried

- attempt1: plain `for (i < n)` with `i * 3` indices: right instructions and registers, but the `/ 3` `lui` and the
  `j = 1` are scheduled into load delay slots, which shifts every word: 77.
- attempt2 (`j = 1` first), attempt4 (base index first), attempt5 (inner `do`/`while`): 77-78.
- attempt3: `for (i = 0; i < n * 3; i += 3)`: frame grows to 0x28.
- decomp-permuter, 5 minutes on attempt1: an empty `do { } while (0);` after the load of `first` gives 23 (attempt6).
- attempt7: the count loaded in its own statement, an empty `do { } while (0);`, then `n /= 3`; and `j = 1` before the
  empty block in the outer loop: 7.

The empty blocks are scheduling barriers found by the permuter, probably standing for something else in the original
(a macro, an inline helper). Next: permuter on attempt7, or other ways to split those basic blocks.

## Matched (by hand, after the loop)

0 of 85 words differ. decomp-permuter (30 minutes on attempt7) moved the inner empty block before `j = 1`: 5 left, the
count and the `/ 3` constant in swapped registers. Loading the count into its own variable and dividing in a separate
statement (`n = count / 3`) gives the original's registers. Both empty blocks are still needed (62 to 79 words without).
