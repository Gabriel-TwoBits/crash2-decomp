#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F1FC[] = "DirnD";

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80020A24);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800217F8);

extern s32 D_80067590;

void func_80021900(s32 count, u32 *p, s32 flags, s32 arg3) {
    u32 *cur;
    u32 v;
    u32 *e;
    s32 neg;

    cur = 0;
    do {
        v = *p;
        if (v & 1) {
            e = *(u32 **)(((v >> 13) & 0x3FC) + D_80067590);
            if (cur == 0 || cur < e) {
                cur = e;
            }
            if (cur[1] != v) {
                while (cur[1] != v) {
                    cur += 2;
                }
                cur -= 2;
                cur += 2;
            }
        } else {
            cur = (u32 *)v;
        }
        if (flags & 8) {
            func_80014260(cur, flags | 2, arg3);
        } else {
            neg = arg3;
            if (flags & 4) {
                neg = -neg;
            }
            func_8001434C((u32)cur, neg);
        }
        count--;
        p++;
    } while (count != 0);
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021A08);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021F4C);

extern s8 D_8005B99B;
extern s32 D_8005B9C4;
extern s32 D_800608F8;

void func_800221DC(void) {
    D_800608F8 = 0;
    if (D_8005B99B != 0) {
        D_8005B9C4 = 0;
        return;
    }
    func_80023154();
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022224);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022314);

extern s32 D_800607FC;
extern s32 D_800608D8[];
extern s32 D_800608E0;
extern s32 D_8006CD98;

void func_800224A0(s32 *arg0) {
    s32 sp28;
    s16 *p;
    s32 new_var;
    s32 lo;
    s32 hi;
    s32 scale;
    s32 d;

    if (func_80031AE8(D_800608D8[0], 0x252, D_800608E0, 0, 0, 0, 0, 0, 0) != 0) {
        p = (s16 *)func_80031AE8(D_800608D8[0], 0x254, D_800608E0, 0, 0, 3, &sp28, 0, 0);
        if (p != 0) {
            lo = p[0] << 8;
            hi = p[1];
            hi = hi << 8;
            new_var = hi;
            scale = p[2];
            if (D_8006CD98 != 0x100) {
                scale = (scale * D_8006CD98) >> 8;
            }
            d = D_800607FC - arg0[2];
            if (new_var < d) {
                d = new_var;
            }
            d -= lo;
            if (d < 0) {
                d = 0;
            }
            D_800607FC -= (d * scale) >> 8;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800225C0);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_8002271C);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022878);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022A5C);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022E74);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022F58);

extern s32 D_8005F38C;

void func_80023084(s32 arg0, s32 arg1) {
    s32 *p;
    s32 n;
    s32 len;
    s32 kind;
    s32 step;

    p = (s32 *)func_80031AE8(arg0, 0x198, arg1, 0, 0, 1, 0, 0, 0);
    if (p != 0) {
        n = func_800322C0(arg0, 0x198, arg1, 1);
        if (n != 0) {
            do {
                kind = p[1];
                len = p[0];
                if (kind >= 0) {
                    func_8001CC10(0, D_8005F38C, kind, len - 2, p + 2);
                    step = len * 4;
                } else {
                    func_80012DC0();
                    step = len * 4;
                }
                n -= len;
                p = (s32 *)((char *)p + step);
            } while (n != 0);
        }
    }
}

typedef struct Unk80060918 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80060918;

typedef struct Unk8005B9C4 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    Unk80060918 unkC;
} Unk8005B9C4;

extern s32 D_8003BEA4;
extern Unk80060918 D_80060918;

void func_80023154(void) {
    Unk8005B9C4 *state = (Unk8005B9C4 *)&D_8005B9C4;

    state->unk0 = 1;
    state->unk8 = 0x155;
    state->unk4 = D_8003BEA4;
    state->unkC = D_80060918;
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800231B0);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800236DC);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800237F8);
