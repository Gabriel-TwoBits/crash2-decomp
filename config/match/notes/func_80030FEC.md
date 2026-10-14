# func_80030FEC

Octagonal-style distance approximation between two s32[3] vectors: max(|dx|,|dy|,|dz|) + (sum of the other two) / 4.

- attempt1 (plain `s32 d[3]` throughout): 68 of 64 words differ, because every read came from the stack.
- attempt2: scalars `x`, `y`, `z` for the absolute differences, also stored into a local `d[3]`; the first max test uses the scalars, the later tests read `d[]`. Result: 0 of 64 words differ.

Matched: `match.c`.
