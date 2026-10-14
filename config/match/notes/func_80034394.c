#include "common.h"
#include "sdk.h"
#include "game.h"

void SsEnd(void);
void func_8003444C(s16 a, s32 b);
s32 D_8005F24C;
s16 D_8005F3E4;

void func_80034394(void) {
    if (D_8005F24C != 0) {
        func_8003444C(D_8005F3E4, 1);
        SsEnd();
        D_8005F24C = 0;
    }
}
