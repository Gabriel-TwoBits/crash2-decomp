#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80038700", func_80038700);

INCLUDE_ASM("asm/nonmatchings/game/80038700", func_80038AFC);

INCLUDE_ASM("asm/nonmatchings/game/80038700", func_80038C14);

typedef struct Obj {
    u8 pad0[0x40];
    s32 regs[(0xBC - 0x40) / 4];
    s32 *sp;
    u8 padC0[0xD4 - 0xC0];
    s32 result;
} Obj;

/* 0xE1F pops the stack, 0xE00-0xFFF name a register, anything else is resolved by func_80036D0C. */
static inline s32 *operand_address(Obj *obj, s32 idx) {
    if ((idx & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            return --obj->sp;
        } else {
            return &obj->regs[idx & 0x1FF];
        }
    } else {
        return func_80036D0C(obj, idx);
    }
}

void func_80038D08(Obj *obj, u32 op) {
    s32 *dst;
    s32 *src;
    s32 srcIdx;
    s32 val;
    s32 flags;

    dst = operand_address(obj, op & 0xFFF);
    flags = (op >> 20) & 0xF;
    srcIdx = (op >> 12) & 0x3F;
    if (srcIdx == 0) {
        val = 0;
    } else {
        if (srcIdx == 0x1F) {
            src = --obj->sp;
        } else {
            src = &obj->regs[srcIdx];
        }
        val = *src;
    }
    if (flags == 0xF) {
        flags = 0x80000000;
    }
    if (op & 0x40000) {
        flags |= 0x40000000;
    }
    if (op & 0x80000) {
        flags |= 0x20000000;
    }
    obj->result = func_80032F18(val, flags, dst, obj);
}

INCLUDE_ASM("asm/nonmatchings/game/80038700", func_80038E14);
