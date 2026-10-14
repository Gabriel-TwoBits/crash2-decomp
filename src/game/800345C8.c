#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
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

s32 func_800349F8(s32 arg0, s32 arg1, void (*arg2)(s32), s32 arg3) {
    s32 ret;
    if (arg0 < arg1) {
        ret = 0;
    } else {
        arg2(arg3);
        ret = 1;
    }
    return ret;
}

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
    s32 *p = D_8005F264;
    s32 n = p[5];
    p[5] = n + 1;
    return n >= p[4];
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035AB8);

void func_80035B3C(void) {
    do {

    } while (func_80035AB8() == -1);
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035B70);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035BB8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035C40);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035C74);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035CBC);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035D48);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035E7C);

extern char D_8001069C[];

s32 func_80035F00(char *arg0) {
    return strncmp(D_8001069C, arg0, 12) == 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035F2C);

extern volatile u32 D_8003BEA4;

void func_80035F70(u32 arg0) {
    u32 start;

    start = D_8003BEA4;
    while (start - D_8003BEA4 < arg0) {
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035F9C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035FD8);

s32 func_80036028(s32 arg0) {
    s32 *p = D_8005F264;

    p[2] = 0;
    p[5] = 0;
    func_80036084(arg0);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036050);

extern s32 D_8006CC5C;

s32 func_80036084(s32 a0) {
    s32 *p = D_8005F264;
    s32 v = p[1] | a0;
    p[1] = v;
    D_8006CC5C = v;
    return 0;
}

s32 func_800360AC(s32 arg0) {
    D_8005F264[1] &= ~arg0;
    return 0;
}

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
