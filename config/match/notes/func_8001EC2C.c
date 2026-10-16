#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 RotTrans(s16 *, s32 *, s32 *, s32);
extern void SetRotMatrix(void *);
extern void SetTransMatrix(void *);
extern char D_80060130;
extern char D_80060774;
extern char D_800607D4;
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_8006083C;
extern s32 D_80060840;
extern s32 D_80060844;

s32 func_8001EC2C(s32 *arg0, s32 *arg1, s32 arg2) {
    s16 sv[3];
    s32 wv[3];
    s32 out[2];
    void *m;
    s32 a2;
    s32 a3;
    s32 x;

    wv[0] = arg0[0];
    wv[1] = arg0[1];
    m = &D_80060774;
    wv[2] = arg0[2];
    if (arg2 != 0) {
        m = &D_800607D4;
    }
    SetRotMatrix(m);
    SetTransMatrix(&D_80060130);
    if (arg2 != 0) {
        a2 = D_80060840;
        a3 = D_80060844;
        x = wv[0] - D_8006083C;
    } else {
        a2 = D_800607F8;
        a3 = D_800607FC;
        x = wv[0] - D_800607F4;
    }
    sv[0] = x >> 8;
    sv[1] = (wv[1] - a2) >> 8;
    sv[2] = (wv[2] - a3) >> 8;
    RotTrans(sv, arg1, out, a3);
    a2 = arg1[0];
    a3 = arg1[1];
    x = arg1[2];
    arg1[0] = a2 << 8;
    arg1[1] = -(a3 << 8);
    arg1[2] = x << 8;
    return -0xFF;
}
