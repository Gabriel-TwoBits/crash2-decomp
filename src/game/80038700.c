#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
#define OP_DEST_MASK 0xFFF
#define OP_DEST_NONE 0xBE0
#define OP_DEST_GROUP_MASK 0xE00
#define OP_DEST_IMM_GROUP 0xE00
#define OP_DEST_PUSH 0xE1F
#define OP_DEST_IMM_INDEX_MASK 0x1FF
#define OBJ_REG_BASE 0x60
#define OBJ_IMM_BASE 0x40
#define VEC_SIZE 0xC
#define REG_MASK 7
#define OP_MODE_SHIFT 18
#define OP_SRC_SHIFT 12
#define OP_DST_SHIFT 15
#define OBJ_FLAG_BIT 1
#define OBJ_OFS_PARENT 0x8
#define OBJ_OFS_CTX 0x18
#define OBJ_OFS_OTHER 0x44
#define OBJ_OFS_RESULT 0xD4
#define OBJ_OFS_STACK 0xBC
#define OBJ_OFS_HALF 0x40
#define CTX_OFS_INNER 0x10
#define INNER_OFS_FLAGS 0x29C

extern s32 g_Player;
extern s32 g_CurrentZone;

#define W(base, ofs) (*(s32 *)((u8 *)(base) + (ofs)))

/* Destination of the secondary vector result. */
static inline s32 *dest_slot(s32 obj, u32 op) {
    u32 d = op & OP_DEST_MASK;

    if ((op & OP_DEST_GROUP_MASK) == OP_DEST_IMM_GROUP) {
        if (d == OP_DEST_PUSH) {
            s32 *p = (s32 *)W(obj, OBJ_OFS_STACK);
            W(obj, OBJ_OFS_STACK) = (s32)(p + 1);
            return p;
        } else {
            return (s32 *)(obj + OBJ_IMM_BASE + (op & OP_DEST_IMM_INDEX_MASK) * 4);
        }
    } else {
        return (s32 *)Gool_ResolveRef((GoolObject *)obj, d);
    }
}

