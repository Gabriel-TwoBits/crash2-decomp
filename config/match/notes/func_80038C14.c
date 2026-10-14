#include "common.h"
#include "sdk.h"

typedef struct {
    u8 pad0[0x40];
    s32 regs[0x1F];
    u8 padBC[0xBC - 0x40 - 0x1F * 4];
    s32 *sp;
    u8 padC0[0x12C - 0xC0];
    s32 result;
} Obj;

s32 func_80032B7C(Obj *, s32 *, s32);
s32 *func_80036D0C(Obj *, u32);

static inline s32 *operand(Obj *obj, u32 idx) {
    if ((idx & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            return --obj->sp;
        } else {
            return &obj->regs[idx & 0x1FF];
        }
    }
    return func_80036D0C(obj, idx);
}

static inline void op(Obj *obj, s32 *b, s32 *a) {
    obj->result = func_80032B7C(obj, b, *(s16 *)a);
}

void func_80038C14(Obj *obj, u32 arg1) {
    s32 *a;
    s32 *b;

    a = operand(obj, (arg1 >> 12) & 0xFFF);
    b = operand(obj, arg1 & 0xFFF);
    op(obj, b, a);
}
