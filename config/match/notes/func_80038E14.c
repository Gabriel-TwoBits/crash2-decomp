#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Obj {
    u8 pad0[0x40];
    s32 regs[(0xBC - 0x40) / 4];
    s32 *sp;
} Obj;

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

void func_80038E14(Obj *obj, u32 arg1) {
    s32 *a;
    s32 *b;
    a = operand_address(obj, (arg1 >> 12) & 0xFFF);
    b = operand_address(obj, arg1 & 0xFFF);
    func_80025FDC(*b, a);
}
