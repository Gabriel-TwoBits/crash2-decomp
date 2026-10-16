#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80027420(u8 *, s32, s32, s32 *, u32, s32);
void func_80027500(s32, s32, s32, s32, s32, s32, s32);

s32 func_800272A4(u8 *arg0, u8 *arg1) {
    s32 sp20;
    s32 a;
    s32 n;
    s32 ret;
    u32 mode;
    u32 f;
    u8 *q;

    f = arg1[0];
    q = arg1 + 4;
    ret = 0;
    switch (f >> 6) {
    case 1:
        func_80027420(q, *(s32 *)(arg0 + 0x38), f & 7, 0, 0, *(s32 *)(arg0 + 0x44));
        ret = 0x200;
        break;
    case 2:
        mode = (f & 0x38) >> 3;
        n = f & 7;
        switch (mode) {
        case 0:
            if (n != 0) {
                func_80027420(arg1 + 1, *(s32 *)(arg0 + 0x34), n, 0, 0, *(s32 *)(arg0 + 0x44));
                ret = 0x100;
            }
            break;
        case 1:
            a = func_80027420(arg1 + ((arg1[2] << 8) | arg1[3]), *(s32 *)(arg0 + 0x40), arg1[1] & 7, 0, mode, *(s32 *)(arg0 + 0x44));
            func_80027500(func_80027420(q, *(s32 *)(arg0 + 0x3C), n, &sp20, mode, *(s32 *)(arg0 + 0x44)), a, *(s32 *)(arg0 + 0x34), sp20, arg1[1] & 8, *(s32 *)(arg0 + 0x18), *(s32 *)(arg0 + 0x1C));
            ret = 0x100;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return ret;
}
