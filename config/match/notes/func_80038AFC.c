#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80038AFC {
    char pad0[0xBC];
    s32 *sp;
    char pad1[0xD4 - 0xC0];
    s32 *result;
} Unk80038AFC;

void func_80038AFC(Unk80038AFC *arg0, u32 arg1) {
    s32 i;
    s32 count;
    s32 n;
    s32 *r;
    u32 hi;
    s32 k;

    count = arg1 & 0x3F;
    hi = arg1 >> 0x14;
    n = hi & 0xF;
    if (count == 0) {
        n += 1;
        count = *(arg0->sp - n);
    }
    i = 0;
    if (count > 0) {
        k = hi & 0xF;
        do {
            r = (s32 *)func_80019800(arg0, (arg1 >> 0xC) & 0xFF, (arg1 >> 6) & 0x3F, k, (s32)(arg0->sp + k * -1), (arg1 >> 0x18) == 0x46);
            if ((s32)r < -0xFE) {
                r[0x50 / 4] = (s32)arg0;
                arg0->result = r;
            } else {
                arg0->result = 0;
            }
            i++;
        } while (i < count);
    }
    arg0->sp = arg0->sp - n;
}
