#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800200A8(s32, s32, s32, s32);
void func_800231B0(s32 *);
s32 func_80024268(s32 *, s32, s32 *);
s32 func_800312CC(s32, s32, s32);
extern s32 D_8005B9AC;
extern s32 D_8005BB3C;
extern s32 D_8005BB44;
extern s32 D_80060818;
extern s32 D_80060820;
extern s32 D_800608CC[];
extern s32 D_800608D4;
extern s32 D_800608E0;
extern s32 D_800608F8;
extern s32 D_80060914;

#define ANGLE_UNSET 0x1001
#define ANGLE_MASK 0xFFF
#define ANGLE_HALF 0x800
#define ANGLE_FULL 0x1000
#define ANGLE_LOW_LIMIT 0x200
#define ANGLE_HIGH_LIMIT 0xE00
#define ANGLE_SMOOTH_STEP 0x80
#define ANGLE_DEAD_ZONE 2
#define ANGLE_DEAD_SPAN 5
#define DIFF_SCALE 221
#define DIFF_SHIFT_PRE 16
#define DIFF_SHIFT_A 8
#define DIFF_SHIFT_B 4

void func_80021F4C(s32 *arg0, s32 arg1) {
    s32 a[4];
    s32 b[4];
    s32 c[3];
    s32 d;
    s32 s0;
    s32 v;
    s32 t;
    s32 a0, a1, a2;

    if (arg1 != 0) {
        D_8005B9AC = ANGLE_UNSET;
    }
    c[0] = arg0[0];
    c[1] = arg0[1];
    c[2] = arg0[2];
    s0 = (ratan2(D_8005BB3C - c[0], D_8005BB44 - c[2]) + ANGLE_HALF) & ANGLE_MASK;
    if (s0 > ANGLE_HALF) {
        if (s0 < ANGLE_HIGH_LIMIT) {
            s0 = ANGLE_HIGH_LIMIT;
        }
    } else if (s0 > ANGLE_LOW_LIMIT) {
        s0 = ANGLE_LOW_LIMIT;
    }
    D_80060914 = s0;
    if (D_8005B9AC != ANGLE_UNSET) {
        s0 = func_800312CC(D_8005B9AC, s0, ANGLE_SMOOTH_STEP);
    }
    D_8005B9AC = s0;
    v = s0 - (ratan2(D_80060818 - D_8005BB3C, D_80060820 - D_8005BB44) & ANGLE_MASK);
    if (v > ANGLE_HALF) {
        v -= ANGLE_FULL;
    }
    if (v < -ANGLE_HALF) {
        v += ANGLE_FULL;
    }
    if ((u32)(v + ANGLE_DEAD_ZONE) >= ANGLE_DEAD_SPAN) {
        if (v < 0) {
            D_800608F8 = 1;
        } else {
            D_800608F8 = 2;
        }
    } else {
        D_800608F8 = 0;
    }
    if (D_800608F8 != 0) {
        if (v < 0) {
            v = -v;
        }
        t = (s32)(v * DIFF_SCALE * DIFF_SHIFT_PRE) >> DIFF_SHIFT_A;
        a[0] = D_800608CC[0];
        a[1] = D_800608D4;
        a[2] = D_800608E0;
        a[3] = D_800608F8;
        t = func_80024268(a, t >> DIFF_SHIFT_B, &d);
        if (t != 0 && func_80026764((s32)a, (void *)b) != 0) {
            D_800608F8 = b[3];
            func_80024268(b, t, &d);
            a0 = b[0];
            a1 = b[1];
            a2 = d;
        } else {
            a0 = D_800608CC[0];
            a1 = D_800608D4;
            a2 = d;
        }
    } else {
        a0 = D_800608CC[0];
        a1 = D_800608D4;
        a2 = D_800608E0;
    }
    func_800200A8(a0, a1, a2, 0);
    func_800231B0(c);
}
