#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_80067FC4;

void func_80013F28(void *arg0) {
    *(s16 *)((u8 *)arg0 + 4) = 1;
    *((u8 *)arg0 + 0xF) = 0;
    *((u8 *)arg0 + 0xE) = 0;
    D_80067FC4[*(s32 *)((u8 *)arg0 + 0x14) >> 1] = -0x12;
}
