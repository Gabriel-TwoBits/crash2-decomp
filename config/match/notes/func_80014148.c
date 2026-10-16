#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 value;
    s16 pad;
    s16 kind;
    u8 rest[0x24];
} Unk80014148;

extern Unk80014148 D_80063000;
extern s32 D_800675B0;
extern s32 D_800675B4;

Unk80014148 *func_80014148(u32 arg0) {
    Unk80014148 *a2;
    Unk80014148 *v1;
    s32 a1;
    s32 a0;

    if (arg0 & 2) {
        if ((arg0 & 0xC0000002) == 0x80000002) {
            a2 = &D_80063000;
            for (a1 = 15; a1 >= 0; a1--) {
                v1 = a2;
                if ((u16)v1->pad - 0x14 < 2U && arg0 == *(u32 *)((u8 *)v1 + 0x1C)) {
                    return a2;
                }
                a2 = (Unk80014148 *)((u8 *)v1 - 0x2C);
            }
        }
        return 0;
    }
    a0 = arg0 - D_800675B4;
    if (a0 < 0) {
        a0 += 0xFFFF;
    }
    a0 >>= 16;
    if (a0 >= 0 && a0 < D_800675B0) {
        return (Unk80014148 *)((u8 *)&D_800675B4 + a0 * 0x2C);
    }
    return 0;
}
