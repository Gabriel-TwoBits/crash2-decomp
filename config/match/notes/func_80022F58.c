#include "common.h"
#include "sdk.h"
#include "game.h"

void func_800200A8(s32, s32, s32, s32);
void func_800231B0(s32);
s32 func_800267C4(s32 *, s32 *, s32, s32);
extern s8 D_8005B994;
extern s32 D_800608CC[];

void func_80022F58(s32 arg0) {
    s32 sp20[24];
    s32 sp80[4];
    s32 d, c, b, a;

    do {
        a = D_800608CC[0];
        b = D_800608CC[2];
        c = D_800608CC[5];
        d = D_800608CC[11];
        sp80[0] = a;
        sp80[1] = b;
        sp80[2] = c;
        sp80[3] = d;
        if (func_800267C4(sp20, sp80, c, d) == 1) {
            func_800200A8(b = sp20[0], sp20[1], sp20[2], 0);
            if (func_800321D4(sp20[1], 0x29, 0, 0, 0, 2, 0) != D_800608CC[9]) {
                D_8005B994 = 5;
            }
        } else {
            func_800200A8(D_800608CC[0], D_800608CC[2], (func_800322C0(D_800608CC[2], 0x4B, 0, 2) - 1) << 8, 0);
            D_8005B994 = 5;
        }
        func_800231B0(arg0);
    } while (D_8005B994 != 5);
}
