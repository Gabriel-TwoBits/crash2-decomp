# func_8001ABF8

Builds one display-list primitive for an object, from `arg0->0xE8` (kind byte 1, 2, 4 or 5; others return):
- 1: 0x64-byte primitive with model/animation data, vectors (sub-kinds 4/5, rotated for 5), light direction (normalised via `func_80041A78`), per-mode table entries and slope/offset terms.
- 2: 0x2C-byte primitive after `func_80018474`, table index from `arg0->0x120/0x121`.
- 4: forwards to `func_8001BD84` with direction -1/0/1.
- 5: loops `func_80018474` over the entries, then a 0x30-byte primitive.
Skipped when the packet list is nearly full unless `arg0 == D_8005F38C`.

Lessons: the mode read from `D_80062ACC` is a full word (not truncated to u8); `arg0` can alias the packet list in random trials, so load/store order follows the assembly (e.g. `arg0->0xA0` is read before the `prim->0x10` store).

Result: 500 of 500 on seeds 0 and 1, 3000 of 3000 on seed 2. Nothing disagrees.
