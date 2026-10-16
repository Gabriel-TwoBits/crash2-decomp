#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80021900(s32, s32, s32, s32);

void func_800217F8(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp28;
    s32 v;
    s32 id;
    s32 off;

    id = 0x208;
    if (arg2 != 2) {
        off = 0x100;
    } else {
        id = 0x209;
        if (arg2 != 2) {
            off = 0x100;
        } else {
            off = -0x100;
        }
    }
    v = func_80031AE8(arg0, id, arg1 + off, 0, &sp28, 1, 0, 0, 0);
    if (v != 0 && sp28 != 0) {
        func_80021900(sp28, v, 0x14, 1);
    }
    id = 0x209;
    if (arg2 == 2) {
        id = 0x208;
    }
    v = func_80031AE8(arg0, id, arg1, 0, &sp28, 1, 0, 0, 0);
    if (v != 0 && sp28 != 0) {
        func_80021900(sp28, v, 0xC, 1);
    }
}
