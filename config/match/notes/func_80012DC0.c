#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_80067FB0[];
extern s32 D_80067FC8;
extern s16 D_8005F3E0;
extern u8 *D_800608CC;
void func_80012BC4(s32);
void func_80034254(s32);

void func_80012DC0(void) {
    D_80067FC8 = 1;
    while (*D_80067FB0 != 0) {
        if (D_800608CC != 0 && D_8005F3E0 != -1) {
            func_80034254(*(s32 *)(D_800608CC + 0x10) + 0x2A4);
        }
        func_80012BC4(-1);
    }
}
