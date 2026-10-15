#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80060918 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80060918;

typedef struct Unk8005B9C4 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    Unk80060918 unkC;
} Unk8005B9C4;

extern s32 D_8003BEA4;
extern s32 D_8005B9C4;
extern Unk80060918 D_80060918;

void func_80023154(void) {
    Unk8005B9C4 *state = (Unk8005B9C4 *)&D_8005B9C4;

    state->unk0 = 1;
    state->unk8 = 0x155;
    state->unk4 = D_8003BEA4;
    state->unkC = D_80060918;
}
