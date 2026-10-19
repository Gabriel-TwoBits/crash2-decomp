#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_80067FC4;
void *func_80013BD8(s32 arg0, s32 arg1, s32 arg2, s8 arg3);

u8 *func_80013F5C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 idx;
    s32 p;

    idx = arg0 >> 1;
    p = D_80067FC4[idx];
    idx = p;
    if (idx != -0x12) {
        if (idx != -0xFF) {
            if (arg2 & 4) {
                *(u8 *)(idx + 0xE) += arg3;
            }
            *(s32 *)(idx + 0x18) = arg1;
            *(u8 *)(idx + 0xF) += arg3;
        }
        return (u8 *)idx;
    }
    return ((u8 *(*)(s32, s32, s32, s32))func_80013BD8)(arg0, arg1, arg2, arg3);
}
