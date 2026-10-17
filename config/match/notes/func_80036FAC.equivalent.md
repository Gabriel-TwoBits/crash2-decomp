# func_80036FAC

Evaluates a condition word (`arg0`) against entry `arg1` (0x4C bytes) of the table at D_80069944..D_80069950.

- Bits 12-13 pick which entry words (D_80069944 / D_80069948 / both with D_80069950) are masked with the low 12 bits of `arg0`; 0 means "always true". A zero mask returns bit 20 of `arg0`.
- Bits 14-15 pick the test on the 4-bit argument (bits 16-19): flag bits 0x1000..0x8000 for 9..12, or comparison against D_8005B440 (12-byte entries) indexed by the top nibble of D_80069948 / D_8006994C. Mode 0 returns the masked value itself.
- Bit 20 inverts the test result.
- Bits 12-13 = 4 in the assembly is unreachable (2-bit field) and omitted.

Final: 500 of 500 trials agree on seeds 0 and 1.
