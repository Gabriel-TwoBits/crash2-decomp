#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800200A8(s32, s32, s32, s32);
void func_800231B0(s32 *);
void func_80023E60(s32 *, s32, s32 *, s32);
s32 func_8002433C(s32 *, s32 *, s32 *, s32 *, s32 *);
s32 func_80024658(s32 *, s32 *, s32, s32, s32, s32);
extern s32 D_800608CC[];

void func_8002271C(s32 *arg0, s32 arg1) {
    s32 v18[3];
    s32 v28[8];
    s32 v48[3];
    s32 v58, v5C, v60;
    s32 r;
    s32 t;
    s32 t2;

    v18[0] = arg0[0];
    v18[1] = arg0[1];
    t2 = arg0[2];
    v28[0] = D_800608CC[0];
    v28[1] = D_800608CC[2];
    v28[2] = D_800608CC[5];
    v18[2] = t2;
    r = func_8002433C(v28, v18, &v58, &v5C, &v60);
    if (r > 0) {
        t = func_80024658(v28, v18, v5C, 0, v60, 1);
        func_80023E60(v28, t, v48, arg1 != 0 ? 0x100 : -1);
        func_800200A8(v48[0], v48[1], v48[2], 0);
    } else if (r < 0) {
        t = func_80024658(v28, v18, v58, 1, v60, 1);
        func_80023E60(v28, t, v48, arg1 != 0 ? 0x100 : -1);
        func_800200A8(v48[0], v48[1], v48[2], 0);
    } else {
        func_800200A8(D_800608CC[0], D_800608CC[2], D_800608CC[5], 0);
    }
    func_800231B0(v18);
}
