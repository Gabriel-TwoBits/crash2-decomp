#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80027664(s32 *);

void func_80039A14(struct Obj *arg0, s32 arg1) {
    u8 *base = (u8 *)arg0;
    s32 idx = arg1 & 0xFFF;
    s32 idx2 = idx;
    s32 *var_v0;
    s32 temp_v0;
    s32 *sp;

    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            var_v0 = *(s32 **)(base + 0xBC) - 1;
            *(s32 **)(base + 0xBC) = var_v0;
        } else {
            var_v0 = (s32 *)(base + ((arg1 & 0x1FF) * 4 + 0x40));
        }
    } else {
        var_v0 = func_80036D0C(arg0, idx2);
    }
    temp_v0 = func_80027664(var_v0);
    sp = *(s32 **)(base + 0xBC);
    *(s32 **)(base + 0xBC) = sp + 1;
    *sp = temp_v0;
}
