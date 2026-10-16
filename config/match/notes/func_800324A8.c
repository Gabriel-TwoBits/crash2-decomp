#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 mode;
    s16 left;
    s16 right;
    s32 pad[7];
} Unk800324A8;

s32 D_8005F248;
s16 D_8005F2FC;
s16 D_8005F300;
s32 D_8005F308;
extern u8 D_8005F30C[];
extern s32 D_8005F4F0[];
extern u8 D_8005FCF8[];
extern s32 D_8006CBF8;

void func_800324A8(void) {
    Unk800324A8 cfg;
    s32 i;
    s32 *p;
    s32 mask;
    s16 vol;

    D_8005F248 = 1;
    i = 0;
    mask = ~8;
    for (p = D_8005F4F0; i < 24; i++) {
        *p &= mask;
        p += 0x14;
    }
    vol = 0x3FFF;
    SsInit();
    SsSetTableSize(D_8005FCF8, 1, 2);
    SpuInitMalloc(0, D_8005F30C);
    cfg.mode = 3;
    cfg.left = vol;
    cfg.right = vol;
    func_800513D8(&cfg);
    func_800512F8(0);
    func_800323AC();
    func_800325D0((D_8006CBF8 * 0x3FFF00) >> 16);
    D_8005F2FC = vol;
    D_8005F300 = 0;
    SsSetMVol(0x7F, 0x7F);
    D_8005F308 = 0;
}
