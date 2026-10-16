#include "common.h"
#include "sdk.h"
#include "game.h"

extern char *D_800608CC;
extern s32 *D_8006759C;
extern u16 D_8006935C[];

void func_8001F8B4(s32 arg0) {
    u16 *p;
    u16 *free;
    u16 key;

    key = (arg0 >> 8) | (D_8006759C[2] << 9);
    free = 0;
    p = D_8006935C;
    if (!(*(s32 *)(*(char **)(D_800608CC + 0x10) + 0x29C) & 0x2000)) {
        for (; *p != 0; p++) {
            if (*p == key) {
                return;
            }
            if (*p == 1) {
                free = p;
            }
        }
        if (free == 0) {
            free = p;
        }
        *free = key;
    }
}
