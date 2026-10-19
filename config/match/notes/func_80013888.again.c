#include "common.h"
#include "sdk.h"

void Nsf_ReleaseTextureSlot();
extern char D_80062D70[];
extern signed char D_80062D7B[];
extern char D_80063000[];

s32 func_80013888(void) {
    char *p;
    s32 i;
    s32 off;

    p = D_80063000;
    off = 4;
    for (i = 15; i >= 0; i--, p -= 0x2C) {
        if (*(s16 *)(p + off) == 1) return i;
    }
    off = 0x294;
    for (i = 15; i >= 0; i--, off -= 0x2C) {
        if (*(s16 *)(D_80062D70 + off) == 0x15) return i;
    }
    p = D_80063000;
    off = 0x294;
    for (i = 15; i >= 0; p -= 0x2C, i--, off -= 0x2C) {
        if ((u32)(*(u16 *)(p + 4) - 0x14) < 2U) {
            if (D_80062D7B[off] == 0) {
                Nsf_ReleaseTextureSlot(i);
                return i;
            }
        }
    }
    return -0xC;
}
