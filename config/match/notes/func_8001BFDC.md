# func_8001BFDC

Per-frame update of an object (arg0): a switch on the state word at +0x108 (jump table of 7 entries, 1 and the default share the
call to func_8001BFD4) handles timeouts against D_8006CB64 and clears the state, then, if the pointer at +0xE8 is non-null and its
first byte is not 4, copies three 8-byte records (offsets 0x8, 0xE, 0x14 from arg0+0x20) into arg0+0x26 with unaligned 4-byte copies.
Returns -0xFF.

Best: `best.c` (= attempt4.c), 46 of 176 words differ; 20 instructions with no counterpart with registers aside.

Closed: switch layout (the shared clear block sits inside case 7, case 6 jumps to it), the signed `/ 4` pattern via a `+ 3` temporary,
unaligned copies (lwl/lwr/swl/swr) with a 4-byte struct.

Still differs:
- Frame is 0x40 instead of 0x30 (16 extra bytes of locals); not caused by the unaligned-copy type or where `sp18` is declared.
- Argument setup order for func_8001CC10 (`addu $a0` comes later in the retail).
- The tail: retail loads the s16 fields at 0x12, 0xC, 0x18 with `lh` right before use; the compiled code hoists `lhu` loads.
