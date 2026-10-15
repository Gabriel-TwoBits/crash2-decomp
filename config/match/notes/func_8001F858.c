#include "common.h"
#include "sdk.h"
#include "game.h"

extern u16 D_8006935C[];
extern s32 D_8006B1C4[];

void func_8001F858(s32 arg0) {
    u16 *p;

    for (p = D_8006935C; *p != 0; p++) {
        if ((*p >> 9) == arg0) {
            D_8006B1C4[*p & 0x1FF] |= 8;
        }
    }
}
