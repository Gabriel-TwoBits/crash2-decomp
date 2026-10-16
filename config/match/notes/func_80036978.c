#include "common.h"
#include "sdk.h"

s32 func_80031578(s32);
void func_80036A84(s32 *, s32, s32, s32);
s32 func_80036A8C(s32 *, s32);
s32 *D_8005F264 = 0;

void func_80036978(void) {
    s32 *p = D_8005F264;
    s32 n;

    p[0x6F4 / 4] = 8;
    n = func_80031578(0xF423F);
    p[0x954 / 4] = n;
    p[0x718 / 4] = 0;
    p[0x718 / 4] = func_80036A8C(p + 0x6F4 / 4, 0x2A4);
    func_80036A84(p + 0x6F4 / 4, 0x2A4, n, 1);
}
