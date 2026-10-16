#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;

s32 func_8003649C(void) {
    s32 i;
    s32 *p;

    if (func_80036150() & 0xD) {
        func_80036084(2);
        return 1;
    }
    i = 0;
    p = D_8005F264;
loop:
    if (*(s32 *)((s8 *)p - -(i * 0x4C) + 0x28) == 0) {
        i++;
        if (i < p[2]) {
            goto loop;
        }
        i = p[2];
    }
    func_800360CC(6);
    func_80035A7C(0x20);
    D_8005F264[3] = i;
    return 0;
}
