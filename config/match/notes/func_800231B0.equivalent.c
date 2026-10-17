#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800312F0(s32, s32, s32, s32);
s32 func_8003145C(s32, s32);
extern s8 D_8005B997;
extern s32 D_8005B9C8;
extern u32 D_8005B9CC;
extern s32 D_8005B9D0;
extern s32 D_8005B9D4;
extern s32 D_8005B9D8;
extern s32 D_8005B9E8;
extern s32 D_8005B9F4;
extern s32 D_8005BA00;
extern s32 D_8005BA08;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_80060800;
extern s32 D_80060804;
extern s32 D_80060808;
extern s32 D_80060824;
extern s32 D_80060828;
extern s32 D_8006082C;
extern s32 D_80060888;
extern s32 D_800608DC;
extern s32 D_800608F0;
extern s32 D_8006091C;
extern s32 D_80060920;
extern s32 *D_8006759C;

void func_800231B0(s32 *arg0) {
    s32 v28[3];
    s32 v38[3];
    s32 sp48[2];
    s32 sp50[2];
    s16 *p;
    s16 *q;
    s16 var_a0;
    s16 var_a0_2;
    s16 var_a1;
    s16 var_v1;
    s16 lim;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 mode;
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    u32 temp_s0;
    u32 ratio;

    v38[0] = arg0[0] - D_800607F4;
    v38[1] = arg0[1] - D_800607F8;
    v38[2] = arg0[2] - D_800607FC;
    func_80031340(v28, v38, D_80060828);
    v28[0] = (v28[0] + D_80060888) & 0xFFF;
    if (D_8005B997 != 0) {
        if (D_8005B997 == 1) {
            var_s1 = D_80060824;
            var_s2 = D_80060828;
            var_s3 = D_8006082C;
        } else {
            var_s1 = v28[0];
            var_s2 = v28[1];
            var_s3 = v28[2];
        }
    } else {
        p = (s16 *)func_80031AE8(D_800608DC, 0x183, D_800608E0, 0, 0, 3, sp50, 0, 0);
        if (p == 0) {
            p = (s16 *)func_80031AE8(D_800608D8[0], 0x183, D_800608E0, 0, 0, 3, sp50, 0, 0);
        }
        if (p != 0) {
            var_s1 = func_800312CC(v28[0], D_80060824, p[0]);
            var_s2 = func_800312CC(v28[1], D_80060828, p[1]);
            var_s3 = func_800312CC(v28[2], D_8006082C, p[2]);
        } else {
            var_s1 = func_8003145C(v28[0], D_80060824);
            var_s2 = func_8003145C(v28[1], D_80060828);
            var_s3 = func_8003145C(v28[2], D_8006082C);
        }
        if (D_8005B9F4 != 0) {
            var_s1 = func_800312CC(var_s1, D_8005B9E8, D_8005B9F4 >> 2);
        }
        if (D_8005BA08 != 0) {
            var_s1 += (D_8005BA08 * 0xE6) >> 0xA;
        }
        var_s1 = (var_s1 + func_80023DC4()) & 0xFFF;
        if (D_8005B9C4 != 0) {
            temp_s0 = D_8003BEA4 - D_8005B9C8;
            if (temp_s0 >= D_8005B9CC) {
                D_8005B9C4 = 0;
            } else {
                ratio = (temp_s0 << 8) / D_8005B9CC;
                var_s1 = func_800312CC(D_8005B9D0, var_s1, ratio);
                var_s2 = func_800312CC(D_8005B9D4, var_s2, ratio);
                var_s3 = func_800312CC(D_8005B9D8, var_s3, ratio);
            }
        }
        if ((D_8005B99B == 0) && (D_80060918.unk0 != 0x1001) && (D_800608F0 != 2)) {
            var_s1 = func_800312F0(D_80060918.unk0, var_s1, 0x80, 0x2D);
            var_s2 = func_800312F0(D_8006091C, var_s2, 0x80, 0x2D);
            var_s3 = func_800312F0(D_80060920, var_s3, 0x80, 0x2D);
        }
    }
    q = (s16 *)func_80031AE8(D_800608D8[0], 0x119, D_800608E0, 0, 0, 3, sp48, 0, 0);
    if (q != 0) {
        temp_a2 = D_80060824 & 0xFFF;
        var_a0 = (var_s1 & 0xFFF) - temp_a2;
        if (var_a0 >= 0x801) {
            var_a0 -= 0x1000;
        } else if (var_a0 < -0x800) {
            var_a0 += 0x1000;
        } else {
        }
        lim = q[2];
        if (lim < var_a0) {
            var_a0 = lim;
        }
        lim = -q[3];
        if (var_a0 < lim) {
            var_a0 = lim;
        }
        var_s1 = (temp_a2 + var_a0) & 0xFFF;
        temp_a2_2 = D_80060828 & 0xFFF;
        var_a0_2 = (var_s2 & 0xFFF) - temp_a2_2;
        if (var_a0_2 >= 0x801) {
            var_a0_2 -= 0x1000;
        } else if (var_a0_2 < -0x800) {
            var_a0_2 += 0x1000;
        } else {
        }
        mode = D_8006759C[2];
        if ((mode == 2) || (mode == 5)) {
            var_a1 = 0;
            var_v1 = 0;
        } else {
            var_v1 = q[0];
            var_a1 = -q[1];
        }
        if (var_v1 < var_a0_2) {
            var_a0_2 = var_v1;
        }
        if (var_a0_2 < var_a1) {
            var_a0_2 = var_a1;
        }
        var_s2 = (temp_a2_2 + var_a0_2) & 0xFFF;
        var_s3 &= 0xFFF;
    }
    if (D_8005BA08 != 0) {
        var_s2 = (var_s2 + ((D_8005BA00 * D_8005BA08) >> 0xA)) & 0xFFF;
    }
    D_80060800 = var_s1;
    D_80060804 = var_s2;
    D_80060808 = var_s3;
}
