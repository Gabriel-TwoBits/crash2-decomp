#include "common.h"
#include "sdk.h"
#include "game.h"

void *func_80018E48(void *arg0, s32 arg1) {
    if (*(s32 *)((u8 *)arg0 + 0xB8) == arg1) {
        return arg0;
    }
    return 0;
}
