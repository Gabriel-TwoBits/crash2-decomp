#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019098);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019800);

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019988);

s32 D_8005F38C;
extern s32 D_8006B1C4[];
extern s32 *D_8006CB10[];
extern s32 D_8006CB58[];
extern s32 D_8006CB5C;
extern s32 D_8006CBA4;

s32 func_800189CC(s32 *, s32 *);
void func_80032808(s32 *);

s32 func_80019BBC(s32 *arg0, s32 arg1) {
    s32 *child;
    s32 *next;
    s32 *slot;
    s32 *base;
    s32 cur;

    if (arg0 != 0 && (s32)arg0 != D_8005F38C) {
        if (arg1 != 0) {
            func_8001CC10(0, (s32)arg0, 0x1A00, 0, 0);
        }
        child = (s32 *)arg0[0x4C / 4];
        while (child != 0) {
            next = (s32 *)child[0x48 / 4];
            func_80019BBC(child, arg1);
            child = next;
        }
        func_80032808(arg0);
        cur = D_8005F38C;
        D_8006B1C4[arg0[0xB8 / 4] >> 8] &= ~1;
        arg0[0] = 0;
        if ((s32)arg0 == cur) {
            if (D_8006CB58[0] == 2) {
                arg1 = D_8006CB5C;
            } else {
                arg1 = D_8006CBA4;
            }
            base = D_8006CB58;
            func_800189CC(arg0, base);
            /* the block wrapper changes the store scheduling */
            do {
                slot = base + 0x13;
                if (base[0] == 2) {
                    slot = base + 1;
                }
                *slot = arg1;
                D_8006CB10[0][0x44 / 4] = 0;
                D_8006CB10[0][0x4C / 4] = 0;
                D_8006CB10[0][0x48 / 4] = 0;
            } while (0);
            D_8005F38C = 0;
            return -0xFF;
        }
        func_800189CC(arg0, D_8006CB58);
    }
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/80019098", func_80019D30);

void func_80019E08(s32 a0) {
    func_80019BBC((s32 *)a0, 1);
}

extern char func_80019D30[];

void func_80019E28(void *arg0) {
    func_80018DD0(func_80018D28, func_80019D30, arg0);
}

typedef struct {
    u8 pad[0x190];
    s32 count;
    s32 items[1];
} Unk19E5C;

typedef struct {
    u8 pad[0x10];
    Unk19E5C *unk10;
} Unk608CC;

/* volatile: the retail code reloads it after the null check. */
extern Unk608CC *volatile D_800608CC;

void func_80019E5C(void) {
    Unk19E5C *obj;
    s32 i;

    if (D_800608CC != 0) {
        obj = D_800608CC->unk10;
        for (i = 0; i < obj->count; i++) {
            if (func_800143C0(&obj->items[i], 0) != 0) {
                func_80019E28(func_80014260(&obj->items[i], 1, 0));
            }
        }
    }
}

extern char D_8005C514[];
extern char D_8005C650[];
extern s32 D_80060944;
extern u32 *D_80063450;
extern char D_8006CB70[];

void func_80019F08(void) {
    u32 n;
    u32 *p;

    n = D_80063450[5];
    *(s32 *)0x1F800050 = D_80060944;
    p = (u32 *)0x1F800054;
    if (n >= 0x67) {
        n = 0x66;
    }
    *p = n;
    *(char **)0x1F800058 = D_8006CB70;
    func_8003B940(D_8005C514, (void *)0x1F800060, (D_8005C650 - D_8005C514) / 2);
    *(s32 *)0x1F80005C = 0x1F800060;
}

extern u8 D_8006CB30[];
extern u8 func_80018E48[];

s32 func_80019FA0(s32 arg0) {
    u8 *p;
    u8 *a;
    s32 i;
    s32 r;

    i = 3;
    p = D_8006CB30;
    do {
        a = p;
        p += 8;
        i++;
        r = func_80018BE4(a, func_80018E48, arg0);
        if (r != 0) {
            break;
        }
        a = p;
    } while (i < 5);
    return r;
}
