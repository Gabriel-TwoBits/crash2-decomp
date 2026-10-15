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

s32 TestEvent(s32);
s32 D_8005F3A0;
s32 D_8005F3A4;
s32 D_8005F3A8;
s32 D_8005F3AC;

void func_80035B70(void) {
    TestEvent(D_8005F3A0);
    TestEvent(D_8005F3A4);
    TestEvent(D_8005F3A8);
    TestEvent(D_8005F3AC);
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035BB8);

void func_80035C40(void) {
    do {
    } while (func_80035BB8() == -1);
}

s32 D_8005F3BC;
s32 D_8005F3C0;
s32 D_8005F3C8;
s32 D_8005F3CC;

void func_80035C74(void) {
    TestEvent(D_8005F3BC);
    TestEvent(D_8005F3C0);
    TestEvent(D_8005F3C8);
    TestEvent(D_8005F3CC);
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035CBC);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035D48);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035E7C);

extern char D_8001069C[];

s32 func_80035F00(char *arg0) {
    return strncmp(D_8001069C, arg0, 12) == 0;
}

s32 close(s32);

void func_80035F2C(s32 *arg0) {
    s32 temp_a0;

    temp_a0 = *arg0;
    if (temp_a0 >= 0) {
        close(temp_a0);
    }
    *arg0 = -1;
}

extern volatile u32 D_8003BEA4;

void func_80035F70(u32 arg0) {
    u32 start;

    start = D_8003BEA4;
    while (start - D_8003BEA4 < arg0) {
    }
}

void func_80035F9C(u8 *arg0) {
    while (*arg0 != 0) {
        if ((u8)(*arg0 - 0x61) < 0x1A) {
            *arg0 = *arg0 - 0x20;
        }
        arg0++;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80035FD8);

s32 func_80036028(s32 arg0) {
    s32 *p = D_8005F264;

    p[2] = 0;
    p[5] = 0;
    func_80036084(arg0);
    return 0;
}

void func_80036050(void) {
    s8 *p;
    s32 i;

    for (i = 0; i < 0xF; i++) {
        if (D_8005F264 != 0) {
            p = ((i * 0x4C) + ((s8 *)D_8005F264)) + 0x34;
            *p = 0;
        }
    }
}

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

void func_80036324(s32 arg0) {
    func_80036028(0);
    func_800360CC(0);
    func_800360AC(0xDF);
    func_80036084(arg0);
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036368);

s32 func_800363D0(void) {
    u32 v = func_80036150();
    int has_ptr;
    do { } while (0); has_ptr = D_8005F264 != 0;
    D_8005F268 = (v >> 4) & 1;
    if (has_ptr) {
        func_800116C0((s32)D_8005F264);
        D_8005F264 = 0;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036414);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003649C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003653C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036598);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800365E4);

s32 func_80036628(void) {
    if (func_8003613C() != 0) {
        func_80036084(2);
        return 1;
    }
    func_800360CC(3);
    func_80035A7C(3);
    return 0;
}

s32 func_80036674(void) {
    s32 ret;
    if (D_8005F264 != 0) {
        func_80036324((func_80036150() & 0x10) | 4);
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800366B4);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003670C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036850);

void func_800368C0(void) {
    s32 i;
    s32 offset;

    i = 0;
    offset = 0x6F4;
    do {
        i++;
        func_8003690C((s8 *)D_8005F264 + offset);
        offset += 0x2A4;
    } while (i < 4);
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_8003690C);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036978);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_800369F4);

void func_80036A84(void) {
}

s32 func_80036A8C(s32 *arg0, s32 arg1) {
    s32 sum;
    s32 i;
    s32 pad[2];

    sum = 0x12345678;
    i = arg1 - 1;
    if (arg1 != 0) {
        do {
            sum += *arg0++;
            i--;
        } while (i != -1);
    }
    return sum;
}

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036AC8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036B68);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036BD8);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036C40);

INCLUDE_ASM("asm/nonmatchings/game/800345C8", func_80036CF8);
