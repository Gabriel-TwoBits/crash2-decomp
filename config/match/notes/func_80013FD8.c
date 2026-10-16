#include "common.h"
#include "sdk.h"
#include "game.h"

u8 *func_80013F5C(s32, s32, s32, s32);

s8 func_80013FD8(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 *p;

loop:
    p = func_80013F5C(arg0, arg3, arg1, arg2);
    if (p == (u8 *)-0xFF) {
        return 1;
    }
    if ((arg1 & 1) && (u32)(*(u16 *)(p + 4) - 0x14) >= 2U) {
        func_80012BC4(arg0);
        arg2 = 0;
        goto loop;
    }
    return *(s8 *)(p + 0xF);
}
