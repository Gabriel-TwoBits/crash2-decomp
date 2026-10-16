#include "common.h"
#include "sdk.h"
#include "game.h"

extern void func_8002D6E4(void *, s32, void *, s32, s32);
extern void func_8003C88C(s32, s32, s32, s32, s32, s32, s32, s32);
extern u8 *D_800608CC;
s32 D_8005F204 = 0xF424;
s32 D_8005F378;
s32 *D_8005F3C4;

void func_8002DE08(s32 *arg0, s32 arg1, u8 *arg2, s32 arg3) {
    s32 *p;
    s32 i;
    s32 flag;
    s32 mode;
    s32 end;
    s32 pad[12];

    if (!(*(s32 *)(*(u8 **)(D_800608CC + 0x10) + 0x29C) & 0x100000)) {
        if (!(*(s32 *)(arg2 + 0x100) & 0x10000000)) {
            p = D_8005F3C4 + 0x1F;
            for (i = 0x1F; i >= 0; i--) {
                *p-- = 0;
            }
            p = (s32 *)D_8005F378 + 0x7F;
            for (i = 0x7F; i >= 0; i--) {
                *p-- = 0;
            }
            mode = *(s32 *)(arg2 + 0x108);
            flag = *(s32 *)(arg2 + 0xB0);
            end = arg3 + 0x1008;
            if ((u32)(mode - 3) < 2U || (flag &= 2, mode == 2)) {
                flag = 2;
            }
            func_8003C88C(arg3, end, flag, *(s32 *)(arg2 + 0x64) + D_8005F204, *(s32 *)(arg2 + 0x64) + 0x3200,
                          arg0[1] + ((*(s32 *)(arg2 + 0x100) & 0x40000000) ? 0x29900 : *(s32 *)(*(u8 **)(arg2 + 8) + 0x40)),
                          arg0[0], arg0[2]);
        }
    }
    func_8002D6E4(arg0, arg1, arg2, arg3, 1);
}
