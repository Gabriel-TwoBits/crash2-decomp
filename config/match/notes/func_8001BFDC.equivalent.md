# func_8001BFDC

State machine on `arg0+0x108` (states 1..7, anything else calls `func_8001BFD4(arg0, 1)`), timed against
`D_8006CB64 - arg0->0x10C`: states 6/7 clear the state and set bit 0x80 of `+0xAC` after 0x10 ticks (7 also on bit 1 of
`+0xA8`); 4 clears after 0x3D; 3 after 0x1C4; 5 after 0x25B or bit 0x20000000 of `+0x100`. States 2, 3, 5 then set the
sub-block at `+0x20` (three halfwords 0x3C, and a rotation `D_80060944 % 4 << 10`). If `+0xE8` is non-null and its first
byte is not 4, the sub-block selects (halfword 6 and the 6-byte entry at 0x1A) one of three entries, the last non-zero wins.
Returns -0xFF.

The `func_8001CC10` call under state 3 (`state == 4` check) is unreachable in the assembly and is left out.
Helpers must be `static inline`: otherwise they are compiled as extra calls and the call lists differ.

Result: 500 of 500 trials agree on seed 0 and seed 1 (`equivalent.c`).
