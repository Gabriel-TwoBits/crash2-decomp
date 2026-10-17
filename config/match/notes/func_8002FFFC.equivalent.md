# func_8002FFFC

Clips a box (6 faces of 4 vertex indices from D_8005BBB8, 12-byte vertices at arg1) against the plane y = arg0 and
writes the segments (pairs of 12-byte points) to arg2; returns the segment count. Details in EARLIER.md.

Final: `best.c` (= earlier.c), 462 of 500 trials (seed 0). Per-field vertex copies (attempt6.c) gave the same count.
The earlier session's recorded 483 could not be reproduced from the files left here.

Still disagreeing: trials where both versions end in an unmapped access at different addresses (a neighbouring field of
the same vertex, or stack garbage read through out-of-range s8 edge indices from randomised tables). No difference in
return value, calls or memory was seen. Not marked equivalent.c.
