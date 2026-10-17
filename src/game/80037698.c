#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/80037698", D_80010804);

INCLUDE_ASM("asm/nonmatchings/game/80037698", func_80037698);

#ifdef NON_MATCHING
#define REG_FILE_OFFSET 0x60
#define REG_SIZE 0xC
#define SLOT_ARRAY_OFFSET 0x40
#define STACK_PTR_OFFSET 0xBC
#define STACK_SLOT_BYTES 4
#define SLOT_SELECT_MASK 0x1FF
#define FIELD_OP_MASK 0xFFF
#define OPERAND_KIND_MASK 0xE00
#define OPERAND_KIND_SLOT 0xE00
#define OPERAND_POP 0xE1F
#define FIXED_SHIFT 8
#define SIN_COS_SHIFT 4
#define NORMALIZE_SCALE 0x118
#define VERTEX_BYTES 3
#define ANIM_HEADER_BYTES 0x18
#define ANIM_FRAME_BYTES 0x28
#define ANIM_COORD_SHIFT 2
#define OP_STATE_FIRST 6
#define OP_STATE_COUNT 2
#define OFFSET_REG_A 0x6C
#define OFFSET_REG_B 0x78
#define OFFSET_REG_C 0x90

enum {
    OP_CALL_F054,
    OP_NORMALIZE,
    OP_SCALE_TRIG,
    OP_ANIM_A,
    OP_BLEND_A,
    OP_BLEND_B,
    OP_ANIM_B,
    OP_CLEAR
};

extern s32 D_80010804[];

s32 func_8001F054(struct Obj *, u32, s32 *);

#define F(o, off, T) (*(T *)((u8 *)(o) + (off)))
#define REG(o, i) ((s32 *)((u8 *)(o) + (i) * REG_SIZE + REG_FILE_OFFSET))

