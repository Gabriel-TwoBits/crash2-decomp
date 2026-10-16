#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk8001D22CRes {
    char pad0[0x14];
    s32 *f14;
} Unk8001D22CRes;

typedef struct Unk8001D22C {
    char pad0[0xC];
    Unk8001D22CRes *fC;
    char pad10[0xAC];
    s32 *fBC;
    s32 *fC0;
} Unk8001D22C;

void func_8001C620(Unk8001D22C *, s32, s32, s32 *);
void func_8003A014(Unk8001D22C *, s32, s32);

void func_8001D22C(Unk8001D22C *arg0, s32 arg1, s32 arg2, s32 *arg3) {
    s32 *temp_v1;
    s32 i;

    for (i = 0; i < arg2; i++) {
        temp_v1 = arg0->fBC;
        arg0->fBC = temp_v1 + 1;
        *temp_v1 = *arg3;
        arg3++;
    }
    func_8001C620(arg0, arg2, 0xFFFF, arg3);
    arg0->fC0 = arg0->fC->f14 + (arg1 & 0x3FFF);
    func_8003A014(arg0, 3, 0);
}
