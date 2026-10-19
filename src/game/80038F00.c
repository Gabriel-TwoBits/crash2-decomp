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
#define OPERAND_MASK 0xFFF
#define OPERAND_SHIFT 12


void Gool_OpChunk(GoolObject *arg0, u32 arg1) {
    s32 *src;
    u8 *sp;
    s32 op;
    s32 n;
    s32 v;

    op = *Gool_ResolveOperand((s32)arg0, (arg1 >> OPERAND_SHIFT) & OPERAND_MASK, 0);
    src = Gool_ResolveOperand((s32)arg0, arg1 & OPERAND_MASK, 0);
    switch (op) {
    case OP_LOAD_A:
        arg0->unk_D4 = (s32)Nsf_OpenEntry(src, 0, 1);
        break;
    case OP_LOAD_F:
        arg0->unk_D4 = (s32)Nsf_OpenEntry(src, 1, 1);
        break;
    case OP_CALL_B:
        arg0->unk_D4 = Nsf_CloseEntry(src, 1);
        break;
    case OP_CALL_C:
        v = Nsf_CloseEntry(src, 0);
        *arg0->sp++ = v;
        break;
    case OP_CALL_D:
        v = Nsf_CountFreePages();
        *arg0->sp++ = v;
        break;
    case OP_CALL_E:
        n = *src;
        v = Nsf_CountPagesFor((u32 **)((u8 *)arg0->sp - n * 4), n);
        sp = (u8 *)arg0->sp - n * 4;
        arg0->sp = (s32 *)(sp + 4);
        *(s32 *)sp = v;
        break;
    default:
        break;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", Gool_OpChunk);
#endif

#ifdef NON_MATCHING
s32 Gool_OpGoToState(GoolObject *arg0, u32 arg1, s32 arg2) {
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
            sp = arg0->sp;
            arg0->sp = sp - 1;
            i--;
            *dst = sp[-1];
            dst--;
        } while (i >= 0);
    }
    mask = arg0->block_flags;
    state = arg0->unk_108;
    flags = *(s32 *)(arg0->exe->items[4] + (arg1 & 0x3FF) * 16);
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
        r = Gool_ChangeState(arg0, arg1 & 0x3FF, n, args);
        if (r < -0xFE && (arg2 & 2)) {
            return r;
        }
        arg0->sp -= 1;
    } else {
        return 0;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", Gool_OpGoToState);
#endif

extern s32 D_8006CB64;

s32 Gool_OpPlayAnim(GoolObject *arg0, u32 arg1, s32 arg2) {
    u8 *q;
    s32 *p;
    s32 t;

    q = arg0->exe->items[5];
    p = arg0->sp;
    arg0->anim_frame = (arg1 & 0x7F) << 8;
    arg0->sp = p + 1;
    arg0->anim = q + ((arg1 >> 5) & 0x7FC);
    t = D_8006CB64;
    *p = (((arg1 >> 16) & 0x3F) << 24) | t;
    switch ((arg1 >> 22) & 3) {
    case 0:
        if (arg0->scale[0] < 0) {
            arg0->scale[0] = -arg0->scale[0];
        }
        break;
    case 1:
        if (arg0->scale[0] > 0) {
            arg0->scale[0] = -arg0->scale[0];
        }
        break;
    case 2:
        arg0->scale[0] = -arg0->scale[0];
        break;
    default:
        break;
    }
    return -((arg2 & 4) != 0) & ~0xFE;
}

s32 Gool_OpPlayFrame(GoolObject *arg0, u32 arg1, s32 arg2) {
    s32 idx = arg1 & 0xFFF;
    s32 idx2 = idx;
    s32 *var_v0;
    s32 *sp;
    s32 t;
    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            var_v0 = arg0->sp - 1;
            arg0->sp = var_v0;
        } else {
            var_v0 = &((s32 *)arg0->links)[arg1 & 0x1FF];
        }
    } else {
        var_v0 = Gool_ResolveRef(arg0, idx2);
    }
    sp = arg0->sp;
    arg0->anim_frame = *var_v0;
    arg0->sp = sp + 1;
    /* The do/while (0) wrapper keeps the D_8006CB64 load after the stores to 0xEC/0xBC. */
    do {
        t =D_8006CB64;
        *sp = (((arg1 >> 16) & 0x3F) << 24) | t;
        switch ((arg1 >> 22) & 3) {
        case 0:
            if (arg0->scale[0] < 0) {
                arg0->scale[0] = -arg0->scale[0];
            }
            break;
        case 1:
            if (arg0->scale[0] > 0) {
                arg0->scale[0] = -arg0->scale[0];
            }
            break;
        case 2:
            arg0->scale[0] = -arg0->scale[0];
            break;
        default:
            break;
        }
        return -((arg2 & 4) != 0) & ~0xFE;
    } while (0);
}

