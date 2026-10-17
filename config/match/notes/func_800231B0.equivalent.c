#include "common.h"
#include "sdk.h"
#include "game.h"

#define ANGLE_MASK 0xFFF
#define ANGLE_HALF_TURN 0x800
#define ANGLE_FULL_TURN 0x1000
#define MODE_FOLLOW_CURRENT 1
#define SHAKE_YAW_SCALE 0xE6
#define SHAKE_SHIFT 10
#define FADE_RATIO_SHIFT 8
#define EASE_ARG_C 0x80
#define EASE_ARG_D 0x2D
#define EASE_TARGET_NONE 0x1001
#define CAM_TYPE_BLEND 0x183
#define CAM_TYPE_CLAMP 0x119
#define CAM_ARG_COUNT 3
#define NO_CLAMP_MODE_A 2
#define NO_CLAMP_MODE_B 5
#define EASE_DISABLED_MODE 2
#define BLEND_STRENGTH_SHIFT 2

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
    s32 var_a0;
    s32 var_a0_2;
    s32 var_a1;
    s32 var_v1;
    s32 lim;
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
    v28[0] = (v28[0] + D_80060888) & ANGLE_MASK;
    if (D_8005B997 != 0) {
        if (D_8005B997 == MODE_FOLLOW_CURRENT) {
            var_s1 = D_80060824;
            var_s2 = D_80060828;
            var_s3 = D_8006082C;
        } else {
            var_s1 = v28[0];
            var_s2 = v28[1];
            var_s3 = v28[2];
        }
    } else {
        p = (s16 *)func_80031AE8(D_800608DC, CAM_TYPE_BLEND, D_800608E0, 0, 0, CAM_ARG_COUNT, sp50, 0, 0);
        if (p == 0) {
            p = (s16 *)func_80031AE8(D_800608D8[0], CAM_TYPE_BLEND, D_800608E0, 0, 0, CAM_ARG_COUNT, sp50, 0, 0);
        } else {
            /* first lookup hit */
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
            var_s1 = func_800312CC(var_s1, D_8005B9E8, D_8005B9F4 >> BLEND_STRENGTH_SHIFT);
        } else {
            /* no blend strength */
        }
        if (D_8005BA08 != 0) {
            var_s1 += (D_8005BA08 * SHAKE_YAW_SCALE) >> SHAKE_SHIFT;
        } else {
            /* no shake */
        }
        var_s1 = (var_s1 + func_80023DC4()) & ANGLE_MASK;
        if (D_8005B9C4 != 0) {
            temp_s0 = D_8003BEA4 - D_8005B9C8;
            if (temp_s0 >= D_8005B9CC) {
                D_8005B9C4 = 0;
            } else {
                ratio = (temp_s0 << FADE_RATIO_SHIFT) / D_8005B9CC;
                var_s1 = func_800312CC(D_8005B9D0, var_s1, ratio);
                var_s2 = func_800312CC(D_8005B9D4, var_s2, ratio);
                var_s3 = func_800312CC(D_8005B9D8, var_s3, ratio);
            }
        } else {
            /* no fade */
        }
        if ((D_8005B99B == 0) && (D_80060918.unk0 != EASE_TARGET_NONE) && (D_800608F0 != EASE_DISABLED_MODE)) {
            var_s1 = func_800312F0(D_80060918.unk0, var_s1, EASE_ARG_C, EASE_ARG_D);
            var_s2 = func_800312F0(D_8006091C, var_s2, EASE_ARG_C, EASE_ARG_D);
            var_s3 = func_800312F0(D_80060920, var_s3, EASE_ARG_C, EASE_ARG_D);
        } else {
            /* ease skipped */
        }
    }
    q = (s16 *)func_80031AE8(D_800608D8[0], CAM_TYPE_CLAMP, D_800608E0, 0, 0, CAM_ARG_COUNT, sp48, 0, 0);
    if (q != 0) {
        temp_a2 = D_80060824 & ANGLE_MASK;
        var_a0 = (var_s1 & ANGLE_MASK) - temp_a2;
        if (var_a0 > ANGLE_HALF_TURN) {
            var_a0 -= ANGLE_FULL_TURN;
        } else if (var_a0 < -ANGLE_HALF_TURN) {
            var_a0 += ANGLE_FULL_TURN;
        } else {
            /* already within a half turn */
        }
        lim = q[2];
        if (lim < var_a0) {
            var_a0 = lim;
        } else {
            /* below the upper limit */
        }
        lim = -q[3];
        if (var_a0 < lim) {
            var_a0 = lim;
        } else {
            /* above the lower limit */
        }
        var_s1 = (temp_a2 + var_a0) & ANGLE_MASK;
        temp_a2_2 = D_80060828 & ANGLE_MASK;
        var_a0_2 = (var_s2 & ANGLE_MASK) - temp_a2_2;
        if (var_a0_2 > ANGLE_HALF_TURN) {
            var_a0_2 -= ANGLE_FULL_TURN;
        } else if (var_a0_2 < -ANGLE_HALF_TURN) {
            var_a0_2 += ANGLE_FULL_TURN;
        } else {
            /* already within a half turn */
        }
        mode = D_8006759C[2];
        if ((mode == NO_CLAMP_MODE_A) || (mode == NO_CLAMP_MODE_B)) {
            var_a1 = 0;
            var_v1 = 0;
        } else {
            var_v1 = q[0];
            var_a1 = -q[1];
        }
        if (var_v1 < var_a0_2) {
            var_a0_2 = var_v1;
        } else {
            /* below the upper limit */
        }
        if (var_a0_2 < var_a1) {
            var_a0_2 = var_a1;
        } else {
            /* above the lower limit */
        }
        var_s2 = (temp_a2_2 + var_a0_2) & ANGLE_MASK;
        var_s3 &= ANGLE_MASK;
    } else {
        /* no clamp entry */
    }
    if (D_8005BA08 != 0) {
        var_s2 = (var_s2 + ((D_8005BA00 * D_8005BA08) >> SHAKE_SHIFT)) & ANGLE_MASK;
    } else {
        /* no shake */
    }
    D_80060800 = var_s1;
    D_80060804 = var_s2;
    D_80060808 = var_s3;
}
