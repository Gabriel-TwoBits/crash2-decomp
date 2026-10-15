#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 *D_80067590;
extern u8 *D_80067594;
extern u8 *D_800675A0;

u8 *func_80014B90(u32 arg0) {
    u8 *p;
    u8 *r;

    p = *(u8 **)(((arg0 >> 13) & 0x3FC) + D_80067590);
    r = p;
    if (*(u32 *)(p + 4) != arg0) {
        p += 8;
        while (1) {
            r = (u8 *)-10;
            if (!(((p - D_80067594) >> 3) < *(s32 *)(D_800675A0 + 0x404))) break;
            if (*(u32 *)(p + 4) == arg0) {
                r = p - 8;
                break;
            }
            p += 8;
        }
    }
    return r;
}
