#include "common.h"
#include "sdk.h"
#include "game.h"

#define OPERAND_MASK 0xFFF
#define OPERAND_KIND_MASK 0xE00
#define OPERAND_KIND_STACK 0xE00
#define OPERAND_STACK_POP 0xE1F
#define OPERAND_REG_MASK 0x1FF
#define OPERAND_FIELD_SHIFT 12
#define RESULT_SHIFT 8
#define OBJ_REGS_OFFSET 0x40
#define OBJ_SP_OFFSET 0xBC
#define OBJ_HANDLE_OFFSET 0xF0
#define WORD_SIZE 4

#define SP_OF(obj) ((s32 **)((u8 *)(obj) + OBJ_SP_OFFSET))

/* Resolves an operand to the address of its word; a pop moves the stack pointer down. */
#define RESOLVE_OPERAND(out, obj, operand) \
    if (((operand) & OPERAND_KIND_MASK) == OPERAND_KIND_STACK) { \
        if (((operand) & OPERAND_MASK) == OPERAND_STACK_POP) { \
            *SP_OF(obj) = (s32 *)((u8 *)*SP_OF(obj) - WORD_SIZE); \
            (out) = *SP_OF(obj); \
        } else { \
            (out) = (s32 *)((u8 *)(obj) + OBJ_REGS_OFFSET + ((operand) & OPERAND_REG_MASK) * WORD_SIZE); \
        } \
    } else { \
        (out) = func_80036D0C((obj), (operand) & OPERAND_MASK); \
    }

void func_80037448(struct Obj *arg0, u32 arg1) {
    s32 first;
    s32 *first_ptr;
    s32 *second;
    s32 *dst;
    s32 handle;
    u32 second_word;
    u32 high = arg1 >> OPERAND_FIELD_SHIFT;

    RESOLVE_OPERAND(first_ptr, arg0, high)
    first = *first_ptr;
    RESOLVE_OPERAND(second, arg0, arg1)
    second_word = *second;
    dst = *SP_OF(arg0);
    handle = *(s32 *)((u8 *)arg0 + OBJ_HANDLE_OFFSET);
    *SP_OF(arg0) = (s32 *)((u8 *)dst + WORD_SIZE);
    if (handle != 0) {
        *dst = func_800321D4(handle, second_word >> RESULT_SHIFT, first, 0, 0, 0, 0);
    } else {
        *dst = 0;
    }
}
