#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80033DE0 {
    s16 a[10];
} Unk80033DE0;

void func_800344B0(Unk80033DE0 *, s32, s32);
void func_80056760(s16, s16, s16, s16);
extern Unk80033DE0 D_8005FCD0[];
extern s16 D_8005FCE4;
extern s32 D_8006CBF8;
extern s32 D_8006CBFC;
s32 D_8005F308;
s16 D_8005F2F8;
s32 D_8005F39C;
s16 D_8005F3E4;

void func_80033DE0(s32 arg0) {
    s32 a;
    Unk80033DE0 *p;

    if (D_8005F2F8 >= 2) {
        a = arg0 >> 8;
        if (D_8005F39C == 3) {
            if (a <= 0 || a < 3 || a != D_8005F39C) {
                p = &D_8005FCD0[1];
                func_800344B0(p, 1, 0);
                func_800344B0(p - 1, 3, 0);
                D_8005F308 = 0;
            } else {
                if (D_8006CBFC != 0 || D_8006CBF8 != 0) {
                    D_8005F308 = 0x3FFF;
                } else {
                    D_8005F308 = 0;
                }
                p = &D_8005FCD0[0];
                func_800344B0(p, 2, 0);
                func_800344B0(p + 1, 0, 1);
            }
            func_80056760(D_8005F3E4, D_8005FCE4, D_8005F308, D_8005F308);
        }
    }
}
