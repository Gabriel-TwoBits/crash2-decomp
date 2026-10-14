#include "common.h"
#include "sdk.h"

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023B04);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023D7C);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023DC4);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023E60);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024188);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800241A8);

s32 func_800322C0(s32, s32, s32, s32);

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

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800242F8);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_8002433C);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024658);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024A48);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024B6C);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800253B0);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800254D4);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80025D18);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80025FDC);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_8002609C);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800260F4);

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80026334);
