#include "common.h"
#include "sdk.h"
#include "game.h"

#define B164_FLD(p, off) (*(s32 *)((u8 *)(p) + (off)))
#define B164_PTR(p, off) (*(void **)((u8 *)(p) + (off)))
#define B164_NO_HIT ((s32)0xC4653601)
#define B164_BOX 0x1020
#define B164_OUT 0x1008
#define B164_ACTIVE 0x1000
#define B164_FIXED_BASE_FLAG 0x40000000
#define B164_FIXED_HEIGHT 0x29900
#define B164_HEIGHT_OFFSET 0x40
#define B164_HALF_EXTENT 0x9600
#define B164_SKIP_FLAG 0x40
#define B164_FLAGS_INIT 0x40001
#define B164_FLAGS_DROP_BIT18 0xFFFBFFFF
#define B164_FLAGS_BASE 0x4001
#define B164_FLAG_SET 0x200000
#define B164_FLAG_LOW 1
#define B164_CLEAR_MASK 0xFFDBBFFE
#define B164_PARENT_FLAG 0x400000
#define B164_EXTRA_RANGE 0x19000
#define B164_FLAG_PARENT_4000 0x4000
#define B164_TIME_WINDOW 4

extern s32 D_8006CB64;

void func_8002B164(void *arg0, s32 *arg1, s32 *arg2, void *arg3, s32 *arg4) {
    s32 sp18[3];
    s32 *box;
    s32 flags;
    s32 extra;
    s32 hitA;
    s32 hitB;
    s32 res;
    s32 base;
    s32 limit;
    s32 y;
    s32 v;
    s32 w;
    void *p;
    void *q;

    box = (s32 *)((u8 *)arg3 + B164_BOX);
    B164_FLD(arg3, 0x1020) = arg2[0] - B164_HALF_EXTENT;
    B164_FLD(arg3, 0x1024) = arg2[1];
    B164_FLD(arg3, 0x1028) = arg2[2] - B164_HALF_EXTENT;
    extra = 0;
    flags = B164_FLAGS_INIT;
    B164_FLD(arg3, 0x102C) = arg2[0] + B164_HALF_EXTENT;
    y = arg2[1];
    if (!(B164_FLD(arg0, 0x100) & B164_FIXED_BASE_FLAG)) {
        v = y + B164_FLD(B164_PTR(arg0, 8), B164_HEIGHT_OFFSET);
    } else {
        v = y + B164_FIXED_HEIGHT;
    }
    box[4] = v;
    box[5] = arg2[2] + B164_HALF_EXTENT;
    if (B164_FLD(arg3, B164_ACTIVE) == 0 ||
        func_80030F58(box, (s32 *)((u8 *)arg3 + B164_OUT)) == 0) {
        func_80029538((s32)arg2, (s32)arg0, (s32)arg3);
    }
    func_8002A3B0(arg0, arg1, arg2, arg3, 8, (s32)sp18);
    hitB = sp18[1];
    hitA = sp18[2];
    if (B164_FLD(arg0, 0x100) & B164_SKIP_FLAG) {
        hitB = B164_NO_HIT;
        hitA = B164_NO_HIT;
    }
    if (hitA == B164_NO_HIT) {
        B164_FLD(arg3, 0x1040) = 0;
    } else {
        B164_FLD(arg3, 0x1040) = hitA;
    }
    if (hitB == B164_NO_HIT) {
        B164_FLD(arg3, 0x1044) = 0;
    } else {
        B164_FLD(arg3, 0x1044) = hitB;
    }
    res = func_8002A554(arg0, arg1, arg2, arg3);
    if (res == B164_NO_HIT) {
        B164_FLD(arg3, 0x1048) = 0;
    } else if (hitA == B164_NO_HIT || res >= hitA) {
        hitA = res;
        B164_FLD(arg3, 0x1048) = hitA;
        p = B164_PTR(arg0, 0x58);
        flags = B164_FLAGS_INIT & B164_FLAGS_DROP_BIT18;
        if (p != 0) {
            if (B164_FLD(p, 0xAC) & B164_PARENT_FLAG) {
                extra = B164_EXTRA_RANGE;
            }
            if ((u32)(B164_FLD(arg0, 0xDC) - B164_FLD(arg0, 0x110)) >= B164_TIME_WINDOW) {
                flags = B164_FLAGS_BASE;
            }
        }
        flags |= B164_FLAG_SET;
    } else {
        /* res is below hitA: nothing changes, flags stay at their initial value */
    }
    if ((hitA == B164_NO_HIT && hitB == hitA) || B164_FLD(arg0, 0x88) > 0) {
        if (B164_FLD(arg0, 0x88) > 0) {
            B164_FLD(arg0, 0xA8) &= B164_CLEAR_MASK;
        }
        B164_FLD(arg3, 0x1038) = 0;
    } else {
        base = hitA;
        if (hitA == B164_NO_HIT) {
            base = hitB;
            flags = B164_FLAG_LOW;
        }
        B164_FLD(arg3, 0x1038) = base;
        w = base + 1;
        if (hitA == B164_NO_HIT) {
            limit = B164_FLD(arg0, 0x64);
        } else {
            limit = B164_FLD(arg0, 0x64) + D_8005F204 + extra;
        }
        *arg4 = limit;
        if (limit < w) {
            w = arg1[1];
            box[1] = w;
        } else {
            y = arg2[1];
            if (w >= y) {
                if (flags & B164_FLAG_SET) {
                    q = B164_PTR(B164_PTR(arg0, 8), 0x48);
                    if (q != 0) {
                        B164_FLD(q, 0xA8) |= B164_FLAG_PARENT_4000;
                    }
                }
            } else {
                w = y;
            }
            box[1] = w;
        }
        if (!(B164_FLD(arg0, 0x100) & B164_FIXED_BASE_FLAG)) {
            v = w + B164_FLD(B164_PTR(arg0, 8), B164_HEIGHT_OFFSET);
        } else {
            v = w + B164_FIXED_HEIGHT;
        }
        box[4] = v;
        arg2[1] = w;
        if (B164_FLD(arg3, B164_ACTIVE) == 0 ||
            func_80030F58(box, (s32 *)((u8 *)arg3 + B164_OUT)) == 0) {
            func_80029538((s32)arg2, (s32)arg0, (s32)arg3);
        }
        y = B164_FLD(arg0, 0x88);
        if (y < 0 && (flags & B164_FLAG_LOW)) {
            B164_FLD(arg0, 0x114) = y;
            B164_FLD(arg0, 0x88) = 0;
        }
        B164_FLD(arg0, 0xA8) |= flags;
        B164_FLD(arg0, 0x110) = D_8006CB64;
    }
}
