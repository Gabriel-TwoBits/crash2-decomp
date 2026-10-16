#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001BFDC);

extern u8 D_8005C514;
extern s32 D_8005F38C;
extern s32 D_80060944;
extern u32 *D_80063454;
extern u32 D_8006CB64;
extern s32 D_8006CCAC;
extern u8 func_80018CAC;
extern u8 func_8001C718;

void func_8001C29C(void *arg0) {
    D_8006CB64 = D_80063454[3] / 34;
    func_8002CA98();
    D_8006CCAC = D_80060944;
    func_80019F08();
    if (D_8005F38C != 0) {
        func_80018DD0(&func_80018CAC, &func_8001C718, arg0);
    }
    *(u8 **)0x1F80005C = &D_8005C514;
}

INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C340);

#ifdef NON_MATCHING
s32 func_8001C620(u8 *arg0, s32 arg1, s32 arg2) {
    u8 *cur;
    s32 prev;
    s32 *p;

    cur = *(u8 **)(arg0 + 0xBC);
    p = *(s32 **)(arg0 + 0xBC);
    prev = *(s32 *)(arg0 + 0xC4);
    *(u8 **)(arg0 + 0xC4) = cur;
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = arg2;
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = *(s32 *)(arg0 + 0xC0);
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = ((cur - arg1 * 4 - 0x40) - arg0) | ((prev - 0x40 - (s32)arg0) << 16);
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C620);
#endif

#ifdef NON_MATCHING
s32 func_8001C688(u8 *arg0, u32 *arg1) {
    u32 temp_a3;
    u32 temp_t0;
    u32 *v1;
    u32 *v1b;
    u32 w;

    v1 = *(u32 **)(arg0 + 0xC4);
    *(u32 **)(arg0 + 0xBC) = v1 + 2;
    temp_a3 = v1[2];
    temp_t0 = temp_a3 >> 16;
    if (temp_t0 == 0) {
        return -0x19;
    }
    *(u32 **)(arg0 + 0xBC) = v1 + 1;
    *(u32 *)(arg0 + 0xC0) = v1[1];
    v1b = *(u32 * volatile *)(arg0 + 0xBC);
    *(u32 **)(arg0 + 0xBC) = v1b - 1;
    w = v1b[-1];
    if (arg1 != 0) {
        *arg1 = (*arg1 & 0xFFFF0000) | (w & 0xFFFF);
    }
    *(u8 **)(arg0 + 0xBC) = &arg0[(temp_a3 & 0xFFFF) + 0x40];
    *(u8 **)(arg0 + 0xC4) = &arg0[temp_t0 + 0x40];
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C688);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C718);

INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001CC10);
