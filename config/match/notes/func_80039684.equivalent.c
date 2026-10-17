#include "common.h"
#include "sdk.h"
#include "game.h"

#define FIELD_REGS 0x40
#define FIELD_FLAGS 0xA8
#define FIELD_STACK 0xBC
#define FIELD_RESULT 0xD4
#define OPERAND_MASK 0xFFF
#define OPERAND_KIND_MASK 0xE00
#define OPERAND_REG_MASK 0x1FF
#define OPERAND_POP 0xE1F
#define REG_POP 0x1F
#define REG_MASK 0x3F
#define REG_SHIFT 12
#define ARGC_SHIFT 18
#define ARGC_MASK 7
#define MODE_SHIFT 21
#define MODE_MASK 7
#define POP_BYTES_SHIFT 16
#define POP_BYTES_MASK 0x1C
#define MAX_ARGS 7
#define FLAG_HALT 0x20000
#define ARG2_SKIP 2
#define RESULT_HALTED -0xFF

s32 func_8001D194(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

s32 func_80039684(struct Obj *arg0, u32 arg1, s32 arg2) {
    u8 *base = (u8 *)arg0;
    s32 buf[MAX_ARGS + 1];
    s32 *p;
    s32 *q;
    s32 *sp;
    s32 n;
    s32 i;
    s32 idx = arg1 & OPERAND_MASK;
    u32 reg;

    if ((arg1 & OPERAND_KIND_MASK) == OPERAND_KIND_MASK) {
        if (idx == OPERAND_POP) {
            p = *(s32 **)(base + FIELD_STACK) - 1;
            *(s32 **)(base + FIELD_STACK) = p;
        } else {
            p = (s32 *)(base + (arg1 & OPERAND_REG_MASK) * 4 + FIELD_REGS);
        }
    } else {
        p = func_80036D0C(arg0, idx);
    }
    *(u32 *)(base + FIELD_FLAGS) &= ~FLAG_HALT;
    reg = (arg1 >> REG_SHIFT) & REG_MASK;
    if (reg == REG_POP) {
        q = *(s32 **)(base + FIELD_STACK) - 1;
        *(s32 **)(base + FIELD_STACK) = q;
    } else {
        q = (s32 *)(base + reg * 4 + FIELD_REGS);
    }
    if (*q != 0 && p != 0) {
        n = (arg1 >> ARGC_SHIFT) & ARGC_MASK;
        sp = *(s32 **)(base + FIELD_STACK);
        for (i = 0; i < n; i++) {
            buf[i] = sp[i - n];
        }
        func_8001D194((s32)arg0, *p, (arg1 >> MODE_SHIFT) & MODE_MASK, n, (s32)buf);
    } else {
        *(s32 *)(base + FIELD_RESULT) = 0;
    }
    if (!(*(u32 *)(base + FIELD_FLAGS) & FLAG_HALT)) {
        *(u8 **)(base + FIELD_STACK) -= (arg1 >> POP_BYTES_SHIFT) & POP_BYTES_MASK;
        return 0;
    } else if (!(arg2 & ARG2_SKIP)) {
        *(s32 **)(base + FIELD_STACK) -= 1;
        return 0;
    } else {
        return RESULT_HALTED;
    }
}
