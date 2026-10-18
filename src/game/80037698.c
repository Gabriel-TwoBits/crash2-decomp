#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/80037698", D_80010804);

#ifdef NON_MATCHING
#define OBJ_SLOTS 0x40
#define OBJ_POS 0x60
#define OBJ_ANGLE 0x70
#define OBJ_VEC 0x78
#define OBJ_STACK 0xBC
#define OBJ_RESULT 0xD4
#define OBJ_TARGET 0xF0
#define OBJ_KIND 0x120
#define OBJ_KIND_FIRST 6
#define OBJ_KIND_COUNT 2

#define REG_SPACE_MASK 0xE00
#define REG_STACK_POP 0xE1F
#define REG_INDEX_MASK 0x1FF
#define REG_ID_MASK 0xFFF
#define OP_SHIFT 20
#define OP_MASK 0xF
#define ARG_SHIFT 15
#define ARG_MASK 0x1F
#define ARG_SIGN_LSHIFT 12
#define ARG_SIGN_RSHIFT 27
#define SLOT_SHIFT 10
#define SLOT_MASK 0x1C
#define VALUE_SHIFT 8
#define ANGLE_MASK 0xFFF
#define ANGLE_HALF 0x800
#define ENTRY_STRIDE 8
#define PROP_ACTIVE 1
#define PROP_HIDDEN 2
#define PROP_B 4
#define PROP_C 8
#define MISSING_SENTINEL 0x7FFFFFFF
#define MIN_VALID (-0xFE)
#define SCRIPT_HANDLE_MASK 0xFF800003
#define SCRIPT_HANDLE_TAG 0x80000000
#define SCRIPT_HANDLE_FLAGS 0x17
#define CHAR_UPPER_S 0x53
#define CHAR_LOWER_S 0x73
#define SPAWN_MSG 0x287
#define SPAWN_COUNT_MAX 2
#define EVENT_ACTIVE 0x300
#define EVENT_IDLE 0x4800
#define SPAWN_SIZE 0x1060
#define WORD_BYTES 4
#define REC_OFFSET_WORDS 2
#define ADDR_NONE (-2)
#define VEC_STRIDE_WORDS 4
#define QUERY_WORDS 5
#define QUERY_KIND 0
#define QUERY_SLOT 1
#define QUERY_RESULT 2
#define QUERY_BEST 3
#define QUERY_VALUE 4

#define FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
#define SLOT(obj, arg1) FIELD(obj, (((arg1) >> SLOT_SHIFT) & SLOT_MASK) + OBJ_SLOTS, u8 *)
#define PROP(dst) D_8006B1C4[(u32)*(dst) >> VALUE_SHIFT]

extern u32 g_TimerTicks;
extern s32 D_8005F38C;
extern u32 D_8005F3F0;
extern s32 D_800608CC;
extern s32 D_800608D0;
extern u8 D_80060958[];
extern s8 D_80069920;
extern s8 D_80069921;
extern u8 D_80069926;
extern u8 D_80069927;
extern u32 D_8006B1C4[];
extern u8 D_8006CB18[];
extern u8 D_8006CB30[];
extern u8 D_8006CB38[];
extern u8 func_80018E48[];

/* Appends to the object's value stack; a macro so no extra call is emitted. */
#define PUSH(value) \
    (pushValue = (value), pushSlot = FIELD(obj, OBJ_STACK, s32 *), FIELD(obj, OBJ_STACK, s32 *) = pushSlot + 1, *pushSlot = pushValue)

