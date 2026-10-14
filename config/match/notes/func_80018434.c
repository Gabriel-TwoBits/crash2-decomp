#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u8 pad0[0x8];
    s32 unk8;
} Unk80063458;

extern Unk80063458 *D_80063458[];
extern s32 D_8005F390;
extern s32 D_80060944;
extern s32 D_80062A18;

void func_80041E5C(s32, Unk80063458 *, void *, s32);

void func_80018434(void) {
    func_80041E5C(D_8005F390, D_80063458[0], &D_80062A18, D_80060944);
}
