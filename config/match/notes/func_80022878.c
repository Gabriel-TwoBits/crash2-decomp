#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800200A8(s32, s32, s32, s32);
void func_800231B0(s32 *);
s32 func_80024268(s32 *, s32, s32 *);
s32 func_80026764(s32 *, s32 *);
extern s32 D_800608CC[];
extern s32 D_800608DC;
extern s32 D_800608E0;
extern s32 D_800608E8;
extern s32 D_800608EC;
extern s32 D_800608F8;

void func_80022878(s32 *arg0) {
    s32 sp20[4];
    s32 sp30[4];
    s32 sp40[3];
    s32 sp50;
    s32 temp_v0;
    s32 var_a0;
    s32 var_a1;
    s32 var_v1;

    sp40[0] = arg0[0];
    sp40[1] = arg0[1];
    sp40[2] = arg0[2];
    if (D_800608F8 == 0) {
        var_a0 = 2;
        if (D_800608EC < (D_800608E8 * 2)) {
            var_a0 = 1;
        }
        D_800608F8 = var_a0;
    }
    var_a1 = func_800321D4(D_800608DC, 0x160, D_800608E0, 0, 0, 3, 0x100);
    var_v1 = 1;
    if (var_a1 < 0) {
        var_a1 = -var_a1;
        if (D_800608F8 == 1) {
            var_v1 = 2;
        }
        D_800608F8 = var_v1;
    }
    sp20[0] = D_800608CC[0];
    sp20[1] = D_800608CC[2];
    sp20[2] = D_800608E0;
    sp20[3] = D_800608F8;
    temp_v0 = func_80024268(sp20, var_a1, &sp50);
    if ((temp_v0 != 0) && (func_80026764(sp20, sp30) != 0)) {
        sp50 = sp30[2];
        D_800608F8 = sp30[3];
        if (func_800321D4(sp30[1], 0x29, 0, 0, 0, 2, 0) == 2) {
            func_80024268(sp30, temp_v0, &sp50);
        } else {
            func_80023084(sp30[1], sp30[2]);
        }
        var_a0 = sp30[0];
        var_v1 = sp30[1];
    } else {
        var_a0 = D_800608CC[0];
        var_v1 = D_800608CC[2];
    }
    func_800200A8(var_a0, var_v1, sp50, 0);
    func_800224A0(sp40);
    func_800231B0(sp40);
}
