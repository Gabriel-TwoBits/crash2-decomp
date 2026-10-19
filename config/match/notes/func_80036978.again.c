#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *g_SaveState = 0;

typedef void (*Unk80036A84Fn)(s32 *, s32, s32, s32);

void func_80036978(void) {
    s32 *p = g_SaveState;
    s32 n;

    p[0x6F4 / 4] = 8;
    p++;
    p--;
    n = Rand_Below(0xF423F);
    p[0x954 / 4] = n;
    p[0x718 / 4] = 0;
    p[0x718 / 4] = func_80036A8C(p + 0x6F4 / 4, 0x2A4);
    /* game.h declares func_80036A84 as void(void); the assembly passes four arguments. */
    ((Unk80036A84Fn)func_80036A84)(p + 0x6F4 / 4, 0x2A4, n, 1);
}
