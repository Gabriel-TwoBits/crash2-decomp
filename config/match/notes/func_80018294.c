#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800607F4;
extern u32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_800608D4;
extern s32 D_800608E8;

void func_80018294(s32 *arg0) {
    s32 sp[6];
    s32 *s0;
    s32 temp_s4;
    s32 i;
    s32 off;
    s32 *a2;
    s32 *v0;

    i = 0;
    temp_s4 = func_800321D4(D_800608D4, 0x27F, -1, D_800608E8, 0, 2, 0);
    if (*arg0 > 0) {
        s0 = arg0;
        off = 4;
        do {
            if (((temp_s4 & 0xFF) >> i) & 1) {
                v0 = func_80014260((u8 *)arg0 + off, 1, 0);
                a2 = (s32 *)v0[4];
                if (a2[3] == 1) {
                    s0[2] = 0;
                    s0[3] = 0;
                    s0[4] = 0;
                } else {
                    sp[0] = a2[0] - (D_800607F4 >> 8);
                    sp[1] = a2[1] - (D_800607F8 >> 8);
                    sp[2] = a2[2] - (D_800607FC >> 8);
                    s0[2] = sp[0];
                    s0[3] = sp[1];
                    s0[4] = sp[2];
                }
                s0[5] = (s32)a2;
                s0[8] = v0[5];
                s0[6] = v0[6];
                s0[7] = v0[7];
                s0[11] = v0[8];
                s0[9] = v0[9];
                s0[12] = (s32)(a2 + 11);
                s0[10] = v0[10];
            }
            s0 += 12;
            i++;
            off += 0x30;
        } while (i < *arg0);
    }
}
