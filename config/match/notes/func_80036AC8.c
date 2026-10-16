#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_80069718[];
s32 *D_8005F264 = 0;

s32 func_80036AC8(s32 arg0) {
    s32 *a1;
    s32 *a2;
    s32 *v1;
    s32 a0;
    s32 a3;
    char *base;

    if (D_8005F264 == 0) {
        return 1;
    }
    base = (char *)D_8005F264 + (arg0 * 0x2A4 + 0x6F4);
    *(s32 *)(base + 4) = 0;
    a1 = (s32 *)(base + 0x60);
    a3 = 0;
    a2 = D_80069718;
    do {
        a0 = 0;
        v1 = a2;
        do {
            *a1++ = *v1++;
            a0++;
        } while (a0 < 2);
        a3++;
        a2 += 2;
    } while (a3 < 0x40);
    func_80036850();
    return 0;
}
