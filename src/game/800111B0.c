#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F17C = 0;
s32 D_8005F180 = 0;
s32 D_8005F184 = 0x1000;
s32 D_8005F188 = 0x200000;
s32 D_8005F18C = 0;
char D_8005F190[] = "dead\n";

#ifdef NON_MATCHING
extern s32 *D_8005F2A4;
extern s32 D_8005F2AC;
extern s32 *D_8005F2B4;

s32 func_800111B0(u32 n) {
    s32 lim;
    s32 hi;
    s32 lo;

    if (n != 0) {
        hi = n;
        lo = n;
        if ((u32)D_8005F2AC < n) {
            lo = D_8005F2AC;
        } else {
            hi = D_8005F2AC;
        }
        lim = ((((char *)D_8005F2B4 - (char *)D_8005F2A4) >> 2) * 4) - 8;
        if (lim >= hi) {
            n = hi;
        } else if (lim >= lo) {
            n = lo;
        } else {
            return -1;
        }
    } else {
        n = 0;
        if ((u32)D_8005F2A4 >= (u32)D_8005F2B4) {
            return -1;
        }
    }
    D_8005F2A4[(s32)n / 4] = -2;
    D_8005F2A4[-1] = n | 1;
    D_8005F2A4 = &D_8005F2A4[((s32)n + 4) / 4];
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800111B0);
#endif

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

#ifdef NON_MATCHING
void bcopy(s32, s32, s32);

s32 func_800116D4(s32 arg0, s32 arg1) {
    s32 r;

    if (arg0 == 0) {
        r = func_8001148C(arg1);
    } else {
        if (arg1 == 0) {
            func_800116C0(arg0);
        } else {
            r = func_800112A4(arg0, arg1);
            if (r == 0) {
                r = func_8001148C(arg1);
                if (r != 0) {
                    bcopy(arg0, r, arg1);
                    func_800116C0(arg0);
                }
            }
        }
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800111B0", func_800116D4);
#endif

#ifdef NON_MATCHING
/* count elements of size bytes from func_8001148C, zeroed; 0 when it returns none */
s32 *func_80011764(s32 count, s32 size) {
    s32 total;
    s32 *block;

    total = size * count;
    block = (s32 *)func_8001148C(total);
    if (block == 0) {
        return 0;
    }
    bzero((u8 *)block, total);
    return block;
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
