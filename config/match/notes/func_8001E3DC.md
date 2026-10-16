# func_8001E3DC

Steers an angle (12-bit, 0..0xFFF) from arg0 toward arg1 by a step: step = arg2 * min(D_80063450[5], 0x66) / 1024,
reduced (/8, /4, /2, 3/4) when arg3's flag 0x20000000 (+0xAC) is set and the signed 12-bit distance is under 4 steps.
If the distance is under the step, returns arg1 and sets bit 0x800 in arg3[0xA8]; otherwise moves arg0 by +-step
(shortest direction; a distance of exactly 0x800 goes negative when arg1 >= 0x800), masks to 0xFFF and sets/clears bit 8.

Matched: 0 of 90 words differ (attempt3.c = match.c). The key was writing the second abs inline as
`(dist < 0 ? -dist : dist) == 0x800` instead of via a local, and returning through arg0.
