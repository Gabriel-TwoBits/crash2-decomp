#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F294 = 0x1F800040;
s32 D_8005F298 = 0;
s32 D_8005F29C[2] = { 0x3039, 0xBAEC2 };

INCLUDE_ASM("asm/nonmatchings/game/80036E30", func_80036E30);

INCLUDE_ASM("asm/nonmatchings/game/80036E30", func_80036FAC);

void func_80037440(void) {
}

#ifdef NON_MATCHING
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
#else
INCLUDE_ASM("asm/nonmatchings/game/80036E30", func_80037448);
#endif

#ifdef NON_MATCHING
inline static s32 *Pop(u8 *o, s32 reg)
{
  if ((reg & 0xE00) == 0xE00)
  {
    if (reg == 0xE1F)
    {
      s32 *p = (*((s32 **) (o + 0xBC))) - 1;
      *((s32 **) (o + 0xBC)) = p;
      return p;
    }
    return (s32 *) (o + (((reg & 0x1FF) * 4) + 0x40));
  }
  return func_80036D0C((struct Obj *) o, reg);
}

inline static s32 *Push(u8 *o, s32 reg)
{
  s32 *p;
  if ((reg & 0xE00) == 0xE00)
  {
    if (reg == 0xE1F)
    {
      p = *((s32 **) (o + 0xBC));
      *((s32 **) (o + 0xBC)) = p + 1;
      return p;
    }
    return (s32 *) (o + (((reg & 0x1FF) * 4) + 0x40));
  }
  return func_80036D0C((struct Obj *) o, reg);
}

void func_8003756C(u8 *o, u32 arg1)
{
  int new_var;
  s32 *dst;
  s32 src;
  src = *Pop(o, (arg1 >> 12) & 0xFFF);
  dst = Push(o, arg1 & 0xFFF);
  if (dst != 0)
  {
    do { new_var = (*((s32 *) (o + 0xF0))) != 0; } while (0);
    if (new_var)
    {
      *dst = func_800321D4(*((s32 *) (o + 0xF0)), ((unsigned int) ((u32) src)) >> 8, 0, 0, 0, 2, 0);
      return;
    }
    *dst = 0;
  }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80036E30", func_8003756C);
#endif
