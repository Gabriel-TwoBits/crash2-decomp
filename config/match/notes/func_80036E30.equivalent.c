#include "common.h"
#include "sdk.h"
#include "game.h"

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
