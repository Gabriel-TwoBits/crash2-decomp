#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001D22C(void *arg0, u16 arg1, s32 arg2, s32 *arg3) {
    s32 *p;
    s32 i;
    s32 n = arg2;

    for (i = 0; i < n; i++) {
        p = *(s32 **)((char *)arg0 + 0xBC);
        *(s32 **)((char *)arg0 + 0xBC) = p + 1;
        *p = *arg3++;
    }
    Gool_PushFrame((GoolObject *) arg0, n, 0xFFFF);
    *(s32 *)((char *)arg0 + 0xC0) = *(s32 *)((p = *(char **)((char *)arg0 + 0xC)) + 0x14) + ((arg1 & 0x3FFF) * 4);
    return Gool_Interpret(arg0, 3, 0);
}
