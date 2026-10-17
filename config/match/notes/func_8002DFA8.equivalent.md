# func_8002DFA8

Walks the 8-byte-entry table at D_80067FE4 (count at +0x1004, origin at +0x1008/+0x100C/+0x1010). A type-0 entry sets
the segment delta (`<<8`) and shift limits from the next entry (skipped). Other drawable kinds (not 0/5; kinds 2/6 only
for mode 0 or only for mode 1) get a per-mode Y range check, then both endpoints, relative to `arg2` and scaled by
`(x*4)/8192`, go to func_8002CB68. Returns the call count. `arg3` is unused.

The earlier landing refusal came from zero-initialising s1/fp/s7/s6 and the delta slots, which retail reads before any
type-0 entry sets them (caller's values). Left uninitialised: 500/500 on seeds 0 and 1, 2000/2000 on seeds 1 and 2.
Nothing disagrees. Agreement depends on the compiler keeping those locals in the caller's registers and stack slots.
