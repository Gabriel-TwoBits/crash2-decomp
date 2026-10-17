#include "common.h"
#include "sdk.h"
#include "game.h"

#define ANGLE_MASK 0xFFF
#define ANGLE_HALF 0x800
#define ANGLE_FULL 0x1000
#define ANGLE_UNSET 0x1001
#define NEAR_DIST_ENTER 0x1404F
#define FAR_DIST_LEAVE 0x209E0
#define LEAVE_BLEND_RESET 0x400
#define SMOOTH_BLEND 0x80
#define MAX_DIST 0xEB1B8
#define DIST_OFFSET 0x416E0
#define DIR_NONE 0
#define DIR_NEG 1
#define DIR_POS 2
#define DEAD_ZONE 2
#define DEAD_ZONE_SPAN 5U
#define TURN_SCALE 3536
#define BOB_SCALE 0x25800

typedef struct { s32 a, b, c, d; } V;
s32 func_80026764(s32 arg0, V *arg1);
void func_8002FF54(s32 *, s32, s32, s32);
extern s8 D_8005B99A;
extern s32 D_8005B99C;
extern s32 D_8005B9A0;
extern s32 D_8005B9A4;
extern s32 D_8005B9A8;
extern s32 D_8005B9AC;
extern s32 D_8005B9EC;
extern s32 D_8005B9F0;
extern s32 D_8005B9F4;
extern s32 D_8005BA04;
extern s32 D_8005BA08;
extern s32 D_8005BB00;
extern s32 D_8005BB08;
extern s32 D_8005BB0C;
extern s32 D_8005BB1C;
extern s32 D_8005BB2C;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80060800;
extern s32 D_80060804;
extern s32 D_80060818;
extern s32 D_80060820;
extern s32 D_800608CC[];
extern s32 D_800608D4;
extern s32 D_800608E0;
extern s32 D_800608F8;
extern s32 D_80060914;

void func_80021A08(s32 *arg0, s32 arg1) {
    s32 sp10[4];
    s32 sp20[4];
    s32 sp30[3];
    s32 sp40[3];
    s32 sp50[2];
    s32 dx;
    s32 dz;
    s32 ang;
    s32 diff;
    s32 dist;
    s32 d;
    s32 steps;
    s32 a0;
    s32 a1;
    s32 a2;

    if (arg1 != 0) {
        D_8005B9AC = ANGLE_UNSET;
    }
    func_800236DC(&D_8005B9F4, &D_8005BB0C, 0);
    func_800236DC(&D_8005BA08, &D_8005BB1C, 0);
    func_800236DC(&D_8005B99C, &D_8005BB2C, 0);

    sp30[0] = arg0[0];
    sp30[1] = arg0[1];
    dx = D_8005BB00 - arg0[0];
    dz = D_8005BB08 - arg0[2];
    sp30[2] = arg0[2];
    ang = ratan2(dx, dz) & ANGLE_MASK;
    dist = func_80041A78((dz >> 8) * (dz >> 8) + (dx >> 8) * (dx >> 8)) << 8;

    if (D_8005B99A != 0) {
        if (dist > FAR_DIST_LEAVE) {
            D_8005B99A = 0;
            D_8005B99C = LEAVE_BLEND_RESET;
            D_8005B9A0 = 0;
            D_8005B9A4 = -1;
        }
    } else if (dist <= NEAR_DIST_ENTER) {
        D_8005B99A = 1;
        D_8005B9A8 = ang;
    }

    if (D_8005B99A != 0) {
        ang = D_8005B9A8;
    } else if (D_8005B99C != 0) {
        ang = func_800312CC(ang, D_8005B9A8, D_8005B99C >> 2);
        D_8005B9A8 = ang;
    }
    if (D_8005BA08 != 0) {
        ang = (ang - ((D_8005BA04 * D_8005BA08) >> 10)) & ANGLE_MASK;
    }
    if (D_8005B9F4 != 0) {
        ang = func_800312CC(ang, D_8005B9EC, D_8005B9F4 >> 2);
    }
    D_80060914 = ang;
    if (D_8005B9AC != ANGLE_UNSET) {
        ang = func_800312CC(D_8005B9AC, ang, SMOOTH_BLEND);
    }
    D_8005B9AC = ang;

    diff = ang - (ratan2(D_80060818 - D_8005BB00, D_80060820 - D_8005BB08) & ANGLE_MASK);
    if (diff > ANGLE_HALF) {
        diff -= ANGLE_FULL;
    }
    if (diff < -ANGLE_HALF) {
        diff += ANGLE_FULL;
    }
    if ((u32)(diff + DEAD_ZONE) >= DEAD_ZONE_SPAN) {
        if (diff < 0) {
            D_800608F8 = DIR_NEG;
        } else {
            D_800608F8 = DIR_POS;
        }
    } else {
        D_800608F8 = DIR_NONE;
    }

    if (D_800608F8 != 0) {
        if (diff < 0) {
            diff = -diff;
        }
        sp10[0] = D_800608CC[0];
        sp10[1] = D_800608D4;
        sp10[2] = D_800608E0;
        sp10[3] = D_800608F8;
        steps = func_80024268(sp10, (diff * TURN_SCALE) >> 8, sp50);
        if (steps != 0 && func_80026764((s32)sp10, (V *)sp20) != 0) {
            D_800608F8 = sp20[3];
            func_80024268(sp20, steps, sp50);
            a0 = sp20[0];
            a1 = sp20[1];
            a2 = sp50[0];
        } else {
            a0 = D_800608CC[0];
            a1 = D_800608D4;
            a2 = sp50[0];
        }
    } else {
        a0 = D_800608CC[0];
        a1 = D_800608D4;
        a2 = D_800608E0;
    }
    func_800200A8(a0, a1, a2, 0);

    if (D_8005BA08 != 0) {
        D_800607F8 -= (D_8005BA08 * BOB_SCALE) >> 10;
    }
    func_800231B0(sp30);

    d = func_80041A78((arg0[0] >> 8) * (arg0[0] >> 8) + (arg0[2] >> 8) * (arg0[2] >> 8)) << 8;
    if (d > MAX_DIST) {
        d = MAX_DIST;
    }
    d = d - DIST_OFFSET;
    if (d < 0) {
        d = 0;
    }
    if (D_8005B9F4 != 0 && D_8005B9F0 >= 0) {
        d += ((D_8005B9F0 - d) * D_8005B9F4) >> 10;
    }
    func_8002FF54(sp40, -d, D_80060800, D_80060804);
    D_800607F4 += sp40[0];
    D_800607F8 += sp40[1];
    D_800607FC += sp40[2];
}
