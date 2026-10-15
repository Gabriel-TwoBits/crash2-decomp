#include "common.h"
#include "sdk.h"
#include "game.h"

void SsSepClose(s16);
void SsVabClose(s16);
s16 D_8005F3E0;

void func_8003444C(s16 a, s32 b) {
    if (a != -1) {
        SsSepClose(a);
    }
    if ((b != 0) && (D_8005F3E0 != -1)) {
        SsVabClose(D_8005F3E0);
    }
}
