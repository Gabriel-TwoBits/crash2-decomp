# func_80025FDC

Appends an entry to a table at `D_8005B9E0 + 0x78` (8 entries of 4 words): word 0 is `arg0` (a command id), followed by
N words copied from `arg1`, N by id (0x500/0x600: 3, 0x700: 2, 0x800: 0, other: 1). `D_8005B9E0` is the entry count and
is incremented; nothing is appended once it reaches 8.

Result: `0 of 48 words differ` (`match.c`, from `a8.c`).

What mattered:
- m2c's if-chain does not match: GCC emits `xori/sltu` for the 0/1 arm. The original is a `switch`.
- The extra `case 0x900:` (falling into `default`) is needed to shape the compare tree (root at 0x700).
- Order of arms in the source sets the layout of the final `ori` arms: 0x500/0x600, 0x800, default, 0x700 last.
- Count and table are one object (struct with `count`, 0x74 padding, `e[8][4]`): the compiler then reuses one address register.

Landing note: the file declares `extern s32 D_8005B9E0;` (twice); the match uses `extern Unk80025FDC D_8005B9E0;`, so the
declarations must be reconciled (the other users only read/clear `count`). Not tried: matching with the `s32` extern.
