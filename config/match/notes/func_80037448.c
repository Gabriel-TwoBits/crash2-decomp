#include "common.h"
#include "sdk.h"
#include "game.h"

#define STACK_TOP (*(s32 **)(arg0 + 0xBC))

void func_80037448(u8 *arg0, u32 arg1) {
    s32 *a;
    s32 *b;
    s32 *top;
    s32 first;
    u32 hi;

    hi = arg1 >> 12;
    if ((hi & 0xE00) == 0xE00) {
        if ((hi & 0xFFF) == 0xE1F) {
            a = STACK_TOP - 1;
            STACK_TOP = a;
        } else {
            a = (s32 *)(arg0 + (hi & 0x1FF) * 4 + 0x40);
        }
    } else {
        a = func_80036D0C((struct Obj *)arg0, hi & 0xFFF);
    }
    first = *a;
    if ((arg1 & 0xE00) == 0xE00) {
        if ((arg1 & 0xFFF) == 0xE1F) {
            b = STACK_TOP - 1;
            STACK_TOP = b;
        } else {
            b = (s32 *)(arg0 + (arg1 & 0x1FF) * 4 + 0x40);
        }
    } else {
        b = func_80036D0C((struct Obj *)arg0, arg1 & 0xFFF);
    }
    top = STACK_TOP;
    STACK_TOP = top + 1;
    if (*(s32 *)(arg0 + 0xF0) != 0) {
        *top = func_800321D4(*(s32 *)(arg0 + 0xF0), (u32)*b >> 8, first, 0, 0, 0, 0);
        return;
    }
    *top = 0;
}
