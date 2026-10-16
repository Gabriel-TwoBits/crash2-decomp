#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023B04);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023D7C);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023DC4);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023E60);

s32 func_80024188(void) {
    func_800241A8();
    return -0xFF;
}

extern s8 D_8005B994;
extern s8 D_8005B99A;
extern s32 D_8005B9AC;
extern s32 D_8005B9BC;
extern s32 D_8005B9C0;
extern s32 D_8005B9C4;
extern s32 D_8005B9E0;
extern s32 D_8005B9E4;
extern s32 D_8005B9F4;
extern s32 D_8005B9F8;
extern s32 D_8005B9FC;
extern s32 D_8005BA08;
extern s32 D_8005BA0C;
extern s32 D_8005BA10;
extern s32 D_800608F0;
extern s32 D_800608F8;
extern s32 D_80060918;
extern s32 D_8006CD98;

void func_800241A8(void) {
    D_8005B994 = 5;
    D_80060918 = 0x1001;
    D_8005B9AC = 0x1001;
    D_800608F0 = 5;
    D_800608F8 = 0;
    D_8005B9E0 = 0;
    D_8005B9E4 = 0;
    D_8005B9C4 = 0;
    D_8005B99A = 0;
    D_8005B9BC = 0;
    D_8005B9C0 = 0;
    D_8005B9FC = 0;
    D_8005B9F8 = 0;
    D_8005B9F4 = 0;
    D_8005BA10 = 0;
    D_8005BA0C = 0;
    D_8005BA08 = 0;
    D_8006CD98 = 0x100;
    func_80022224(1);
}

s32 func_80024268(s32 *p, s32 d, s32 *out) {
    s32 v;
    s32 lim;
    s32 over;
    s32 k;
    k = p[3];
    do {
        v = p[2];
        if (k == 2) {
            v += d;
            lim = (func_800322C0(p[1], 0x4B, 0, 2) - 1) << 8;
            over = v - lim;
            if (over > 0) {
                *out = lim;
                return over;
            }
        } else {
            v -= d;
            if (v < 0) {
                *out = 0;
                return -v;
            }
        }
    } while (0);
    *out = v;
    return 0;
}

s32 func_800242F8(s32 arg0) {
    switch (arg0) {
    case 0:
        return 4;
    case 3:
        return 1;
    case 8:
        return 2;
    default:
        return 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_8002433C);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024658);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024A48);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024B6C);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800253B0);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800254D4);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80025D18);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80025FDC);

typedef struct Unk8005BA58 {
    u8 pad[0x10];
} Unk8005BA58;

void func_800260F4(Unk8005BA58 *);
extern s32 D_8005B9E0;
extern Unk8005BA58 D_8005BA58[];

void func_8002609C(void) {
    Unk8005BA58 *p;
    s32 n;

    n = D_8005B9E0;
    p = D_8005BA58;
    while (n > 0) {
        func_800260F4(p);
        n--;
        p++;
    }
    D_8005B9E0 = 0;
}

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800260F4);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80026334);
