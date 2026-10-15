#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F3A0;
s32 D_8005F3A4;
s32 D_8005F3A8;
s32 D_8005F3AC;
void func_80031578(s32);

s32 func_80035AB8(void) {
    func_80031578(0x3E7);
    if (TestEvent(D_8005F3A0) == 1) {
        return 0;
    }
    if (TestEvent(D_8005F3A4) == 1) {
        return 1;
    }
    if (TestEvent(D_8005F3A8) == 1) {
        return 2;
    }
    if (TestEvent(D_8005F3AC) != 1) {
        return -1;
    }
    func_80036084(0x10);
    return 3;
}
