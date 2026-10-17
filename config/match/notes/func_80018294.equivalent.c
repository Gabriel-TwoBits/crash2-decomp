#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_STRIDE 0x30
#define FIRST_ENTRY_OFFSET 4
#define POS_SHIFT 8
#define ACTIVE_MASK 0xFF
#define MASK_QUERY_ID 0x27F
#define MASK_QUERY_MODE 2
#define SHAPE_FLAG_ZERO_OFFSET 1

extern s32 D_800607F4;
extern s32 D_800608D4;
extern s32 D_800608E8;

void func_80018294(s32 *arg0) {
    s32 mask;
    s32 i;
    s32 off;
    s32 *entry;
    s32 *src;
    s32 *shape;
    s32 dx;
    s32 dy;
    s32 dz;

    mask = func_800321D4(D_800608D4, MASK_QUERY_ID, -1, D_800608E8, 0, MASK_QUERY_MODE, 0);
    entry = arg0;
    off = FIRST_ENTRY_OFFSET;
    for (i = 0; i < *arg0; i++) {
        if (((mask & ACTIVE_MASK) >> i) & 1) {
            src = (s32 *)func_80014260((u8 *)arg0 + off, 1, 0);
            shape = (s32 *)src[4];
            if (shape[3] == SHAPE_FLAG_ZERO_OFFSET) {
                entry[2] = 0;
                entry[3] = 0;
                entry[4] = 0;
            } else {
                dx = shape[0] - (D_800607F4 >> POS_SHIFT);
                dy = shape[1] - ((u32)D_800607F8 >> POS_SHIFT);
                dz = shape[2] - (D_800607FC >> POS_SHIFT);
                entry[2] = dx;
                entry[3] = dy;
                entry[4] = dz;
            }
            entry[5] = (s32)shape;
            entry[8] = src[5];
            entry[6] = src[6];
            entry[7] = src[7];
            entry[11] = src[8];
            entry[9] = src[9];
            entry[12] = (s32)(shape + 11);
            entry[10] = src[10];
        }
        entry += ENTRY_STRIDE / 4;
        off += ENTRY_STRIDE;
    }
}
