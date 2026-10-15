# func_8002CA1C

Pops an entry from a stack at D_8006CB6C (count at +0, pointer array at +0x290): decrements the count, reads the
popped entry's first word, and lowers D_8006CB68 by 0x20 (kind 0) or 0x44 (kind 1) from word & 7.

Matched on attempt 2 (0 of 31 words differ). D_8006CB6C modelled as a struct (count + pointer array at 0x290) and
D_8006CB68 read into a local before the switch; plain s32 + cast (attempt 1) did not match.
