#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_800608D4;
extern s32 D_800608E0;

void func_80023D7C(void) {
    s32 a = D_800608D4;
    s32 b = D_800608E0;
    func_800321D4(a, 0x130, b, 0, 0, 3, 0x120);
}
