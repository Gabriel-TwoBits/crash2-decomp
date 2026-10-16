#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800200A8(s32, s32, s32, s32);
void func_800231B0(s32 *);
void func_80023E60(s32 *, s32, s32 *, s32);
s32 func_8002433C(s32 *, s32 *, s32 *, s32 *, s32 *);
s32 func_80024658(s32 *, s32 *, s32, s32, s32, s32);
extern s32 D_800608CC[];
extern s32 *D_8006759C;

void func_80022314(s32 *arg0, s32 arg1) {
    s32 a[3];
    s32 b[8];
    s32 out[4];
    s32 sp58;
    s32 sp5C;
    s32 sp60;
    s32 r;
    s32 a3;
    s32 x, y, z;

    a[0] = arg0[0];
    a[1] = arg0[1];
    a[2] = arg0[2];
    if (D_8006759C[2] == 3) {
        a[0] = (a[0] * 5) >> 3;
    }
    b[0] = D_800608CC[0];
    b[1] = D_800608CC[2];
    b[2] = D_800608CC[5];
    r = func_8002433C(b, a, &sp58, &sp5C, &sp60);
    if (r > 0) {
        r = func_80024658(b, a, sp5C, 0, sp60, 1);
        func_80023E60(b, r, out, arg1 ? 0x100 : -1);
        x = out[0];
        y = out[1];
        z = out[2];
    } else if (r < 0) {
        s32 r2 = func_80024658(b, a, sp58, 1, sp60, 1);
        s32 a3b = -1;
        if (arg1 != 0) {
            a3b = 0x100;
        }
        func_80023E60(b, r2, out, a3b);
        x = out[0];
        y = out[1];
        z = out[2];
    } else {
        x = D_800608CC[0];
        y = D_800608CC[2];
        z = D_800608CC[5];
    }
    func_800200A8(x, y, z, 0);
    func_800224A0(a);
    func_800231B0(a);
}
