#include "common.h"
#include "sdk.h"
#include "game.h"

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

extern s32 D_80067FB0[];

void func_80013E10(void *arg0) {
    s8 temp_v0;
    s8 temp_v0_2;

    temp_v0 = *(s8 *)((char *)arg0 + 0xF);
    if (temp_v0 > 0) {
        temp_v0_2 = temp_v0 - 1;
        *(s8 *)((char *)arg0 + 0xF) = temp_v0_2;
        if (((temp_v0_2 << 0x18) <= 0) && (*(s16 *)((char *)arg0 + 6) == 0)) {
            func_80013F28(arg0);
            D_80067FB0[0] -= 1;
        }
    }
}

void func_80013E7C(s8 *arg0) {
    s8 t = arg0[0xE];
    if (t != 0) {
        arg0[0xE] = t - 1;
        func_80013E10(arg0);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013EB0);

extern s32 *D_80067FC4;

void func_80013F28(void *arg0) {
    *(s16 *)((u8 *)arg0 + 4) = 1;
    *((u8 *)arg0 + 0xF) = 0;
    *((u8 *)arg0 + 0xE) = 0;
    D_80067FC4[*(s32 *)((u8 *)arg0 + 0x14) >> 1] = -0x12;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013F5C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80013FD8);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014078);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014148);

typedef struct {
    s32 value;
    s16 pad;
    s16 kind;
} Unk80014148;

Unk80014148 *func_80014148();

s32 func_80014220(void) {
    Unk80014148 *e;

    e = func_80014148();
    if (e && e->kind == 1) {
        return e->value;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014260);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001434C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_800143C0);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014420);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001456C);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014638);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014790);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014918);

typedef struct {
    s32 unk0;
    u32 unk4;
} Unk80014B44;

extern Unk80014B44 **D_80067590;

Unk80014B44 *func_80014B44(u32 arg0) {
    Unk80014B44 **table;
    Unk80014B44 *p;

    table = D_80067590;
    do {
        p = table[(arg0 >> 15) & 0xFF];
        while (p->unk4 != arg0) {
            p++;
        }
    } while (0);
    return p;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80014B90);

s32 func_80014C08(void) {
    s32 temp_v0;

    temp_v0 = func_8001148C(4);
    if (temp_v0 != 0) {
        func_800116C0(temp_v0);
    }
    return temp_v0;
}

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

extern void func_80051328(int);
typedef struct { void (*fn)(); char pad[24]; } Entry;
extern Entry D_8005B1F0[];
extern int D_80067FB4[];
extern int D_80067FBC[];

int func_8001521C(int *p) {
    int i;
    int off;
    char buf[8];
    if (p[0] == 1) {
        if (D_8005F1A4 != 0 && D_80067FB4[0] != 0) func_80012ED0(1);
        if (D_80067FBC[0] != 0) func_80051328(1);
        i = 0;
        func_80034394();
        off = 0;
        func_80032588();
        do {
            void (*f)() = *(void (**)())((char *)D_8005B1F0 + off);
            if (f != 0) f();
            i++;
            off += 0x1C;
        } while (i < 22);
        i = 0;
        if (p[10] > 0) {
            do {
                func_800155E0(p, i);
                i++;
            } while (i < p[10]);
        }
        func_800116C0(p[11]);
        func_800116C0(p[0xA3C / 4]);
        func_800116C0(p[6]);
        i = -0xFF;
        p[0] = 0;
        return i;
    }
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_80015344);

INCLUDE_ASM("asm/nonmatchings/game/800123F0", func_8001557C);

void func_800155E0(void *arg0, s32 arg1) {
    char *p;

    p = (char *)arg0 + arg1 * 0x2C;
    if (*(s16 *)(p + 0x30) != 0) {
        *(s16 *)(p + 0x30) = 0;
    }
}
