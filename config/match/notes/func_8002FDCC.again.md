# Input_LoadDemo notes

Clears D_800675AC, opens an NSF entry for arg0, stores the magic 0x6396347F in *arg0, copies word 4 of the entry to g_DemoBlock, spawns an object with a {0, 0x6400} stack buffer, starts playback.

best.c (earlier.c shape): 4 of 35 words differ; the lui/addiu for D_8006CB20 is scheduled before the lw 0x10($v0).
A 4-minute permuter run found a 0-of-35 candidate, written under build/permuter/Input_LoadDemo/output-0-1/source.c (outside this directory, so not readable in this session). It has to be copied to match.c and checked with score.sh.
Tried: D_8006CB20 as s32[] (5), as char[] with reordered stores (10).
