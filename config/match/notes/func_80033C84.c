#include "common.h"
#include "sdk.h"
#include "game.h"
s16 D_8005F2EC;
s16 D_8005F2F8;
s32 D_8005F39C;
s16 D_8005F3E4;
typedef struct { s16 a; char pad[0x12]; } E;
extern E D_8005FCD0[];
void func_80056760(s16, s16, s32, s32);
void func_80033C84(s16 arg0) {
    s32 i;
    s16 new_var;
    s32 pad[2];
    D_8005F2EC = arg0;
    if (D_8005F39C == 3) {
        new_var = (arg0 << 16) >> 23;
        i = 0;
        if (D_8005F2F8 > 0) {
            do {
                func_80056760(D_8005F3E4, D_8005FCD0[i].a, new_var, new_var);
                i++;
            } while (i < D_8005F2F8);
        }
    }
}
