#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F17C = 0;
s32 D_8005F180 = 0;
s32 D_8005F184 = 0x1000;
s32 D_8005F188 = 0x200000;
s32 D_8005F18C = 0;
char D_8005F190[] = "dead\n";

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800111B0);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800112A4);

extern s32 *D_8005F2A4;
extern s32 D_8005F2AC;
extern s32 *D_8005F2B4;
extern s32 D_8005F2BC;

void func_8001144C(s32 *arg0, u32 arg1) {
    D_8005F2A4 = arg0;
    do { *arg0 = 0; D_8005F2B4 = &arg0[arg1 >> 2] + 1; } while (0);
    D_8005F2AC = arg1 - 4;
    D_8005F2BC = 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_8001148C);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800116C0);

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800116D4);

#ifdef NON_MATCHING
s32 *func_80011764(s32 count, s32 size) {
    s32 total;
    s32 p;

    total = size * count;
    p = func_8001148C(total);
    if (p == 0) {
        return 0;
    }
    bzero((u8 *)p, total);
    return (s32 *)p;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_80011764);
#endif

void __main(void);
void func_80011800(s32 arg0);
extern s32 D_8005F1A4;

s32 func_800117BC(void) {
    __main();
    D_8005F1A4 = 1;
    func_80015614();
    func_80011800(0x3C);
    func_800157A0();
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_80011800);
