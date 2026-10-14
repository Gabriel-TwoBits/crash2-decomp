#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_8005F38C;
extern s32 D_8006CB10;
extern s32 D_8006CB14;

void func_8001EFD8(void) {
    func_800116C0(D_8006CB14);
    D_8005F38C = 0;
    func_800116C0(D_8006CB10);
}
