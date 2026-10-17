#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 *D_80069358;

#define TAG_SHIFT 6
#define TAG_HEADER 0
#define TAG_COUNTED 2
#define HEADER_FLAG_MASK 0x1
#define HEIGHT_HI_MASK 0x1F
#define DEPTH_MASK 0xE0
#define DEPTH_SHIFT 3
#define BYTE_BITS 8
#define COUNT_SHIFT 8
#define INIT_VALUE (-0x100)

typedef struct Unk80027664List {
    /* 0x00 */ u8 unk_00[0xC];
    /* 0x0C */ s32 count;
    /* 0x10 */ u8 *ent[1];
} Unk80027664List;

s32 func_80027664(s32 *arg0) {
    Unk80027664List *list;
    u8 *hdr;
    s32 i;
    s32 n;
    s32 flag;
    s32 *out;

    list = func_80014260(arg0, 1, 1);
    hdr = 0;
    n = 0;
    for (i = 0; i < list->count; i++) {
        u32 tag = list->ent[i][0] >> TAG_SHIFT;

        if (tag == TAG_HEADER) {
            hdr = list->ent[i];
        } else if (tag == TAG_COUNTED) {
            n++;
        } else {
            /* other tags are ignored */
        }
    }
    if (hdr == 0 || n == 0) {
        return 0;
    }
    flag = hdr[0] & HEADER_FLAG_MASK;
    out = func_800277A8((hdr[1] << BYTE_BITS) | hdr[2],
                        ((hdr[3] & HEIGHT_HI_MASK) << BYTE_BITS) | hdr[4],
                        (hdr[3] & DEPTH_MASK) >> DEPTH_SHIFT, flag);
    if (out == 0) {
        return 0;
    }
    out[0] = INIT_VALUE;
    out[5] = 0;
    out[1] = n << COUNT_SHIFT;
    out[0x50 / 4] = (s32)list;
    out[0x4C / 4] = *arg0;
    if (flag != 0) {
        D_80069358 = (u8 *)out;
        func_80026EF4(out);
    } else {
        /* no global registration */
    }
    return (s32)out;
}
