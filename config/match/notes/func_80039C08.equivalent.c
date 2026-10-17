#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80027600(void *arg0, s32 arg1, s32 arg2, s32 arg3);
s32 *func_80036D0C(struct Obj *, s32);

#define SP(o) (*(s32 **)((u8 *)(o) + 0xBC))

static inline s32 *Operand(struct Obj *o, s32 idx, u32 v) {
    s32 *r;

    if ((v & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            r = SP(o) - 1;
            SP(o) = r;
        } else {
            r = (s32 *)((u8 *)o + ((v & 0x1FF) * 4 + 0x40));
        }
    } else {
        r = func_80036D0C(o, idx);
    }
    return r;
}

void func_80039C08(struct Obj *arg0, u32 arg1) {
    s32 *src;
    s32 *dst;
    s32 *sp;
    u32 hi;
    u8 *owner;
    s32 packed;
    s32 base;
    s32 addr;
    s32 dstVal;
    s32 srcVal;

    hi = arg1 >> 12;
    src = Operand(arg0, hi & 0xFFF, hi);
    dst = Operand(arg0, arg1 & 0xFFF, arg1);
    sp = SP(arg0);
    owner = *(u8 **)((u8 *)arg0 + 0xC);
    SP(arg0) = sp - 1;
    packed = sp[-1];
    base = *(s32 *)(owner + 0x24);
    SP(arg0) = sp - 2;
    addr = sp[-2];
    dstVal = *dst;
    srcVal = *src;
    func_80027600((void *)addr, base + ((u32)packed >> 8) * 4, dstVal, srcVal);
}
