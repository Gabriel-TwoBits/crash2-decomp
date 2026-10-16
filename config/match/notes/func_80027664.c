#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 *D_80069358;
void func_80026EF4(u8 *);
u8 *func_800277A8(s32, s32, u32, s32);

s32 func_80027664(s32 *arg0) {
    u8 *obj;
    u8 *p;
    u8 *entry;
    s32 *list;
    s32 count;
    s32 i;
    s32 n;
    s32 flag;
    u32 b;
    u8 new_var;
    u8 c;

    list = func_80014260(arg0, 1, 1);
    count = list[3];
    entry = 0;
    n = 0;
    for (i = 0; i < count; i++) {
        p = (u8 *)list[4 + i];
        b = *p >> 6;
        if (b == 0) {
            entry = p;
        } else if (b == 2) {
            n++;
        }
    }
    if (entry != 0 && n != 0) {
        new_var = entry[0];
        c = entry[3];
        flag = new_var & 1;
        count = entry[1];
        obj = func_800277A8((count << 8) | entry[2], ((c & 0x1F) << 8) | entry[4], (c & 0xE0) >> 3, flag);
        if (obj != 0) {
            *(s32 *)obj = -0x100;
            *(s32 *)(obj + 0x14) = 0;
            *(s32 *)(obj + 4) = n << 8;
            *(void **)(obj + 0x50) = list;
            *(s32 *)(obj + 0x4C) = *arg0;
            if (flag) {
                D_80069358 = obj;
                func_80026EF4(obj);
            }
            return (s32)obj;
        }
        return 0;
    }
    return 0;
}
