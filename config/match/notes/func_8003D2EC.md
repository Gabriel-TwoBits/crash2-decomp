# func_8003D2EC — not matched (8 of 29 words differ)

## What it does

Sine lookup over a 4096-unit turn: `arg0 & 0xFFF` picks the quadrant, and `D_8005BCB0` is a quarter-wave table of `s16`
(0x401 entries, values up to 0x1000). Quadrants: `t[a]`, `t[0x800 - a]`, `-t[a - 0x800]`, `-t[0x1000 - a]`. It returns
`int` (no sign extension after `negu`). It matches the shape of PsyQ libgte's `rsin`/`sin_1` and `rsin_tbl`.

## Best: best.c (attempt3.c), 8 of 29

Control flow, frame, table base in `$v1`, the byte-offset addressing (`0x1000`, `-0x1000`, `0x2000`) and the `negu` all
line up. What still differs:

- Every unconditional branch is `bgez $zero` (`b`) in the target and `j` in the build, with the `lh` in the branch's
  delay slot followed by a `nop`; the build puts the `lh` before the `j` and a `nop` in the slot.
- The last quadrant ends with a `b` to the very next instruction (the epilogue), which GCC at -O2 removes.
- `addu $v0, $v1, $v0` (table + index) in the target, `addu $v0, $v0, $v1` in the build; swapping the C operands
  (attempt4.c) does not change it.

## Conclusion

The `bgez $zero` encoding and the kept jump to the next instruction appear in 47 functions between 0x80039FC4 and
0x800461E0 (412 occurrences), which points at a separately built library (libgte-like) with a different assembler or
compiler, not at the C. `score.sh` only tries GCC 2.7.2 with ASPSX 2.34; the other ASPSX versions in
`tools/probe_compiler.py` were not run in this session.

Attempts: attempt1.c (array indexing, `s16` return, 39 differ), attempt2.c (local pointer, `s32` return, early returns,
25), attempt3.c (byte arithmetic, single result variable, 8), attempt4.c (operands swapped, 8).
