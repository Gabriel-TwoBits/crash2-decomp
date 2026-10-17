#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
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
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80038F00);
#endif

#ifdef NON_MATCHING
s32 func_8003900C(char *arg0, u32 arg1, s32 arg2) {
    s32 args[16];
    s32 n;
    s32 i;
    s32 *dst;
    s32 mask;
    s32 state;
    s32 flags;
    s32 hit;
    s32 r;
    s32 *sp;

    n = (arg1 >> 10) & 0xF;
    i = n - 1;
    if (i >= 0) {
        dst = args + i;
        do {
            sp = *(s32 **)(arg0 + 0xBC);
            *(s32 **)(arg0 + 0xBC) = sp - 1;
            i--;
            *dst = sp[-1];
            dst--;
        } while (i >= 0);
    }
    mask = *(s32 *)(arg0 + 0xB0);
    state = *(s32 *)(arg0 + 0x108);
    flags = *(s32 *)(*(char **)(*(char **)(arg0 + 0xC) + 0x20) + (arg1 & 0x3FF) * 16);
    if ((u32)(state - 3) >= 2U) {
        hit = mask & flags;
        if (state == 2) {
            goto b;
        }
    } else {
b:
        hit = (mask | 0x1002) & flags;
    }
    if (hit == 0) {
        r = func_8001C340(arg0, arg1 & 0x3FF, n, args);
        if (r < -0xFE && (arg2 & 2)) {
            return r;
        }
        *(char **)(arg0 + 0xBC) -= 4;
    } else {
        return 0;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_8003900C);
#endif

extern s32 D_8006CB64;

typedef struct Unk80039104Src {
    char pad0[0x24];
    u8 *unk24;
} Unk80039104Src;

typedef struct Unk80039104 {
    char pad0[0xC];
    Unk80039104Src *unkC;
    char pad10[0x68];
    s32 unk78;
    char pad7C[0x40];
    s32 *unkBC;
    char padC0[0x28];
    u8 *unkE8;
    s32 unkEC;
} Unk80039104;

s32 func_80039104(Unk80039104 *arg0, u32 arg1, s32 arg2) {
    u8 *q;
    s32 *p;
    s32 t;

    q = arg0->unkC->unk24;
    p = arg0->unkBC;
    arg0->unkEC = (arg1 & 0x7F) << 8;
    arg0->unkBC = p + 1;
    arg0->unkE8 = q + ((arg1 >> 5) & 0x7FC);
    t = D_8006CB64;
    *p = (((arg1 >> 16) & 0x3F) << 24) | t;
    switch ((arg1 >> 22) & 3) {
    case 0:
        if (arg0->unk78 < 0) {
            arg0->unk78 = -arg0->unk78;
        }
        break;
    case 1:
        if (arg0->unk78 > 0) {
            arg0->unk78 = -arg0->unk78;
        }
        break;
    case 2:
        arg0->unk78 = -arg0->unk78;
        break;
    default:
        break;
    }
    return -((arg2 & 4) != 0) & ~0xFE;
}

typedef struct Unk800391D8 {
    char pad0[0x78];
    s32 unk78;
    char pad7C[0x40];
    s32 *unkBC;
    char padC0[0x2C];
    s32 unkEC;
} Unk800391D8;

s32 func_800391D8(Unk800391D8 *arg0, u32 arg1, s32 arg2) {
    s32 idx = arg1 & 0xFFF;
    s32 idx2 = idx;
    s32 *var_v0;
    s32 *sp;
    s32 t;
    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            var_v0 = arg0->unkBC - 1;
            arg0->unkBC = var_v0;
        } else {
            var_v0 = (s32 *)((u8 *)arg0 + ((arg1 & 0x1FF) * 4 + 0x40));
        }
    } else {
        var_v0 = func_80036D0C((struct Obj *)arg0, idx2);
    }
    sp = arg0->unkBC;
    arg0->unkEC = *var_v0;
    arg0->unkBC = sp + 1;
    /* The do/while (0) wrapper keeps the D_8006CB64 load after the stores to 0xEC/0xBC. */
    do {
        t =D_8006CB64;
        *sp = (((arg1 >> 16) & 0x3F) << 24) | t;
        switch ((arg1 >> 22) & 3) {
        case 0:
            if (arg0->unk78 < 0) {
                arg0->unk78 = -arg0->unk78;
            }
            break;
        case 1:
            if (arg0->unk78 > 0) {
                arg0->unk78 = -arg0->unk78;
            }
            break;
        case 2:
            arg0->unk78 = -arg0->unk78;
            break;
        default:
            break;
        }
        return -((arg2 & 4) != 0) & ~0xFE;
    } while (0);
}

