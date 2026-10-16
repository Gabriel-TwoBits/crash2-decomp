# func_8002DFA8 — not matched

Walks the table at D_80067FE4 (8-byte entries, count at +0x1004, origin at +0x1008/0x100C/0x1010). An entry with type 0
sets the segment delta (`d`, `<<8`) and the next entry's shift limits; any other entry of a drawable kind (not 5/0; 2/6
only for mode 0 or only for mode 1) is converted to a pair of points relative to `arg2`, range-checked per mode, scaled
by `(x*4)/8192` and passed to func_8002CB68 with `arg0`. Returns how many were drawn.

Best: `best.c` (= attempt4.c): 33 of 182 instructions without a counterpart (registers aside), 153 words differ.

Found:
- The frame (0x78) and stack slots (0x18/0x24/0x30 vectors, 0x40 `arg0`) match only with one `Vec L[3]` array plus a
  local `sp40 = arg0`; three separate `Vec`/`s32[3]` locals give a larger frame.
- `(x * 4) / 8192` folds to `x / 2048` in this GCC; copying `x * 4` into separate locals first keeps the 0x1FFF bias form.

Still differs:
- The table base: retail keeps `D_80067FE4` in `$s3` (lui/addiu) and `arg2` in `$s5`; the draft uses `$fp` for the base and
  `$s3` for `arg2`, and addresses the `+0x1008` fields through a second register instead of `0x1008($s3)`.
- Scheduling of the four `sll 2` / bias sequences at the end differs from retail.
- Not tried: permuter, `u16` locals declared in other orders, accessing the fields through a `s32 *` base.
