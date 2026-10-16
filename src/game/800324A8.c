#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F244 = 0x1E;
s32 D_8005F248 = 0;
s32 D_8005F24C = 0;
s16 D_8005F250 = 0;
s32 D_8005F254 = 0;
s32 D_8005F258 = -1;
s32 D_8005F25C = -1;
s32 D_8005F260 = 0;

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800324A8);

typedef struct {
    s32 a;
    s32 b;
    s16 c;
    s16 d;
    s32 e;
    s32 f;
} Unk80032588;

extern void func_8004FE38(s32);
extern void func_800501A8(Unk80032588 *);

void func_80032588(void) {
    Unk80032588 s;

    if (D_8005F248 != 0) {
        s.a = 7;
        s.b = 0;
        s.c = 0;
        s.d = 0;
        func_800501A8(&s);
        func_8004FE38(0);
        D_8005F248 = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800325D0);

extern void SpuSetReverbDepth(Unk80032588 *);

s32 func_800325DC(s32 arg0, s16 arg1, s16 arg2, s32 arg3, s32 arg4) {
    Unk80032588 s;

    s.a = 7;
    s.b = arg0 | 0x100;
    s.c = arg1;
    s.d = arg2;
    if ((u32)(arg0 - 7) < 2U) {
        s.e = arg3;
        s.f = arg4;
    }
    func_800501A8(&s);
    SpuSetReverbDepth(&s);
    func_8004FE38(arg0 != 0);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_8003264C);

typedef struct {
    s32 a;
    s32 flags;
    s32 pad8;
    struct Obj *key;
    u8 pad10[0xD];
    u8 b;
    u8 pad1E[0x32];
} Ent;

s32 D_8005F304;
extern Ent D_8005F4EC[];

void func_80032808(struct Obj *arg0) {
    s32 sp10;
    s32 i;
    Ent *p;

    if (arg0 != 0) {
        i = D_8005F304;
        p = &D_8005F4EC[i];
        for (; i < 0x18; i++, p++) {
            if ((p->flags & 8) && p->key == arg0) {
                if (i) {
                    D_8005F244 = 0x3C;
                    sp10 = 0;
                    func_80032F18(p->a, 0x40000000, &sp10, p->key);
                    p->key = 0;
                    p->b = 0;
                } else {
                    D_8005F244 = 0x3C;
                    sp10 = 0;
                    func_80032F18(p->a, 0x40000000, &sp10, p->key);
                    p->key = 0;
                    p->b = 0;
                }
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800328D4);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80032A98);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80032B7C);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80032F18);

void func_80013E10(void *arg0);
void func_80051078(s32 arg0, s32 arg1);

void func_80033448(void *arg0) {
    do { func_80051078(0, 1 << ((u16 *)arg0)[3]); } while (0);
    ((s32 *)arg0)[1] &= ~8;
    if (((void **)arg0)[6] != 0) {
        func_80013E10(((void **)arg0)[6]);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800334A0);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033C84);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033D10);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033DD0);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033DE0);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80033EE0);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80034238);

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_80034254);

void func_8003444C(s16 a, s32 b);
s16 D_8005F3E4;

void func_80034394(void) {
    if (D_8005F24C != 0) {
        func_8003444C(D_8005F3E4, 1);
        SsEnd();
        D_8005F24C = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800343CC);

void SsSepClose(s16);
void SsVabClose(s16);
s16 D_8005F3E0;

void func_8003444C(s16 a, s32 b) {
    if (a != -1) {
        SsSepClose(a);
    }
    if ((b != 0) && (D_8005F3E0 != -1)) {
        SsVabClose(D_8005F3E0);
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800324A8", func_800344B0);