#ifdef NON_MATCHING
struct Unk80039310 {
    u8 pad0[0x40];
    s32 regs[32];
    u8 padC0[0xA8 - 0xC0 + 0x18];
};

s32 func_80039310(u8 *o, u32 arg1, s32 arg2) {
    s32 args[16];
    s32 *src;
    s32 *p;
    s32 *q;
    s32 a;
    s32 n;
    s32 i;
    s32 sel;
    s32 idx;
    s32 first;

    first = *(s32 *)(o + 0x40 + ((arg1 >> 19) & 0x1C));
    idx = arg1 & 0xFFF;
    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            src = *(s32 **)(o + 0xBC) - 1;
            *(s32 **)(o + 0xBC) = src;
        } else {
            src = (s32 *)(o + 0x40 + (arg1 & 0x1FF) * 4);
        }
    } else {
        src = func_80036D0C((struct Obj *)o, idx);
    }
    *(u32 *)(o + 0xA8) &= 0xFFFDFFFF;
    sel = (arg1 >> 12) & 0x3F;
    if (sel == 0x1F) {
        q = *(s32 **)(o + 0xBC) - 1;
        *(s32 **)(o + 0xBC) = q;
    } else {
        q = (s32 *)(o + 0x40 + sel * 4);
    }
    if (*q != 0 && first != 0 && src != 0) {
        n = (arg1 >> 18) & 7;
        if (n != 0) {
            p = args;
            for (i = 0; i < n; i++) {
                *p++ = (*(s32 **)(o + 0xBC))[i - n];
            }
        }
        func_8001CC10((s32)o, first, *src, n, args);
    } else {
        *(s32 *)(o + 0xD4) = 0;
    }
    if (!(*(u32 *)(o + 0xA8) & 0x20000)) {
        *(s32 **)(o + 0xBC) = (s32 *)((u8 *)*(s32 **)(o + 0xBC) - ((arg1 >> 16) & 0x1C));
        return 0;
    }
    if (!(arg2 & 2)) {
        *(s32 **)(o + 0xBC) = *(s32 **)(o + 0xBC) - 1;
        return 0;
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039310);
#endif

#ifdef NON_MATCHING
typedef struct Unk800394C4 {
    u8 pad0[0xA8];
    s32 flags;
    u8 padAC[0x10];
    s32 *sp;
    u8 padC0[0x14];
    s32 d4;
} Unk800394C4;

#define R(o, i) (((s32 *)(o))[0x10 + (i)])

s32 func_800394C4(Unk800394C4 *o, u32 ins, s32 arg2) {
    s32 args[18];
    s32 *src;
    s32 *chk;
    s32 *dst;
    s32 fn;
    s32 n;
    s32 i;
    s32 idx;
    s32 temp_a0;

    fn = R(o, (ins >> 21) & 7);
    temp_a0 = ins & 0xFFF;
    if ((ins & 0xE00) == 0xE00) {
        if (temp_a0 == 0xE1F) {
            src = o->sp - 1;
            o->sp = src;
        } else {
            src = &R(o, ins & 0x1FF);
        }
    } else {
        src = func_80036D0C((struct Obj *)o, temp_a0);
    }
    o->flags = o->flags & 0xFFFDFFFF;
    idx = (ins >> 12) & 0x3F;
    if (idx == 0x1F) {
        chk = o->sp - 1;
        o->sp = chk;
    } else {
        chk = &R(o, idx);
    }
    if (*chk != 0 && fn != 0 && src != 0) {
        n = (ins >> 18) & 7;
        i = 0;
        if (n != 0) {
            dst = args;
            do {
                *dst = o->sp[i - n];
                i++;
                dst++;
            } while (i < n);
        }
        func_8001D1E0((s32)o, fn, *src, (ins >> 21) & 7, n, (s32)args);
    } else {
        o->d4 = 0;
    }
    if (!(o->flags & 0x20000)) {
        o->sp = (s32 *)((u8 *)o->sp - ((ins >> 16) & 0x1C));
        goto ret0;
    }
    i = -0xFF;
    if (!(arg2 & 2)) {
        o->sp = o->sp - 1;
ret0:
        i = 0;
    }
    return i;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_800394C4);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039684);

INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_8003981C);

