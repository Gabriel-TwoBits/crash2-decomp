#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80013A54(s32, s32);
void func_80027980(s32);
extern s32 D_80069358;

void func_800278F8(s32 arg0) {
    if (*(s32 *)(arg0 + 0xC) != 1) {
        func_800143C0((void *)(arg0 + 0x4C), 1);
        if (*(s32 *)(arg0 + 0x24) != 0) {
            func_80013A54(8, 0);
            func_80013A54(9, 0);
            func_80013A54(0xC, 0);
            func_80013A54(0xD, 0);
            D_80069358 = 0;
        }
        func_80027980(arg0);
    }
}
