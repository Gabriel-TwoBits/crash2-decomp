#include "common.h"
#include "sdk.h"
#include "game.h"

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

void func_80028C48(s32, s32 *, s32);
void func_80028FE0(s32, s32 *, s32, s32);
extern s32 D_8005F38C;
extern s32 D_800608CC;

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
        return (s32 *)func_80036D0C((struct Obj *)obj, d);
    }
}

void func_80038700(s32 arg0, u32 arg1) {
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

    func_80036E30(arg0, arg1 & OP_DEST_MASK, 0);
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
        if (arg0 == D_8005F38C || W(arg0, OBJ_OFS_OTHER) == D_8005F38C) {
            func_80028C48(arg0, a, D_8005F38C);
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
            ctx = D_800608CC;
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
