#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800321D4(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    s32 sp28[4];
    u8 *sp38;
    u8 *p;

    p = (u8 *) func_80031AE8(arg0, arg1, arg2, arg3, (s32 *) arg4, arg5, sp28, 1, (s32) &sp38);
    if (p == 0) {
        return arg6;
    }
    switch (sp38[4] & 0x1F) {
    case 1:
        return *p;
    case 17:
        return *(s8 *) p;
    case 2:
        return *(u16 *) p;
    case 18:
        return *(s16 *) p;
    default:
        return *(s32 *) p;
    }
}
