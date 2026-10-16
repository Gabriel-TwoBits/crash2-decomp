#include "common.h"
#include "sdk.h"
#include "game.h"

void func_8002B164(void *, s32 *, s32 *, void *, s32 *);
void func_8002BC38(void *, s32 *, s32 *, void *);
void func_8002DE08(s32 *, s32 *, void *, void *);
s32 func_8002E280(s32 *, s32, s32, s32 *, s32 *, void *, s32);

void func_8002C0F0(void *arg0, void *arg1, s32 *arg2, s32 *arg3, s32 *arg4) {
    s32 pos[4];
    s32 delta[4];
    s32 sp40;
    s32 sp44;
    u32 tmp;
    s32 sp48;
    s32 temp_s2;
    s32 temp_s3;
    s32 x;
    s32 y;
    s32 d;
    s32 i;

    for (i = 0; i < 3; i++) {
        delta[i] = arg3[i];
    }
    for (i = 0; i < 3; i++) {
        pos[i] = arg2[i] + delta[i];
    }
    func_8002B164(arg0, arg2, pos, arg1, &sp40);
    func_8002DE08(arg2, pos, arg0, arg1);
    x = (pos[0] - arg2[0]) * 4;
    if (x < 0) {
        x += 0x1FFF;
    }
    y = (pos[2] - arg2[2]) * 4;
    temp_s2 = (x >> 13) + 0x10;
    if (y < 0) {
        y += 0x1FFF;
    }
    temp_s3 = (y >> 13) + 0x10;
    if (func_8002E280(arg2, temp_s2, temp_s3, &sp44, &sp48, arg0, 0) != 0) {
        pos[0] = ((sp44 - 0x10) << 13 >> 2) + arg2[0];
        pos[2] = ((sp48 - 0x10) << 13 >> 2) + (tmp = arg2[2]);
    } else {
        pos[0] = arg2[0];
        pos[2] = arg2[2];
    }
    *(s32 *)((u8 *)arg1 + 0x104C) += 1;
    if ((temp_s2 != 0x10 || temp_s3 != temp_s2) && temp_s2 == sp44 && temp_s3 == sp48) {
        *(s32 *)((u8 *)arg0 + 0xA8) |= 0x100;
    }
    func_8002BC38(arg0, arg2, pos, arg1);
    func_8002BF58(arg0, pos);
    for (i = 0; i < 3; i++) {
        tmp = pos[i];
        arg4[i] = tmp;
    }
}
