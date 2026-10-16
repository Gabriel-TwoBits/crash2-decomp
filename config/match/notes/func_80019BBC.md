# func_80019BBC

Recursively frees an object: calls func_8001CC10 (when `arg1` is set), recurses over the child list (`+0x4C` head,
`+0x48` next), calls func_80032808, clears bit 0 of the `D_8006B1C4` slot indexed by `obj[0xB8] >> 8`, zeroes `obj[0]`,
and, when the object is the current one (`D_8005F38C`), relinks the `D_8006CB58` structure, clears `D_8006CB10`'s
`+0x44/+0x48/+0x4C` and `D_8005F38C`. Otherwise calls func_800189CC. Returns -0xFF on every path (game.h declares it
`void`; the retail code sets `$v0` regardless, so the prototype may need `s32`).

Not matched: best is `best.c`, 31 of 93 words differ.

Tried:
- `D_8005F38C` defined in the file (`s32 D_8005F38C;`) gives the `$gp` access (baseline 84 -> 56).
- `arg1` reused as the saved value (lands in `$s2`), `D_8006CB58` as an array with a pointer kept in `$s0` (47 -> 31).

Still differs:
- Retail reloads `D_8006CB10` before each of the three stores; compiled code loads it once. A struct type and a `volatile`
  pointer did not reproduce it (volatile hoists all three loads).
- Retail has `sw $zero, 0($s1)` in the delay slot of the `bne` (store before the compare, with `D_8005F38C` loaded earlier
  than the store); here the table store takes the slot and `arg0[0] = 0` is duplicated in both branches.
