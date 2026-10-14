#include "common.h"
#include "sdk.h"

s32 D_8005F1A4 = 1;
char D_8005F1A8[] = "hde\n";
RECT D_8005F1B0 = { 0, 0, 0x100, 0x80 };
char D_8005F1B8[] = "de ls\n";
char D_8005F1C0[] = "de l\n";
char D_8005F1C8[] = "!mem\n";
s32 D_8005F1D0 = 0;

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800123F0);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001254C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800125F8);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012A20);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012BC4);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012DC0);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012E4C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012ED0);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80012F24);

typedef struct {
    u8 pad0[0x14];
    s32 unk14;
} Unk80013038Sub;

typedef struct {
    u8 pad0[0x1C];
    s32 unk1C;
    u8 pad20[0x4];
    Unk80013038Sub *unk24;
} Unk80013038;

s32 func_80013038(Unk80013038 *arg0, s32 arg1) {
    s32 temp;

    if (arg1 == -1) {
        return 1;
    }
    arg1 >>= 1;
    temp = arg0->unk24->unk14 >> 1;
    if (temp >= arg1) {
        if (temp - arg0->unk1C < arg1) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013084);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800130A0);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013200);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800136C8);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013888);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013968);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013A54);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013AE8);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013BD8);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013C98);

void func_80013F28(void);
extern s32 D_80067FB0[];

void func_80013E10(void *arg0) {
    s8 temp_v0;
    s8 temp_v0_2;

    temp_v0 = *(s8 *)((char *)arg0 + 0xF);
    if (temp_v0 > 0) {
        temp_v0_2 = temp_v0 - 1;
        *(s8 *)((char *)arg0 + 0xF) = temp_v0_2;
        if (((temp_v0_2 << 0x18) <= 0) && (*(s16 *)((char *)arg0 + 6) == 0)) {
            func_80013F28();
            D_80067FB0[0] -= 1;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013E7C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013EB0);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013F28);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013F5C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013FD8);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014078);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014148);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014220);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014260);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001434C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800143C0);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014420);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001456C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014638);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014790);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014918);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014B44);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014B90);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014C08);

extern s32 D_8005F184;
extern s32 D_8005F188;

s32 func_80014C48(void) {
    s32 a = D_8005F184;
    s32 b = D_8005F188;

    /* The loop notes keep the scheduler from hoisting `lui 0x8000` above the loads. */
    do {
        a -= 0x80000000;
    } while (0);
    return b - a;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014C68);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001521C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80015344);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001557C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800155E0);
