#include "common.h"
#include "sdk.h"
#include "game.h"

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
