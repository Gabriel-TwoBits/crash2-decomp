#include "common.h"
#include "sdk.h"
#include "game.h"

extern void exit(s32);
extern s32 func_80036368(void);
extern s32 func_8003BC5C(u8 *, s32);
extern s32 D_8005F3EC;
extern s32 D_8006344C;
typedef struct {
    u8 pad0[0x8];
    s32 unk8;
    u8 padC[0x202C - 0xC];
    u16 unk202C;
    u16 unk202E;
    u8 pad2030[0x2088 - 0x2030];
    void *unk2088;
    u8 pad208C[0x2090 - 0x208C];
    char *unk2090;
} Unk80063458;

extern Unk80063458 *D_80063458[];
extern u8 *D_80063454;

s32 func_800161F8(void) {
    char *s2;
    char *s0;
    s32 i;
    s32 buf;
    s32 v;
    s32 a0;

    if (D_8005F3EC == 2) {
        func_80036368();
    }
    func_8003BC5C(&D_80063458[0]->padC[0xC], 0x800);
    func_8003BC5C(D_80063454 + 0x18, 0x800);
    i = 0;
    buf = func_8001148C(0x1770);
    if (D_8006344C > 0) {
        s2 = (char *)D_80063458 + 4;
        s0 = s2;
        do {
            a0 = 0x16400;
            *(s32 *)(s0 + 0x208C) = buf;
            if (D_8005F3EC == 0x28) {
                a0 = 0x2C800;
            }
            v = func_8001148C(a0);
            *(s32 *)(s0 + 4) = v;
            if (v == 0) {
                exit(-0xF);
            }
            func_800163C4((s32 *)s0);
            i++;
            s0 += 0x2094;
        } while (i < *(s32 *)(s2 - 0x10));
    }
    return -0xFF;
}
