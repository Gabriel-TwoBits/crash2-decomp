#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F294 = 0x1F800040;
s32 D_8005F298 = 0;
s32 D_8005F29C[2] = { 0x3039, 0xBAEC2 };

#ifdef NON_MATCHING
#define FLAG_REG 0x800
#define FLAG_SEL 0x400
#define FLAG_MID 0x200
#define FLAG_HI 0x100
#define FLAG_LO 0x80
#define MASK_IDX9 0x1FF
#define MASK_IDX10 0x3FF
#define MASK_IDX6 0x3F
#define IDX_STACK 0x1F
#define CODE_NULL 0xBE0
#define CODE_ONE 0xBF0
#define TABLE_BASE 0x40
#define OFF_STACK_PTR 0xBC
#define OFF_BASE_PTR 0xC4
#define OFF_SRC_A 0x10
#define OFF_SRC_B 0x14
#define OFF_SRC_DATA 0x18
#define TABLE_SELECT_MASK 0x1C
#define TABLE_SELECT_SHIFT 4

s32 *func_80036E30(s32 arg0, s32 arg1, s32 arg2) {
    s32 src;
    s32 flip;
    s32 *slot;
    s32 base;
    s32 sp;

    if (arg1 & FLAG_REG) {
        if (arg1 & FLAG_SEL) {
            if (arg1 & FLAG_MID) {
                if ((arg1 & MASK_IDX9) == IDX_STACK) {
                    sp = *(s32 *)(arg0 + OFF_STACK_PTR);
                    if (arg2 == 0) {
                        sp -= 4;
                        *(s32 *)(arg0 + OFF_STACK_PTR) = sp;
                        return (s32 *)sp;
                    } else {
                        *(s32 *)(arg0 + OFF_STACK_PTR) = sp + 4;
                        return (s32 *)sp;
                    }
                } else {
                    return (s32 *)(arg0 + (arg1 & MASK_IDX9) * 4 + TABLE_BASE);
                }
            } else {
                base = *(s32 *)(arg0 + ((arg1 >> TABLE_SELECT_SHIFT) & TABLE_SELECT_MASK) + TABLE_BASE);
                if (base != 0) {
                    return (s32 *)(base + (arg1 & MASK_IDX6) * 4 + TABLE_BASE);
                } else {
                    return (s32 *)0;
                }
            }
        } else if (arg1 & FLAG_MID) {
            if (arg1 & FLAG_HI) {
                if (arg1 & FLAG_LO) {
                    if (arg1 == CODE_NULL) {
                        return (s32 *)0;
                    } else if (arg1 == CODE_ONE) {
                        return (s32 *)1;
                    } else {
                        return (s32 *)CODE_ONE;
                    }
                } else {
                    return (s32 *)(*(s32 *)(arg0 + OFF_BASE_PTR) + ((arg1 << 25) >> 23));
                }
            } else {
                flip = D_8005F298 == 0;
                slot = (s32 *)(D_8005F294 + flip * 4);
                D_8005F298 = flip;
                *slot = (arg1 << 24) >> 20;
                return slot;
            }
        } else {
            flip = D_8005F298 == 0;
            slot = (s32 *)(D_8005F294 + flip * 4);
            D_8005F298 = flip;
            *slot = (arg1 << 23) >> 15;
            return slot;
        }
    } else {
        if (arg1 & FLAG_SEL) {
            src = *(s32 *)(arg0 + OFF_SRC_B);
        } else {
            src = *(s32 *)(arg0 + OFF_SRC_A);
        }
        return (s32 *)(*(s32 *)(src + OFF_SRC_DATA) + (arg1 & MASK_IDX10) * 4);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80036E30", func_80036E30);
#endif

#ifdef NON_MATCHING
#define COND_SRC_SHIFT 12
#define COND_CMP_SHIFT 14
#define COND_ARG_SHIFT 16
#define COND_INVERT_BIT 0x100000
#define COND_INVERT_SHIFT 20
#define COND_2BIT_MASK 3
#define COND_ARG_MASK 0xF
#define COND_MASK_BITS 0xFFF
#define TABLE_INDEX_MASK 0xF000
#define TABLE_INDEX_SHIFT 12
#define ENTRY_WORDS 19 /* 0x4C-byte entries */
#define TABLE_STRIDE_WORDS 3 /* 0xC-byte entries */
#define ARG_FLAG_9 9
#define ARG_FLAG_10 10
#define ARG_FLAG_11 11
#define ARG_FLAG_12 12
#define FLAG_9_BIT 0x1000
#define FLAG_10_BIT 0x2000
#define FLAG_11_BIT 0x4000
#define FLAG_12_BIT 0x8000

extern u32 D_8005B440[];
extern u32 D_80069944[];
extern u32 D_80069948[];
extern u32 D_8006994C[];
extern u32 D_80069950[];

/* Value of the table entry selected by the top nibble of a word. */
#define TABLE_VALUE(word) \
    D_8005B440[(((word) & TABLE_INDEX_MASK) >> TABLE_INDEX_SHIFT) * TABLE_STRIDE_WORDS]

s32 func_80036FAC(u32 arg0, s32 arg1) {
    u32 *a;
    u32 *b;
    u32 *c;
    u32 *d;
    u32 masked;
    u32 arg;
    s32 res;
    u32 flag;

    a = &D_80069944[arg1 * ENTRY_WORDS];
    b = &D_80069948[arg1 * ENTRY_WORDS];
    c = &D_8006994C[arg1 * ENTRY_WORDS];
    d = &D_80069950[arg1 * ENTRY_WORDS];

    switch ((arg0 >> COND_SRC_SHIFT) & COND_2BIT_MASK) {
    case 1:
        masked = (*a & COND_MASK_BITS) & arg0;
        break;
    case 2:
        masked = (*b & COND_MASK_BITS) & arg0;
        break;
    case 3:
        masked = (arg0 & COND_MASK_BITS) & (*a | *d);
        break;
    default:
        masked = 1;
        break;
    }
    if (masked == 0) {
        return (arg0 >> COND_INVERT_SHIFT) & 1;
    }

    arg = (arg0 >> COND_ARG_SHIFT) & COND_ARG_MASK;
    switch ((arg0 >> COND_CMP_SHIFT) & COND_2BIT_MASK) {
    case 1:
    case 2:
        {
            u32 *src;
            src = (((arg0 >> COND_CMP_SHIFT) & COND_2BIT_MASK) == 1) ? a : b;
            if (arg == ARG_FLAG_9) {
                res = *src & FLAG_9_BIT;
            } else if (arg == ARG_FLAG_10) {
                res = *src & FLAG_10_BIT;
            } else if (arg == ARG_FLAG_11) {
                res = *src & FLAG_11_BIT;
            } else if (arg == ARG_FLAG_12) {
                res = *src & FLAG_12_BIT;
            } else if (src == a) {
                res = 0;
                if (arg == TABLE_VALUE(*b)) {
                    res = arg != TABLE_VALUE(*c);
                }
            } else {
                res = arg == TABLE_VALUE(*b);
            }
        }
        break;
    case 3:
        res = 0;
        if (arg == TABLE_VALUE(*b) || arg == TABLE_VALUE(*c)) {
            res = 1;
        }
        break;
    default:
        /* No comparison: the masked value itself is the result. */
        res = masked;
        break;
    }
    if (arg0 & COND_INVERT_BIT) {
        return res == 0;
    }
    return res;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80036E30", func_80036FAC);
#endif

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
