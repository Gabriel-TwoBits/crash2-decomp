#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
s32 func_8003649C();
s32 func_80036598(s32);
s32 func_800365E4(void);

s32 func_8003670C(u32 arg0, s32 arg1) {
    switch (arg0) {
    case 2:
        func_800360AC(0x80);
        return 1;
    case 3:
        return func_8003649C(arg1);
    case 5:
        return func_80036628();
    case 7:
        return func_8003653C();
    case 8:
        return func_800365E4();
    case 10:
        return func_80036674();
    case 12:
        return func_800366B4();
    case 13:
        return func_80036AC8(arg1);
    case 14:
        return func_80036C40(arg1);
    case 15:
        if (D_8005F264 != 0) {
            func_8003690C((char *)D_8005F264 + (arg1 * 0x2A4 + 0x6F4));
            func_80036850();
            return 0;
        }
        return 1;
    case 16:
        return func_80036B68(arg1);
    case 17:
        return func_80036BD8(arg1);
    case 18:
        return func_80036598(arg1);
    case 0:
    default:
        return 1;
    }
}
