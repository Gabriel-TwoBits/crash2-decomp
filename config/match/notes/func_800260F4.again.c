#include "common.h"
#include "sdk.h"
#include "game.h"

extern char D_80010400[];
extern s8 D_8005B99B;
extern s32 D_8005B9E4[];
extern s32 D_8005B9E8[];
extern s32 D_8005B9F0[];
extern s32 D_8005B9F4[];
extern s32 D_8005BA00[];
extern s32 D_8005BA04;
extern s32 D_8005BA0C;
extern s32 D_8005BA10;
extern s32 D_800608FC;

void Cam_ProcessMessage(s32 *arg0) {
    s32 *p;
    s32 *q;
    s32 i;
    s32 set;

    switch (*arg0) {
    case 0x0:
        func_80026334(*(arg0 + 1) >> 8);
        break;
    case 0x100:
        if (D_800608FC != (arg0[1] >> 8)) {
            func_80022224(arg0[1] >> 8);
            func_80023154();
        }
        break;
    case 0x200:
        D_8005B9E4[0] |= arg0[1] >> 8;
        break;
    case 0x300:
        D_8005B9E4[0] &= ~(arg0[1] >> 8);
        break;
    case 0x400:
        D_8005B99B = 1;
        break;
    case 0x500:
        p = D_8005B9E8 + arg0[1] * 3;
        p[0] = arg0[2];
        p[1] = arg0[3];
        break;
    case 0x600:
        i = arg0[1] * 3;
        set = *(arg0 + 2) != 0;
        q = D_8005B9F4 + i;
        if (set) {
            q[2] = 1;
            D_8005B9F0[i] = arg0[3];
            q[1] = 0;
        } else {
            q[2] = -1;
            q[1] = 0;
        }
        break;
    case 0x700:
        D_8005BA00[0] = *(arg0 + 1);
        set = 1;
        D_8005BA10 = set;
        D_8005BA0C = 0;
        D_8005BA04 = *(arg0 + 2);
        break;
    case 0x800:
        D_8005BA10 = -1;
        D_8005BA0C = 0;
        break;
    case 0x900:
        break;
    default:
        printf(D_80010400, *arg0);
        break;
    }
}
