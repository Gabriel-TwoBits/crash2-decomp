#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001E5FC(s32 arg0, s32 arg1, s32 arg2);
s32 func_8002B57C(void *, s32 *, void *, s32);

#define F(p, off) (*(s32 *)((s8 *)(p) + (off)))
#define SUB(p) (*(void **)((s8 *)(p) + 8))

void func_8002BC38(void *arg0, s32 *arg1, s32 *arg2, void *arg3) {
    s32 sp18;
    s32 ret;
    s32 y;
    s32 a;
    s32 b;
    s32 flag;
    void *sub;

    flag = 0;
    if (F(arg0, 0xA8) & 0x100000) {
        sub = SUB(arg0);
        if ((F(sub, 0x4C) & 0x100000) && (F(sub, 0x50) & 0x100000) && (F(sub, 0x54) & 0x100000)
            && (F(sub, 0x58) & 0x100000) && (F(sub, 0x5C) & 0x100000) && (F(sub, 0x60) & 0x100000)
            && (F(sub, 0x64) & 0x100000) && (F(sub, 0x68) & 0x100000)) {
            a = F(arg0, 0x100) & 0x40000000;
            flag = a == 0;
        }
    }
    ret = func_8002B57C(arg0, arg2, arg3, flag);
    if (ret == 0xC4653601) {
        F(arg3, 0x103C) = 0;
    } else {
        F(arg3, 0x103C) = ret;
    }
    if (ret == 0xC4653601) {
        goto fail;
    }
    b = arg2[1];
    if (!(F(arg0, 0x100) & 0x40000000)) {
        if (ret - 1 >= b + F(SUB(arg0), 0x40)) {
            goto fail;
        }
    } else if (ret - 1 >= b + 0x29900) {
        goto fail;
    }
    if (!(F(arg0, 0xA8) & 1)) {
        y = ret - ((F(arg0, 0x100) & 0x40000000) ? 0x29900 : F(SUB(arg0), 0x40)) - 1;
        if (F(arg0, 0x100) & 0x40) {
            if (arg2[1] < y) {
                y = arg2[1];
            }
            arg2[1] = func_8001E5FC(arg1[1], y, 0x4E20);
        } else {
            b = arg2[1];
            a = arg1[1];
            if (a < b) {
                if (y < a) {
                    y = a;
                }
            } else if (y < b) {
                y = b;
            }
            arg2[1] = y;
        }
        F(arg0, 0xA8) |= 0x80;
        if (F(arg0, 0x88) > 0) {
            F(arg0, 0x88) = 0;
        }
        return;
    }
fail:
    if (F(arg0, 0x100) & 0x40) {
        if (ret == 0xC4653601) {
            arg2[1] = arg1[1];
        } else {
            sp18 = 0;
            y = ret - ((F(arg0, 0x100) & 0x40000000) ? 0x29900 : F(SUB(arg0), 0x40)) - 1;
            arg2[1] = func_8001E5FC(arg1[1], y, 0x4E20);
            func_8001CC10(0, (s32)arg0, 0x3300, 1, &sp18);
        }
    }
}
