#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800200A8(s32, s32, s32, s32);
void func_800231B0(s32 *);
void func_80023E60(s32 *, s32, s32 *, s32);
s32 func_8002433C(s32 *, s32 *, s32 *, s32 *, s32 *);
s32 func_80024658(s32 *, s32 *, s32, s32, s32, s32);
extern s32 D_800608CC[];

void func_800225C0(s32 *arg0, s32 arg1) {
    s32 sp18[4];
    s32 sp28[4];
    s32 pad[4];
    s32 sp48[4];
    s32 p[4];
    s32 r;

    sp18[0] = arg0[0];
    do {
        sp18[1] = arg0[1];
        sp18[2] = arg0[2];
        sp28[0] = D_800608CC[0];
        sp28[1] = D_800608CC[2];
        sp28[2] = D_800608CC[5];
        r = func_8002433C(sp28, sp18, &p[0], &p[1], &p[2]);
        if (r > 0) {
            r = func_80024658(sp28, sp18, p[1], 0, p[2], 1);
            func_80023E60(sp28, r, sp48, arg1 != 0 ? 0x100 : -1);
            func_800200A8(sp48[0], sp48[1], sp48[2], 0);
        } else if (r < 0) {
            r = func_80024658(sp28, sp18, p[0], 1, p[2], 1) + 0;
            func_80023E60(sp28, r, sp48, arg1 != 0 ? 0x100 : -1);
            func_800200A8(sp48[0], sp48[1], sp48[2], 0);
        } else {
            func_800200A8(D_800608CC[0], D_800608CC[2], D_800608CC[5], 0);
        }
    } while (0);
    func_800231B0(sp18);
}
