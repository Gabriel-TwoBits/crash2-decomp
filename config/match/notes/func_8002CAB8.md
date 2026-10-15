# func_8002CAB8

Stores three scratchpad addresses (0x1F800100, 0x1F800180, 0x1F800380) into globals, then copies 0x20 words from
D_8005BC18 to the scratchpad at 0x1F800380 and returns -0xFF.

Matched: 0 of 23 words differ (attempt 5, `match.c`).

- m2c's `var += 4` on `s32 *` is a 16-byte step; the target steps 4 bytes, so plain `*dst++ = *src++`.
- D_8005F378/F3C4/F3E8 are defined (not extern) so they are reached through `$gp`.
- The initialisation `i = 0, dst = 0x1F800380, src = D_8005BC18` inside the `for` header, after the three global stores,
  restores the original's instruction order; the same statements before the stores scored 11 differing words.
