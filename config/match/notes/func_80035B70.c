#include "common.h"
#include "sdk.h"
#include "game.h"

s32 TestEvent(s32);
s32 D_8005F3A0;
s32 D_8005F3A4;
s32 D_8005F3A8;
s32 D_8005F3AC;

void func_80035B70(void) {
    TestEvent(D_8005F3A0);
    TestEvent(D_8005F3A4);
    TestEvent(D_8005F3A8);
    TestEvent(D_8005F3AC);
}