void func_80039A14(struct Obj *arg0, s32 arg1) {
    u8 *base = (u8 *)arg0;
    s32 idx = arg1 & 0xFFF;
    s32 idx2 = idx;
    s32 *var_v0;
    s32 temp_v0;
    s32 *sp;

    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            var_v0 = *(s32 **)(base + 0xBC) - 1;
            *(s32 **)(base + 0xBC) = var_v0;
        } else {
            var_v0 = (s32 *)(base + ((arg1 & 0x1FF) * 4 + 0x40));
        }
    } else {
        var_v0 = func_80036D0C(arg0, idx2);
    }
    temp_v0 = func_80027664(var_v0);
    sp = *(s32 **)(base + 0xBC);
    *(s32 **)(base + 0xBC) = sp + 1;
    *sp = temp_v0;
}

void func_80039AA4(struct Obj *arg0, s32 arg1) {
    s32 *var_v0;
    s32 temp_a2;
    s32 a3;

    temp_a2 = arg1 & 0xFFF;
    a3 = temp_a2;
    if ((arg1 & 0xE00) == 0xE00) {
        if (temp_a2 == 0xE1F) {
            var_v0 = *(s32 **)((u8 *)arg0 + 0xBC) - 1;
            *(s32 **)((u8 *)arg0 + 0xBC) = var_v0;
        } else {
            var_v0 = (s32 *)((u8 *)arg0 + (((arg1 & 0x1FF) * 4) + 0x40));
        }
    } else {
        var_v0 = func_80036D0C(arg0, a3);
    }
    func_800278F8(*var_v0);
}

#ifdef NON_MATCHING
void func_80039B14(struct Obj *arg0, u32 arg1) {
    s32 *var_s1;
    s32 *var_v0;
    s32 temp_a0_2;
    s32 temp_a1;
    u32 temp_a0;

    temp_a0 = arg1 >> 12;
    temp_a1 = temp_a0 & 0xFFF;
    if ((temp_a0 & 0xE00) == 0xE00) {
        if (temp_a1 == 0xE1F) {
            var_s1 = *(s32 **)((char *)arg0 + 0xBC) - 1;
            *(s32 **)((char *)arg0 + 0xBC) = var_s1;
        } else {
            var_s1 = (s32 *)((char *)arg0 + ((temp_a0 & 0x1FF) * 4) + 0x40);
        }
    } else {
        var_s1 = func_80036D0C(arg0, temp_a1);
    }
    temp_a0_2 = arg1 & 0xFFF;
    if ((arg1 & 0xE00) == 0xE00) {
        if (temp_a0_2 == 0xE1F) {
            var_v0 = *(s32 **)((char *)arg0 + 0xBC) - 1;
            *(s32 **)((char *)arg0 + 0xBC) = var_v0;
        } else {
            var_v0 = (s32 *)((char *)arg0 + ((arg1 & 0x1FF) * 4) + 0x40);
        }
    } else {
        var_v0 = func_80036D0C(arg0, temp_a0_2);
    }
    *(s32 *)((char *)arg0 + 0xD4) = func_800270C4((s32 *) *var_v0, *var_s1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039B14);
#endif

#ifdef NON_MATCHING
#define SP(o) (*(s32 **)((u8 *)(o) + 0xBC))

static inline s32 *Operand(struct Obj *o, s32 idx, u32 v) {
    s32 *r;

    if ((v & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            r = SP(o) - 1;
            SP(o) = r;
        } else {
            r = (s32 *)((u8 *)o + ((v & 0x1FF) * 4 + 0x40));
        }
    } else {
        r = func_80036D0C(o, idx);
    }
    return r;
}

void func_80039C08(struct Obj *arg0, u32 arg1) {
    s32 *src;
    s32 *dst;
    s32 *sp;
    u32 hi;
    u8 *owner;
    s32 packed;
    s32 base;
    s32 addr;
    s32 dstVal;
    s32 srcVal;

    hi = arg1 >> 12;
    src = Operand(arg0, hi & 0xFFF, hi);
    dst = Operand(arg0, arg1 & 0xFFF, arg1);
    sp = SP(arg0);
    owner = *(u8 **)((u8 *)arg0 + 0xC);
    SP(arg0) = sp - 1;
    packed = sp[-1];
    base = *(s32 *)(owner + 0x24);
    SP(arg0) = sp - 2;
    addr = sp[-2];
    dstVal = *dst;
    srcVal = *src;
    func_80027600((void *)addr, base + ((u32)packed >> 8) * 4, dstVal, srcVal);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039C08);
#endif
