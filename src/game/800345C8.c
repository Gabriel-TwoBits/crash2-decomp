#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F264 = 0;
s32 D_8005F268 = 1;
s32 D_8005F26C = -1;
s32 D_8005F270 = -1;
char D_8005F274[] = "bu00:";
char D_8005F27C[] = "%s%s";
char D_8005F284[] = "*";
char D_8005F288[] = "EMPTY";

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800345C8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800346E8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80034810);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80034898);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800349F8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80034A2C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80034AC4);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80034BB8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003516C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800352D8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035444);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800359F0);

void func_80035A64(void) {
}

void func_80035A6C(void) {
}

void func_80035A74(void) {
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035A7C);

s32 func_80035A94(void) {
    s32 *p = (s32 *)D_8005F264;
    s32 n = p[5];
    p[5] = n + 1;
    return n >= p[4];
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035AB8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035B3C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035B70);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035BB8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035C40);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035C74);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035CBC);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035D48);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035E7C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035F00);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035F2C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035F70);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035F9C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035FD8);

s32 func_80036028(void) {
    s32 *p = (s32 *)D_8005F264;

    p[2] = 0;
    p[5] = 0;
    func_80036084();
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036050);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036084);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800360AC);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800360CC);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003613C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036150);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036164);

void StopCARD(void);

s32 func_80036304(void) {
    StopCARD();
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036324);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036368);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800363D0);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036414);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003649C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003653C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036598);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800365E4);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036628);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036674);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800366B4);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003670C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036850);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800368C0);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003690C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036978);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800369F4);

void func_80036A84(void) {
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036A8C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036AC8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036B68);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036BD8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036C40);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036CF8);
