#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad[8];
    s32 unk8;
    u8 pad2[0x2088 - 0xC];
    void *unk2088;
    u8 pad3[4];
    char *unk2090;
} Unk80063458;

extern Unk80063458 *D_80063458[];

void func_800163E8(s32 arg0) {
    s32 sum;

    sum = D_80063458[0]->unk8 + arg0;
    /* Both branches store the same value; the branch is what puts the sum in $a0. */
    if (D_80063458[0]->unk8) {
        D_80063458[0]->unk8 = sum;
    } else {
        D_80063458[0]->unk8 = sum;
    }
}
