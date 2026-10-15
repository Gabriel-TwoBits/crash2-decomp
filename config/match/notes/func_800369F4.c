#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
void func_80036A84(s32 *, s32, s32, s32);
s32 func_80036A8C(s32 *, s32);
void func_800368C0(void);

s32 func_800369F4(void) {
    s32 *base = D_8005F264;
    s32 *p = base + 0x6F4 / 4;
    s32 old;
    s32 ret;

    func_80036A84(p, 0x2A4, base[0x954 / 4], 0);
    old = base[0x718 / 4];
    base[0x718 / 4] = 0;
    if (old == func_80036A8C(p, 0x2A4)) {
        ret = 2;
        if (base[0x6F4 / 4] == 8) {
            return 0;
        }
    } else {
        ret = 1;
    }
    func_800368C0();
    return ret;
}