void func_8003817C(struct Obj *obj, u32 arg1) {
    u8 *o = (u8 *)obj;
    s32 *dst = REG(o, (arg1 >> 12) & 7);
    s32 *src;
    s32 sp18[3];
    s32 sp28[3];
    s32 sp38[3];
    s32 sp48[3];
    u32 op = (arg1 >> 18) & 7;
    u8 *other;
    u8 *anim;
    u8 *anim2;
    u8 *entry;
    s32 *reg2;
    s32 div;
    s32 *a2p;
    s32 *a3p;

    if ((arg1 & OPERAND_KIND_MASK) == OPERAND_KIND_SLOT) {
        if ((arg1 & FIELD_OP_MASK) == OPERAND_POP) {
            src = (s32 *)(F(o, STACK_PTR_OFFSET, u8 *) - STACK_SLOT_BYTES);
            F(o, STACK_PTR_OFFSET, s32 *) = src;
        } else {
            src = (s32 *)(o + (arg1 & SLOT_SELECT_MASK) * 4 + SLOT_ARRAY_OFFSET);
        }
    } else {
        src = func_80036D0C(obj, arg1 & FIELD_OP_MASK);
    }

    switch (op) {
    case OP_CALL_F054:
        if (src != 0 && F(o, 0xF0, s32) != 0) {
            sp18[0] = dst[0];
            sp18[1] = dst[1];
            sp18[2] = dst[2];
            func_8001F054(obj, (u32)src[0], sp18);
            dst[0] = sp18[0];
            dst[1] = sp18[1];
            dst[2] = sp18[2];
            F(o, 0xFC, s32) = dst[1];
        }
        return;
    case OP_SCALE_TRIG:
        dst[0] = ((func_8003D2EC(F(o, 0x94, s32) & FIELD_OP_MASK) >> SIN_COS_SHIFT) * src[0]) >> FIXED_SHIFT;
        if ((u32)(F(o, 0x120, u8) - OP_STATE_FIRST) >= OP_STATE_COUNT) {
            dst[2] = ((func_8003D360(F(o, 0x94, s32) & FIELD_OP_MASK) >> SIN_COS_SHIFT) * src[0]) >> FIXED_SHIFT;
        } else {
            dst[1] = ((func_8003D360(F(o, 0x94, s32) & FIELD_OP_MASK) >> SIN_COS_SHIFT) * src[0]) >> FIXED_SHIFT;
        }
        F(o, 0x104, s32) = src[0];
        return;
    case OP_BLEND_A:
    case OP_BLEND_B:
        sp28[2] = src[0];
        F(o, STACK_PTR_OFFSET, u8 *) -= STACK_SLOT_BYTES;
        sp28[1] = F(F(o, STACK_PTR_OFFSET, u8 *), 0, s32);
        F(o, STACK_PTR_OFFSET, u8 *) -= STACK_SLOT_BYTES;
        sp28[0] = F(F(o, STACK_PTR_OFFSET, u8 *), 0, s32);
        if (op == OP_BLEND_A) {
            a2p = (s32 *)(o + OFFSET_REG_A);
            a3p = (s32 *)(o + OFFSET_REG_B);
        } else {
            a2p = (s32 *)(o + OFFSET_REG_C);
            a3p = 0;
        }
        func_8001E8E4(sp28, dst, a2p, (s32)a3p, REG(o, (arg1 >> 15) & 7));
        return;
    case OP_ANIM_A:
    case OP_ANIM_B:
        other = F(o, ((arg1 >> 19) & 0x1C) + SLOT_ARRAY_OFFSET, u8 *);
        if (F(other, 0xE8, u8 *) != 0 && *F(other, 0xE8, u8 *) == 1) {
            anim = F(F(other, 8, u8 *), 0x20, u8 *);
            anim2 = F(F(other, 8, u8 *), 0x24, u8 *);
            entry = anim + F(anim, 0xC, s32) * ANIM_FRAME_BYTES + ANIM_HEADER_BYTES + ((u32)src[0] >> FIXED_SHIFT) * VERTEX_BYTES;
            sp38[0] = (F(anim, 0, s16) + (entry[0] << ANIM_COORD_SHIFT)) << FIXED_SHIFT;
            sp38[1] = (F(anim, 2, s16) + (entry[2] << ANIM_COORD_SHIFT)) << FIXED_SHIFT;
            sp38[2] = (F(anim, 4, s32) + (entry[1] << ANIM_COORD_SHIFT)) << FIXED_SHIFT;
            if (anim != anim2) {
                entry = anim2 + F(anim2, 0xC, s32) * ANIM_FRAME_BYTES + ANIM_HEADER_BYTES + ((u32)src[0] >> FIXED_SHIFT) * VERTEX_BYTES;
                sp38[0] += (F(anim2, 0, s16) + (entry[0] << ANIM_COORD_SHIFT)) << FIXED_SHIFT;
                sp38[1] += (F(anim2, 2, s16) + (entry[2] << ANIM_COORD_SHIFT)) << FIXED_SHIFT;
                sp38[0] >>= 1;
                sp38[1] >>= 1;
                sp38[2] += (F(anim2, 4, s32) + (entry[1] << ANIM_COORD_SHIFT)) << FIXED_SHIFT;
                sp38[2] >>= 1;
            }
            if (op == OP_ANIM_A) {
                sp48[0] = D_80010804[0];
                sp48[1] = D_80010804[1];
                sp48[2] = D_80010804[2];
                func_8001E8E4(sp38, (s32 *)(other + REG_FILE_OFFSET), sp48,
                              F(F(F(other, 8, u8 *), 0x28, u8 *), 0x10, s32), dst);
            } else {
                func_8001E9EC(other, sp38, dst);
            }
        }
        return;
    case OP_NORMALIZE:
        reg2 = REG(o, (arg1 >> 15) & 7);
        func_8001ED60(dst, reg2);
        if (src[0] != 0) {
            div = reg2[2] >> FIXED_SHIFT;
            if (div != 0) {
                src[0] = (src[0] * NORMALIZE_SCALE) / div;
                src[1] = (src[1] * NORMALIZE_SCALE) / div;
                src[2] = (src[2] * NORMALIZE_SCALE) / div;
            }
        }
        return;
    case OP_CLEAR:
        func_8001EC2C(dst, REG(o, (arg1 >> 15) & 7), 0);
        return;
    default:
        return;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80037698", func_8003817C);
#endif
