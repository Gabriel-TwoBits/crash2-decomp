#include "common.h"
#include "sdk.h"
#include "game.h"

#define UNK_ARG1 0x130
#define UNK_ARG6 0x120

extern s32 D_800608D4[];
extern s32 D_800608E0;

void func_80023D7C(void) {
    func_800321D4(*D_800608D4, UNK_ARG1, D_800608E0, 0, 0, 3, UNK_ARG6);
}
