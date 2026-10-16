#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_ASM("asm/nonmatchings/game/80038700", func_80038700);

#ifdef NON_MATCHING
typedef struct Unk80038AFC {
    char pad0[0xBC];
    s32 *sp;
    char pad1[0xD4 - 0xC0];
    s32 *result;
} Unk80038AFC;

void func_80038AFC(Unk80038AFC *arg0, u32 arg1) {
    s32 i;
    s32 count;
    s32 n;
    s32 *r;
    u32 hi;
    s32 k;

    count = arg1 & 0x3F;
    hi = arg1 >> 0x14;
    n = hi & 0xF;
    if (count == 0) {
        n += 1;
        count = *(arg0->sp - n);
    }
    i = 0;
    if (count > 0) {
        k = hi & 0xF;
        do {
            r = (s32 *)func_80019800(arg0, (arg1 >> 0xC) & 0xFF, (arg1 >> 6) & 0x3F, k, (s32)(arg0->sp + k * -1), (arg1 >> 0x18) == 0x46);
            if ((s32)r < -0xFE) {
                r[0x50 / 4] = (s32)arg0;
                arg0->result = r;
            } else {
                arg0->result = 0;
            }
            i++;
        } while (i < count);
    }
    arg0->sp = arg0->sp - n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038700", func_80038AFC);
#endif

typedef struct Obj {
    u8 pad0[0x40];
    s32 regs[(0xBC - 0x40) / 4];
    s32 *sp;
    u8 padC0[0xD4 - 0xC0];
    s32 result;
    u8 padD8[0x12C - 0xD8];
    s32 unk12C;
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

s32 func_80032B7C(Obj *, s32 *, s32);

void func_80038C14(Obj *obj, u32 arg1) {
    s32 *a;
    s32 *b;

    a = operand_address(obj, (arg1 >> 12) & 0xFFF);
    b = operand_address(obj, arg1 & 0xFFF);
    obj->unk12C = func_80032B7C(obj, b, *(s16 *)a);
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

void func_80038E14(Obj *obj, u32 arg1) {
    s32 *a;
    s32 *b;
    a = operand_address(obj, (arg1 >> 12) & 0xFFF);
    b = operand_address(obj, arg1 & 0xFFF);
    func_80025FDC(*b, a);
}
