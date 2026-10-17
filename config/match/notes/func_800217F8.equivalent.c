#include "common.h"
#include "sdk.h"
#include "game.h"

#define RES_TYPE_A 0x208
#define RES_TYPE_B 0x209
#define SIDE_SELECT 2
#define OFFSET_DELTA 0x100
#define FLAGS_FIRST 0x14
#define FLAGS_SECOND 0xC

void func_800217F8(s32 arg0, s32 arg1, s32 arg2) {
    s32 count;
    s32 type;
    s32 offset;
    s32 p;

    if (arg2 == SIDE_SELECT) {
        type = RES_TYPE_B;
        offset = arg1 - OFFSET_DELTA;
    } else {
        type = RES_TYPE_A;
        offset = arg1 + OFFSET_DELTA;
    }
    p = func_80031AE8(arg0, type, offset, 0, &count, 1, 0, 0, 0);
    if ((p != 0) && (count != 0)) {
        func_80021900(count, (u32 *)p, FLAGS_FIRST, 1);
    }

    if (arg2 == SIDE_SELECT) {
        type = RES_TYPE_A;
    } else {
        type = RES_TYPE_B;
    }
    p = func_80031AE8(arg0, type, arg1, 0, &count, 1, 0, 0, 0);
    if ((p != 0) && (count != 0)) {
        func_80021900(count, (u32 *)p, FLAGS_SECOND, 1);
    }
}
