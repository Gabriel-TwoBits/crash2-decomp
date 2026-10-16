#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk800130A0 {
    /* 0x00 */ u8 *base;
    /* 0x04 */ s16 state;
    /* 0x06 */ s16 pad6;
    /* 0x08 */ s16 offset;
    /* 0x0A */ s16 flag;
    /* 0x0C */ s32 pad0C;
    /* 0x10 */ u16 *dest;
    /* 0x14 */ s32 index;
    /* 0x18 */ s32 pad18;
    /* 0x1C */ s32 count;
    /* 0x20 */ s32 pad20[3];
    /* 0x2C */ u16 *alt;
} Unk800130A0;

extern u8 *D_800675A0;
extern s32 D_80067FC8;

Unk800130A0 *func_800130A0(Unk800130A0 *arg0) {
    Unk800130A0 *p;
    s16 off;
    s32 state;
    s32 count;
    s32 i;
    s32 flag;
    Unk800130A0 *last;

    p = arg0;
    state = p->state;
    count = p->count;
    off = 0;
    if (state == 3 || state == 13) {
        p->state = 4;
        func_8001254C(p->base);
        return 0;
    }
    flag = 0;
    if ((p->index >> 1) < *(s32 *)(D_800675A0 + 0x41C)) {
        if (p) {
            flag = D_80067FC8 != 0;
        } else {
            flag = D_80067FC8 != 0;
        }
    }
    for (i = 0; i < count; i++) {
        if (state == 2) {
            p->flag = flag;
            if (flag != 0) {
                off = off + 32;
                off -= *(s32 *)(D_800675A0 + (p->index >> 1) * 4 + 0x420) & 0x3F;
                p->offset = off;
            }
            if (p->state == state) {
                p->state = 3;
            } else {
                p->state = 13;
                if (flag != 0) {
                    p->dest = (u16 *)(p->base + (((*(s32 *)(D_800675A0 + (p->index >> 1) * 4 + 0x420) & 0x3F) + p->offset) << 11));
                } else {
                    p->dest = p->alt;
                }
                *p->dest = 0x8765;
            }
        }
        last = p;
        p = (Unk800130A0 *)((u8 *)p - 0x2C);
    }
    return last;
}
