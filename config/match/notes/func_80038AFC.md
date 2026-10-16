# func_80038AFC

Opcode-style handler: decodes `arg1` (count = bits 0-5, nibble = bits 20-23, byte = bits 12-19, 6-bit field = bits 6-11,
flag = top byte == 0x46). When count is 0 it reads it from the stack slot below the pointer at +0xBC. Calls
`func_80019800` count times, stores the result (or 0) in +0xD4 (and the owner in result+0x50 when the result is < -0xFE),
then pops `n` words off the pointer at +0xBC.

Final count: 37 of 70 words differ (best.c = attempt3.c; attempt1 `for` form 40, do-while form 37).
Remaining difference: the compiler hoists `arg1 >> 12` out of the loop into `$fp` (target recomputes it in the loop),
which grows the frame to 0x40/0x48 and shifts every save slot. The rest (control flow, `negu`/`sll` for the stack
offset, `$s4`-`$s7` allocation) already lines up. Tried: `(u8)` cast, `(x & 0xFF000) >> 12`, `for` with k inside the
loop (worse: 54, 63).