#ifdef NON_MATCHING
s32 Gool_OpSendEvent(GoolObject *o, u32 arg1, s32 arg2) {
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

    first = *(s32 *)((u8 *)o->links + ((arg1 >> 19) & 0x1C));
    idx = arg1 & 0xFFF;
    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            src = o->sp - 1;
            o->sp = src;
        } else {
            src = &((s32 *)o->links)[arg1 & 0x1FF];
        }
    } else {
        src = Gool_ResolveRef(o, idx);
    }
    o->unk_A8 &= 0xFFFDFFFF;
    sel = (arg1 >> 12) & 0x3F;
    if (sel == 0x1F) {
        q = o->sp - 1;
        o->sp = q;
    } else {
        q = &((s32 *)o->links)[sel];
    }
    if (*q != 0 && first != 0 && src != 0) {
        n = (arg1 >> 18) & 7;
        if (n != 0) {
            p = args;
            for (i = 0; i < n; i++) {
                *p++ = o->sp[i - n];
            }
        }
        Gool_SendEvent((s32)o, first, *src, n, args);
    } else {
        o->unk_D4 = 0;
    }
    if (!(o->unk_A8 & 0x20000)) {
        o->sp = (s32 *)((u8 *)o->sp - ((arg1 >> 16) & 0x1C));
        return 0;
    }
    if (!(arg2 & 2)) {
        o->sp = o->sp - 1;
        return 0;
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", Gool_OpSendEvent);
#endif

#ifdef NON_MATCHING
s32 Gool_OpCascadeEvent(GoolObject *o, u32 ins, s32 arg2) {
    s32 args[18];
    s32 *src;
    s32 *chk;
    s32 *dst;
    s32 fn;
    s32 n;
    s32 i;
    s32 idx;
    s32 temp_a0;

    fn = ((s32 *)o->links)[(ins >> 21) & 7];
    temp_a0 = ins & 0xFFF;
    if ((ins & 0xE00) == 0xE00) {
        if (temp_a0 == 0xE1F) {
            src = o->sp - 1;
            o->sp = src;
        } else {
            src = &((s32 *)o->links)[ins & 0x1FF];
        }
    } else {
        src = Gool_ResolveRef(o, temp_a0);
    }
    o->unk_A8 = o->unk_A8 & 0xFFFDFFFF;
    idx = (ins >> 12) & 0x3F;
    if (idx == 0x1F) {
        chk = o->sp - 1;
        o->sp = chk;
    } else {
        chk = &((s32 *)o->links)[idx];
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
        Gool_CascadeEvent((s32)o, fn, *src, (ins >> 21) & 7, n, (s32)args);
    } else {
        o->unk_D4 = 0;
    }
    if (!(o->unk_A8 & 0x20000)) {
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
INCLUDE_ASM("asm/nonmatchings/game/80038F00", Gool_OpCascadeEvent);
#endif

#ifdef NON_MATCHING
#define FIELD_REGS 0x40 /* GoolObject.links: the registers, read by byte offset */
#define OPERAND_MASK 0xFFF
#define OPERAND_KIND_MASK 0xE00
#define OPERAND_REG_MASK 0x1FF
#define OPERAND_POP 0xE1F
#define REG_POP 0x1F
#define REG_MASK 0x3F
#define REG_SHIFT 12
#define ARGC_SHIFT 18
#define ARGC_MASK 7
#define MODE_SHIFT 21
#define MODE_MASK 7
#define POP_BYTES_SHIFT 16
#define POP_BYTES_MASK 0x1C
#define MAX_ARGS 7
#define FLAG_HALT 0x20000
#define ARG2_SKIP 2
#define RESULT_HALTED -0xFF


s32 Gool_OpBroadcastEvent(GoolObject *arg0, u32 arg1, s32 arg2) {
    GoolObject *obj = arg0;
    s32 buf[MAX_ARGS + 1];
    s32 *p;
    s32 *q;
    s32 *sp;
    s32 n;
    s32 i;
    s32 idx = arg1 & OPERAND_MASK;
    u32 reg;

    if ((arg1 & OPERAND_KIND_MASK) == OPERAND_KIND_MASK) {
        if (idx == OPERAND_POP) {
            p = obj->sp - 1;
            obj->sp = p;
        } else {
            p = (s32 *)((u8 *)obj + (arg1 & OPERAND_REG_MASK) * 4 + FIELD_REGS);
        }
    } else {
        p = Gool_ResolveRef(arg0, idx);
    }
    obj->unk_A8 &= ~FLAG_HALT;
    reg = (arg1 >> REG_SHIFT) & REG_MASK;
    if (reg == REG_POP) {
        q = obj->sp - 1;
        obj->sp = q;
    } else {
        q = (s32 *)((u8 *)obj + reg * 4 + FIELD_REGS);
    }
    if (*q != 0 && p != 0) {
        n = (arg1 >> ARGC_SHIFT) & ARGC_MASK;
        sp = obj->sp;
        for (i = 0; i < n; i++) {
            buf[i] = sp[i - n];
        }
        Gool_BroadcastEvent((s32)arg0, *p, (arg1 >> MODE_SHIFT) & MODE_MASK, n, (s32)buf);
    } else {
        obj->unk_D4 = 0;
    }
    if (!(obj->unk_A8 & FLAG_HALT)) {
        obj->sp = (s32 *)((u8 *)obj->sp - ((arg1 >> POP_BYTES_SHIFT) & POP_BYTES_MASK));
        return 0;
    } else if (!(arg2 & ARG2_SKIP)) {
        obj->sp -= 1;
        return 0;
    } else {
        return RESULT_HALTED;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", Gool_OpBroadcastEvent);
#endif

#ifdef NON_MATCHING
#define CTX_REGS_OFFSET 0x40 /* GoolObject.links: the registers, read by byte offset */
#define REG_SP 0x1F
#define ERR_NOT_READY -0xE
#define ERR_UNHANDLED -0xFF
#define ERR_CODE_LIMIT -0xFE
#define STATUS_READY 0x8
#define STATUS_TAKEN 0x20
#define OPCODE_SKIP 0x3D
#define CODE_CALL 0xFF
#define COND_ALWAYS 0
#define COND_VALUE 1
#define COND_ZERO 2
#define ACTION_ADJUST 0
#define ACTION_POP_CALL 1
#define ACTION_CALL 2

s32 Gool_OpAcceptOrRejectEvent(GoolObject *arg0, u32 arg1, s32 arg2, s32 *arg3) {
    s32 action = (arg1 >> 0x16) & 3;
    s32 mode = (arg1 >> 0x14) & 3;
    s32 cond;
    u32 idx;
    s32 *p;
    s32 v;
    s32 n;
    s32 *sp;

    *(s32 *)0x1F800048 = arg2;
    if (!(arg2 & STATUS_READY)) {
        return ERR_NOT_READY;
    }
    switch (mode) {
    case COND_ALWAYS:
        cond = 1;
        break;
    case COND_VALUE:
    case COND_ZERO:
        idx = (arg1 >> 0xE) & 0x3F;
        if (idx == REG_SP) {
            p = arg0->sp - 1;
            arg0->sp = p;
        } else {
            p = (s32 *)((u8 *)arg0 + idx * 4 + CTX_REGS_OFFSET);
        }
        if (mode == COND_VALUE) {
            cond = *p;
        } else {
            cond = *p == 0;
        }
        break;
    default:
        /* mode 3 leaves the condition register unset; retail tests the caller's $t1, taken as nonzero */
        cond = 1;
        break;
    }
    arg2 |= STATUS_TAKEN;
    if (cond != 0) {
        arg3[1] = (arg1 >> 0x18) != OPCODE_SKIP;
        *(s32 *)0x1F800048 = arg2;
        if (action == ACTION_POP_CALL) {
            n = (arg1 >> 0xA) & 0xF;
            for (v = n - 1; v >= 0; v--) {
                sp = arg0->sp;
                arg0->sp = sp - 1;
                arg3[v + 3] = sp[-1];
            }
            arg3[2] = n;
            v = Gool_PopFrame(arg0, 0);
            if (v < ERR_CODE_LIMIT) {
                arg3[0] = arg1 & 0x3FF;
                return ERR_UNHANDLED;
            }
            return v;
        } else if (action == ACTION_CALL) {
            v = Gool_PopFrame(arg0, 0);
            if (v < ERR_CODE_LIMIT) {
                arg3[0] = CODE_CALL;
                return ERR_UNHANDLED;
            }
            return v;
        } else {
            return 0;
        }
    }
    if (action == ACTION_ADJUST) {
        arg0->pc += (s32)(arg1 << 0x16) >> 0x14;
        arg0->sp = (s32 *)((u8 *)arg0->sp - ((arg1 >> 8) & 0x3C));
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", Gool_OpAcceptOrRejectEvent);
#endif

void func_80039A14(GoolObject *arg0, s32 arg1) {
    s32 idx = arg1 & 0xFFF;
    s32 idx2 = idx;
    s32 *var_v0;
    s32 temp_v0;
    s32 *sp;

    if ((arg1 & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            var_v0 = arg0->sp - 1;
            arg0->sp = var_v0;
        } else {
            var_v0 = &((s32 *)arg0->links)[arg1 & 0x1FF];
        }
    } else {
        var_v0 = Gool_ResolveRef(arg0, idx2);
    }
    temp_v0 = func_80027664(var_v0);
    sp = arg0->sp;
    arg0->sp = sp + 1;
    *sp = temp_v0;
}

void func_80039AA4(GoolObject *arg0, s32 arg1) {
    s32 *var_v0;
    s32 temp_a2;
    s32 a3;

    temp_a2 = arg1 & 0xFFF;
    a3 = temp_a2;
    if ((arg1 & 0xE00) == 0xE00) {
        if (temp_a2 == 0xE1F) {
            var_v0 = arg0->sp - 1;
            arg0->sp = var_v0;
        } else {
            var_v0 = &((s32 *)arg0->links)[arg1 & 0x1FF];
        }
    } else {
        var_v0 = Gool_ResolveRef(arg0, a3);
    }
    func_800278F8(*var_v0);
}

#ifdef NON_MATCHING
void func_80039B14(GoolObject *arg0, u32 arg1) {
    s32 *var_s1;
    s32 *var_v0;
    s32 temp_a0_2;
    s32 temp_a1;
    u32 temp_a0;

    temp_a0 = arg1 >> 12;
    temp_a1 = temp_a0 & 0xFFF;
    if ((temp_a0 & 0xE00) == 0xE00) {
        if (temp_a1 == 0xE1F) {
            var_s1 = arg0->sp - 1;
            arg0->sp = var_s1;
        } else {
            var_s1 = (s32 *)((char *)arg0 + ((temp_a0 & 0x1FF) * 4) + 0x40);
        }
    } else {
        var_s1 = Gool_ResolveRef(arg0, temp_a1);
    }
    temp_a0_2 = arg1 & 0xFFF;
    if ((arg1 & 0xE00) == 0xE00) {
        if (temp_a0_2 == 0xE1F) {
            var_v0 = arg0->sp - 1;
            arg0->sp = var_v0;
        } else {
            var_v0 = (s32 *)((char *)arg0 + ((arg1 & 0x1FF) * 4) + 0x40);
        }
    } else {
        var_v0 = Gool_ResolveRef(arg0, temp_a0_2);
    }
    arg0->unk_D4 = func_800270C4((s32 *) *var_v0, *var_s1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039B14);
#endif

#ifdef NON_MATCHING
static inline s32 *Operand(GoolObject *o, s32 idx, u32 v) {
    s32 *r;

    if ((v & 0xE00) == 0xE00) {
        if (idx == 0xE1F) {
            r = o->sp - 1;
            o->sp = r;
        } else {
            r = &((s32 *)o->links)[v & 0x1FF];
        }
    } else {
        r = Gool_ResolveRef(o, idx);
    }
    return r;
}

void func_80039C08(GoolObject *arg0, u32 arg1) {
    s32 *src;
    s32 *dst;
    s32 *sp;
    u32 hi;
    NsfEntry *owner;
    s32 packed;
    s32 base;
    s32 addr;
    s32 dstVal;
    s32 srcVal;

    hi = arg1 >> 12;
    src = Operand(arg0, hi & 0xFFF, hi);
    dst = Operand(arg0, arg1 & 0xFFF, arg1);
    sp = arg0->sp;
    owner = arg0->exe;
    arg0->sp = sp - 1;
    packed = sp[-1];
    base = (s32)owner->items[5];
    arg0->sp = sp - 2;
    addr = sp[-2];
    dstVal = *dst;
    srcVal = *src;
    func_80027600((void *)addr, base + ((u32)packed >> 8) * 4, dstVal, srcVal);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80038F00", func_80039C08);
#endif