void func_80037698(u8 *obj, u32 arg1) {
    s32 *dst;
    s32 sub;
    u32 op;
    s32 v;
    s32 i;
    s32 vec[2][VEC_STRIDE_WORDS];
    s32 sp48;
    s32 sp4C;
    s32 sp50[3];
    s32 query[QUERY_WORDS];
    s16 *list;
    u32 *flags;
    u8 *slot;
    s32 *rec;
    s32 *a;
    s32 *b;
    s32 *sp;
    s32 *pushSlot;
    s32 pushValue;

    if ((arg1 & REG_SPACE_MASK) == REG_SPACE_MASK) {
        if ((arg1 & REG_ID_MASK) == REG_STACK_POP) {
            dst = FIELD(obj, OBJ_STACK, s32 *) - 1;
            FIELD(obj, OBJ_STACK, s32 *) = dst;
        } else {
            dst = (s32 *)(obj + (arg1 & REG_INDEX_MASK) * WORD_BYTES + OBJ_SLOTS);
        }
    } else {
        dst = func_80036D0C((void *)obj, arg1 & REG_ID_MASK);
    }
    sub = (s32)(arg1 << ARG_SIGN_LSHIFT) >> ARG_SIGN_RSHIFT;
    op = (arg1 >> OP_SHIFT) & OP_MASK;

    switch (op) {
    case 1:
    case 6:
        a = (s32 *)(obj + OBJ_POS);
        if (op == 6) {
            a = dst;
        }
        b = (s32 *)(SLOT(obj, arg1) + OBJ_POS);
        if (sub & 2) {
            vec[0][0] = a[0];
            vec[0][1] = 0;
            vec[0][2] = a[2];
            vec[1][0] = b[0];
            vec[1][1] = 0;
            vec[1][2] = b[2];
            a = vec[0];
            b = vec[1];
        }
        if (sub & 1) {
            v = func_8003118C((void *)a, (void *)b);
        } else {
            v = func_80030FEC(a, b);
        }
        PUSH(v);
        return;
    case 5:
        v = func_80031264((s32 *)(obj + OBJ_POS), (s32 *)(SLOT(obj, arg1) + OBJ_POS)) - (FIELD(obj, OBJ_ANGLE, s32) & ANGLE_MASK);
        i = v;
        if (i < 0) {
            i = -i;
        }
        if (i >= ANGLE_HALF) {
            v = -v & ANGLE_MASK;
        }
        PUSH(v);
        return;
    case 2:
        if ((u32)(FIELD(obj, OBJ_KIND, u8) - OBJ_KIND_FIRST) >= OBJ_KIND_COUNT) {
            v = func_80031264((s32 *)(SLOT(obj, arg1) + OBJ_POS), dst);
        } else {
            v = Math_AngleTo((s32 *)(SLOT(obj, arg1) + OBJ_POS), dst);
        }
        PUSH(v);
        return;
    case 3:
        PUSH(FIELD(SLOT(obj, arg1), ((u32)*dst >> VALUE_SHIFT) * WORD_BYTES + OBJ_SLOTS, s32));
        return;
    case 4:
        sp = FIELD(obj, OBJ_STACK, s32 *);
        FIELD(obj, OBJ_STACK, s32 *) = sp - 1;
        FIELD(SLOT(obj, arg1), ((u32)*dst >> VALUE_SHIFT) * WORD_BYTES + OBJ_SLOTS, s32) = sp[-1];
        return;
    case 7:
        if (dst == 0) {
            return;
        }
        if (*dst == 0 || !(PROP(dst) & PROP_ACTIVE)) {
            PUSH(0);
            return;
        }
        v = func_80018BE4(D_8006CB30, func_80018E48, *dst);
        if (v == 0) {
            v = func_80018BE4(D_8006CB30 + ENTRY_STRIDE, func_80018E48, *dst);
        }
        PUSH(v);
        return;
    case 8:
        if (dst == 0) {
            return;
        }
        if (sub != 0) {
            PROP(dst) &= ~PROP_HIDDEN;
        } else {
            PROP(dst) |= PROP_HIDDEN;
        }
        return;
    case 10:
        if (dst == 0) {
            return;
        }
        switch (sub) {
        case 0:
            PROP(dst) &= ~PROP_B;
            return;
        case 1:
            PROP(dst) |= PROP_B;
            return;
        case 4:
            func_8001F94C(*dst);
            PROP(dst) &= ~PROP_C;
            return;
        case 2:
            PROP(dst) &= ~PROP_C;
            return;
        case 5:
            func_8001F8B4(*dst);
            PROP(dst) |= PROP_C;
            return;
        case 3:
            PROP(dst) |= PROP_C;
            return;
        case 9:
            PROP(dst) |= PROP_ACTIVE;
            return;
        case 8:
            PROP(dst) &= ~PROP_ACTIVE;
            return;
        default:
            return;
        }
    case 11:
        switch (sub) {
        case 1:
            PUSH(0);
            if (dst != 0) {
                FIELD(obj, OBJ_STACK, s32 *)[-1] = ((PROP(dst) >> 1) ^ 1) & 1;
            }
            return;
        case 2:
            PUSH(0);
            if (dst != 0) {
                FIELD(obj, OBJ_STACK, s32 *)[-1] = PROP(dst) & PROP_B;
            }
            return;
        case 3:
            PUSH(0);
            if (dst != 0) {
                FIELD(obj, OBJ_STACK, s32 *)[-1] = PROP(dst) & PROP_C;
            }
            return;
        default:
            return;
        }
    case 9:
        if (dst == 0) {
            v = D_800608CC;
        } else {
            v = (s32)func_80027DEC((char *)D_800608CC, dst);
        }
        if (v < MIN_VALID) {
            FIELD(SLOT(obj, arg1), 0x18, s32) = v;
        }
        return;
    case 12:
        switch ((arg1 >> ARG_SHIFT) & ARG_MASK) {
        case 0:
            func_8002F34C((GoolObject *)obj, D_80060958 + FIELD(dst, 1, u8) * SPAWN_SIZE, 0, FIELD(dst, 2, u16));
            return;
        case 1:
            func_8002F6C8(D_80060958 + ((u32)*dst >> VALUE_SHIFT) * SPAWN_SIZE);
            return;
        case 2:
            func_800189CC((void *)obj, D_8006CB18 + ((u32)*dst >> VALUE_SHIFT) * ENTRY_STRIDE);
            return;
        case 3:
            func_800324A0(*dst);
            return;
        case 4:
            FIELD(*dst, 0x18, s32) = D_800608D0;
            return;
        case 5:
            func_80033DD0();
            return;
        case 6:
            func_80033DE0(*dst);
            return;
        case 7:
            func_80019E5C();
            return;
        case 8:
            slot = SLOT(obj, arg1);
            PUSH(ratan2(FIELD(slot, 0x64, s32) - dst[1], func_80031208((s32 *)(slot + OBJ_POS), dst)));
            return;
        case 9:
            D_8005F3F0 = (u32)*dst >> VALUE_SHIFT;
            return;
        case 10:
            Nsf_SeekLevel((u32)*dst >> VALUE_SHIFT);
            return;
        case 11:
            func_8002F0DC(1);
            return;
        case 12:
            *dst = g_TimerTicks;
            return;
        case 13:
            g_TimerTicks = *dst;
            return;
        case 14:
            FIELD(SLOT(obj, arg1), 0x14, s32) = *dst;
            return;
        case 15:
            PUSH(FIELD(*dst, 0x14, s32));
            return;
        case 16:
            FIELD(SLOT(obj, arg1), 0x10, s32) = *dst;
            return;
        case 17:
            PUSH(FIELD(*dst, 0x10, s32));
            return;
        case 18:
            sp4C = 1;
            list = (s16 *)func_80031AE8(FIELD(obj, OBJ_TARGET, s32), SPAWN_MSG, 0, 0, &sp48, SPAWN_COUNT_MAX, 0, 0, 0);
            if (list == 0) {
                return;
            }
            for (i = 0; i < sp48; i++) {
                flags = &D_8006B1C4[list[i]];
                if (*flags & PROP_ACTIVE) {
                    v = func_80019FA0(list[i] << VALUE_SHIFT);
                    if (v != 0) {
                        func_8001CC10((s32)obj, v, EVENT_ACTIVE, 1, &sp4C);
                    }
                } else if (!(*flags & PROP_HIDDEN)) {
                    func_8001CC10((s32)obj, D_8005F38C, EVENT_IDLE, 0, 0);
                    *flags |= PROP_HIDDEN;
                }
            }
            return;
        case 20:
            v = 0;
            if (D_80069920 == 0) {
                v = D_80069921;
                if ((v == CHAR_UPPER_S || v == CHAR_LOWER_S) && dst != 0) {
                    dst[0] = D_80069926;
                    dst[1] = D_80069927;
                }
            }
            PUSH(v);
            return;
        case 21:
            func_8001A4A4((void *)obj, (s32 *)(obj + OBJ_VEC));
            func_8001AA80((GoolObject *)obj, sp50);
            return;
        default:
            return;
        }
    case 13:
        query[QUERY_KIND] = (arg1 >> ARG_SHIFT) & ARG_MASK;
        query[QUERY_BEST] = MISSING_SENTINEL;
        query[QUERY_RESULT] = 0;
        query[QUERY_SLOT] = (s32)SLOT(obj, arg1);
        query[QUERY_VALUE] = *dst;
        func_80018BE4(D_8006CB38, (void *)func_80018E68, query);
        if ((arg1 >> ARG_SHIFT) & SCRIPT_HANDLE_FLAGS) {
            func_80018BE4(D_8006CB38 - ENTRY_STRIDE, (void *)func_80018E68, query);
        }
        PUSH(query[QUERY_RESULT]);
        return;
    case 14:
        v = (s32)SLOT(obj, arg1);
        if (((u32)v & SCRIPT_HANDLE_MASK) != SCRIPT_HANDLE_TAG) {
            PUSH(0);
            return;
        }
        rec = func_8002C324((void *)v, 0, ADDR_NONE);
        if (rec == 0) {
            PUSH(0);
            return;
        }
        sp = FIELD(obj, OBJ_STACK, s32 *);
        PUSH(func_80030D98((void *)dst, (void *)(rec + REC_OFFSET_WORDS)));
        func_8002CA1C(sp);
        return;
    case 15:
        FIELD(obj, OBJ_RESULT, s32) = func_8003670C((arg1 >> ARG_SHIFT) & ARG_MASK, *dst);
        return;
    default:
        return;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80037698", func_80037698);
#endif

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

s32 func_8001F054(GoolObject *, u32, s32 *);

#define F(o, off, T) (*(T *)((u8 *)(o) + (off)))
#define REG(o, i) ((s32 *)((u8 *)(o) + (i) * REG_SIZE + REG_FILE_OFFSET))

void func_8003817C(GoolObject *obj, u32 arg1) {
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
        dst[0] = ((Math_Sin(F(o, 0x94, s32) & FIELD_OP_MASK) >> SIN_COS_SHIFT) * src[0]) >> FIXED_SHIFT;
        if ((u32)(F(o, 0x120, u8) - OP_STATE_FIRST) >= OP_STATE_COUNT) {
            dst[2] = ((Math_Cos(F(o, 0x94, s32) & FIELD_OP_MASK) >> SIN_COS_SHIFT) * src[0]) >> FIXED_SHIFT;
        } else {
            dst[1] = ((Math_Cos(F(o, 0x94, s32) & FIELD_OP_MASK) >> SIN_COS_SHIFT) * src[0]) >> FIXED_SHIFT;
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
