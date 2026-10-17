#include "common.h"
#include "sdk.h"
#include "game.h"

#define OP_LOAD_A 1
#define OP_CALL_B 2
#define OP_CALL_C 3
#define OP_CALL_D 4
#define OP_CALL_E 5
#define OP_LOAD_F 6
#define FIELD_RESULT 0xD4
#define FIELD_STACK 0xBC
#define OPERAND_MASK 0xFFF
#define OPERAND_SHIFT 12

s32 *func_80036E30(s32, s32, s32);

void func_80038F00(s32 arg0, u32 arg1) {
    s32 *src;
    u8 *sp;
    s32 op;
    s32 n;
    s32 v;

    op = *func_80036E30(arg0, (arg1 >> OPERAND_SHIFT) & OPERAND_MASK, 0);
    src = func_80036E30(arg0, arg1 & OPERAND_MASK, 0);
    switch (op) {
    case OP_LOAD_A:
        *(void **)(arg0 + FIELD_RESULT) = func_80014260(src, 0, 1);
        break;
    case OP_LOAD_F:
        *(void **)(arg0 + FIELD_RESULT) = func_80014260(src, 1, 1);
        break;
    case OP_CALL_B:
        *(s32 *)(arg0 + FIELD_RESULT) = func_800143C0(src, 1);
        break;
    case OP_CALL_C:
        v = func_800143C0(src, 0);
        *(*(s32 **)(arg0 + FIELD_STACK))++ = v;
        break;
    case OP_CALL_D:
        v = func_8001456C();
        *(*(s32 **)(arg0 + FIELD_STACK))++ = v;
        break;
    case OP_CALL_E:
        n = *src;
        v = func_80014918((u32 **)(*(u8 **)(arg0 + FIELD_STACK) - n * 4), n);
        sp = *(u8 **)(arg0 + FIELD_STACK) - n * 4;
        *(u8 **)(arg0 + FIELD_STACK) = sp + 4;
        *(s32 *)sp = v;
        break;
    default:
        break;
    }
}
