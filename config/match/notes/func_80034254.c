#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80033DE0 {
    s16 a[10];
} Unk80033DE0;
extern Unk80033DE0 D_8005FCD0[];
extern u16 D_8005F2EC;
s16 D_8005F250 = 0;
s32 D_8005F254 = 0;
s32 D_8005F308;
s32 D_8005F2F4;
s32 D_8005F39C;
s16 D_8005F3E4;
s16 D_8005F3E0;
void func_80033EE0(s32);
s32 func_80052B4C(s16, s16, s32, s32);

void func_80034254(s32 arg0) {
    s32 a;

    if (D_8005F39C != 0) {
        if (D_8005F39C == 4) {
            if (D_8005F250 == 0 || (--D_8005F250 << 16) == 0) {
                D_8005F39C = 1;
            }
        }
        if (D_8005F39C == 3) {
            a = *(s32 *)arg0;
            if (a & 1) {
                if (D_8005F2F4 != a) goto try;
            } else if (D_8005F2F4 != ((s32 *)a)[1]) {
try:
                if (D_8005F308 == 0 && D_8005F3E0 != -1 && D_8005F254 == 0) {
                    func_80052B4C(D_8005F3E4, D_8005FCD0[0].a[0], ((s32)(D_8005F2EC << 16)) >> 23, 30);
                    D_8005F250 = 30;
                    D_8005F254 = arg0;
                    D_8005F39C = 4;
                    return;
                }
            }
        }
        if (D_8005F39C < 3 && D_8005F39C > 0) {
            func_80033EE0(arg0);
        }
    }
}
