#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk800237F8 {
    s32 cur;
    s32 want;
    s32 count;
    s32 rest[3];
} Unk800237F8;

extern void func_800236DC(void *, void *, s32);
extern s32 D_8005B9BC[];
extern s32 D_8005B9C0;
extern Unk800237F8 D_8005BA28[];
extern char D_8005BB48[];
extern s32 D_80060914;
extern s32 D_80069948;
extern char *D_8005F38C;

void func_800237F8(void) {
    s32 notFlag;
    s32 bits;
    s32 d;
    s32 i;
    Unk800237F8 *e;
    char *q;

    notFlag = (u32)*(s32 *)(D_8005F38C + 0xAC) >> 7;
    notFlag = notFlag ^ 1;
    notFlag = notFlag & 1;
    bits = 0;
    if (notFlag == 0) {
        d = (s32)((*(s32 *)(D_8005F38C + 0x70) - D_80060914) << 20) >> 20;
        if ((u32)(d - 0x71) < 0x71E) {
            bits = 2;
        } else if ((u32)(d + 0x78E) < 0x71E) {
            bits = 8;
        }
        if ((u32)(d + 0x38D) < 0x71B) {
            bits |= 4;
        } else if ((u32)(d + 0x471) >= 0x8E3) {
            bits |= 1;
        }
        if (D_8005B9BC[0] != bits) {
            D_8005B9BC[0] = bits;
            D_8005B9C0 = 0;
            bits = 0;
        } else {
            D_8005B9C0 = D_8005B9C0 + 1;
            if (D_8005B9C0 >= 16) {
                D_8005B9C0 = 15;
            } else {
                bits = 0;
            }
        }
        if (*(s32 *)(D_8005F38C + 0x100) & 0x04000000) {
            if (D_80069948 & 0x40) {
                bits |= 1;
            } else if (D_80069948 & 0x20) {
                bits |= 4;
            }
        } else {
            if (D_80069948 & 0x1000) {
                bits |= 1;
            } else if (D_80069948 & 0x4000) {
                bits |= 4;
            }
            if (D_80069948 & 0x2000) {
                bits |= 2;
            } else if (D_80069948 & 0x8000) {
                bits |= 8;
            }
        }
    }
    e = D_8005BA28;
    q = D_8005BB48;
    for (i = 0; i < 2; i++) {
        if (notFlag == 0) {
            if (i == 1) {
                if (bits & 1) {
                    if (e->want == i) {
                        e->want = 0;
                        e->count = 0;
                    }
                } else if (bits & 4) {
                    if (e->want == 0) {
                        e->want = i;
                        e->count = 0;
                    }
                }
            } else if (bits & 2) {
                if (e->want == 1) {
                    e->want = 0;
                    e->count = 0;
                }
            } else if (bits & 8) {
                if (e->want == 0) {
                    e->want = 1;
                    e->count = 0;
                }
            }
        }
        if (e->want != e->cur) {
            e->count = e->count + 1;
            if (e->count > 0) {
                e->cur = e->want;
                e->rest[1] = 0;
            }
        }
        func_800236DC(&e->rest[0], q, e->cur == 0 ? -1 : 1);
        e++;
        q += 0x10;
    }
}
