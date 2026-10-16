#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800200A8(s32, s32, s32, s32);
void func_800231B0(s32 *);
s32 func_80024268(s32 *, s32, s32 *);
s32 func_80026764(s32 *, s32 *);
s32 func_800312CC(s32, s32, s32);
extern s32 D_8005B9AC;
extern s32 D_8005BB3C;
extern s32 D_8005BB44;
extern s32 D_80060818;
extern s32 D_80060820;
extern s32 D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608E0;
extern volatile s32 D_800608F8;
extern s32 D_80060914;

void func_80021F4C(s32 *arg0, s32 arg1) {
    s32 a[4];
    s32 b[4];
    s32 c[8];
    s32 d;
    s32 s0;
    s32 v;
    s32 t;
    s32 a0, a1, a2;
    s32 *p;

    if (arg1 != 0) {
        D_8005B9AC = 0x1001;
    }
    c[0] = arg0[0];
    c[1] = arg0[1];
    c[2] = arg0[2];
    s0 = (ratan2(D_8005BB3C - c[0], D_8005BB44 - c[2]) + 0x800) & 0xFFF;
    if (s0 >= 0x801) {
        if (s0 < 0xE00) {
            s0 = 0xE00;
        }
    } else if (s0 >= 0x201) {
        s0 = 0x200;
    }
    p = &D_8005B9AC;
    D_80060914 = s0;
    if (*p != 0x1001) {
        s0 = func_800312CC(*p, s0, 0x80);
    }
    *p = s0;
    v = s0 - (ratan2(D_80060818 - D_8005BB3C, D_80060820 - D_8005BB44) & 0xFFF);
    if (v >= 0x801) {
        v -= 0x1000;
    }
    if (v < -0x800) {
        v += 0x1000;
    }
    if ((u32)(v + 2) >= 5U) {
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
        t = (s32)(v * 221 * 16) >> 8;
        a[0] = D_800608CC;
        a[1] = D_800608D4;
        a[2] = D_800608E0;
        a[3] = D_800608F8;
        t = func_80024268(a, t >> 4, &d);
        if (t != 0 && func_80026764(a, b) != 0) {
            D_800608F8 = b[3];
            func_80024268(b, t, &d);
            a0 = b[0];
            a1 = b[1];
            a2 = d;
        } else {
            a0 = D_800608CC;
            a1 = D_800608D4;
            a2 = d;
        }
    } else {
        a0 = D_800608CC;
        a1 = D_800608D4;
        a2 = D_800608E0;
    }
    func_800200A8(a0, a1, a2, 0);
    func_800231B0(c);
}