void Gool_OpCollide(s32 arg0, u32 arg1) {
    s32 a[8];
    s32 b[8];
    s32 c[16];
    s32 *dst;
    s32 mode;
    s32 flag;
    s32 sel;
    s32 ctx;
    s32 *src2 = (s32 *)(arg0 + OBJ_REG_BASE + ((arg1 >> OP_SRC_SHIFT) & REG_MASK) * VEC_SIZE);
    s32 *out;
    extern void *func_80028750(s32 *, s32 *, s32);

    Gool_ResolveOperand(arg0, arg1 & OP_DEST_MASK, 0);
    mode = (arg1 >> OP_MODE_SHIFT) & REG_MASK;
    switch (mode) {
    case 0:
        a[0] = W(arg0, 0x60);
        a[1] = W(arg0, 0x64);
        a[2] = W(arg0, 0x68);
        b[0] = src2[0];
        b[1] = src2[1];
        b[2] = src2[2];
        a[1] = a[1] + W(W(arg0, OBJ_OFS_PARENT), OBJ_OFS_HALF) / 2;
        W(arg0, OBJ_OFS_RESULT) = (s32)func_80028750(a, b, a[1]);
        out = a;
        dst = b;
        break;
    case 1:
        a[0] = W(arg0, 0x60);
        a[1] = W(arg0, 0x64);
        a[2] = W(arg0, 0x68);
        b[0] = W(arg0, 0x60);
        b[1] = W(arg0, 0x64);
        b[2] = W(arg0, 0x68);
        if (arg0 == g_Player || W(arg0, OBJ_OFS_OTHER) == g_Player) {
            func_80028C48(arg0, a, g_Player);
        }
        W(arg0, OBJ_OFS_RESULT) = (s32)func_80028A28((void *)arg0, b);
        out = b;
        dst = &b[4];
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        flag = (mode == 5 || mode == 3);
        c[0] = W(arg0, 0x60);
        c[1] = W(arg0, 0x64);
        c[2] = W(arg0, 0x68);
        ctx = W(arg0, OBJ_OFS_CTX);
        if (ctx == 0) {
            ctx = g_CurrentZone;
        }
        if (W(W(ctx, CTX_OFS_INNER), INNER_OFS_FLAGS) & OBJ_FLAG_BIT) {
            sel = flag ? 6 : 2;
        } else {
            sel = flag ? 5 : 1;
        }
        func_80028FE0(arg0, c, sel, (u32)(mode - 4) >= 2U);
        out = c;
        dst = &c[4];
        break;
    case 6:
        func_800294B4((s16 *)arg0);
        return;
    default:
        return;
    }
    if ((arg1 & OP_DEST_MASK) != OP_DEST_NONE) {
        s32 *p = dest_slot(arg0, arg1);
        p[0] = dst[0];
        p[1] = dst[1];
        p[2] = dst[2];
    }
    {
        s32 *r = (s32 *)(arg0 + OBJ_REG_BASE + ((arg1 >> OP_DST_SHIFT) & REG_MASK) * VEC_SIZE);
        r[0] = out[0];
        r[1] = out[1];
        r[2] = out[2];
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038700", Gool_OpCollide);
#endif

#ifdef NON_MATCHING
void Gool_OpSpawn(GoolObject *arg0, u32 arg1) {
    s32 i;
    s32 count;
    s32 n;
    s32 *r;
    u32 hi;
    s32 k;

    count = arg1 & 0x3F;
    hi = arg1 >> 0x14;
    n = hi & 0xF;
    if (count == 0) {
        n += 1;
        count = *(arg0->sp - n);
    }
    i = 0;
    if (count > 0) {
        k = hi & 0xF;
        do {
            r = (s32 *)Obj_Spawn(arg0, (arg1 >> 0xC) & 0xFF, (arg1 >> 6) & 0x3F, k, (s32)(arg0->sp + k * -1), (arg1 >> 0x18) == 0x46);
            if ((s32)r < -0xFE) {
                r[0x50 / 4] = (s32)arg0;
                arg0->unk_D4 = (s32)r;
            } else {
                arg0->unk_D4 = 0;
            }
            i++;
        } while (i < count);
    }
    arg0->sp = arg0->sp - n;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038700", Gool_OpSpawn);
#endif

/* 0xE1F pops the stack, 0xE00-0xFFF name a register, anything else is resolved by Gool_ResolveRef. */
static inline s32 *operand_address(GoolObject *obj, s32 idx) {
    if ((idx & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            return --obj->sp;
        } else {
            return &((s32 *)obj->links)[idx & 0x1FF];
        }
    } else {
        return Gool_ResolveRef(obj, idx);
    }
}

s32 func_80032B7C(GoolObject *, s32 *, s32);

void Gool_OpPlaySound(GoolObject *obj, u32 arg1) {
    s32 *a;
    s32 *b;

    a = operand_address(obj, (arg1 >> 12) & 0xFFF);
    b = operand_address(obj, arg1 & 0xFFF);
    obj->unk_12C = func_80032B7C(obj, b, *(s16 *)a);
}

void Gool_OpSetAudio(GoolObject *obj, u32 op) {
    s32 *dst;
    s32 *src;
    s32 srcIdx;
    s32 val;
    s32 flags;

    dst = operand_address(obj, op & 0xFFF);
    flags = (op >> 20) & 0xF;
    srcIdx = (op >> 12) & 0x3F;
    if (srcIdx == 0) {
        val = 0;
    } else {
        if (srcIdx == 0x1F) {
            src = --obj->sp;
        } else {
            src = &((s32 *)obj->links)[srcIdx];
        }
        val = *src;
    }
    if (flags == 0xF) {
        flags = 0x80000000;
    }
    if (op & 0x40000) {
        flags |= 0x40000000;
    }
    if (op & 0x80000) {
        flags |= 0x20000000;
    }
    obj->unk_D4 = func_80032F18(val, flags, dst, obj);
}

void func_80038E14(GoolObject *obj, u32 arg1) {
    s32 *a;
    s32 *b;
    a = operand_address(obj, (arg1 >> 12) & 0xFFF);
    b = operand_address(obj, arg1 & 0xFFF);
    Cam_QueueMessage(*b, a);
}
