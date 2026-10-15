#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80031578(s32);
s32 D_8005F3BC;
s32 D_8005F3C0;
s32 D_8005F3C8;
s32 D_8005F3CC;

s32 func_80035BB8(void) {
    func_80031578(0x3E7);
    if (TestEvent(D_8005F3BC) == 1) {
        return 0;
    }
    if (TestEvent(D_8005F3C0) == 1) {
        return 1;
    }
    if (TestEvent(D_8005F3C8) == 1) {
        return 2;
    }
    if (TestEvent(D_8005F3CC) == 1) {
        return 3;
    }
    return -1;
}
