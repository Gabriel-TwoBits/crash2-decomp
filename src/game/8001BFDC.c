#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
#define FIELD(base, type, off) (*(type *)((u8 *)(base) + (off)))
#define OBJ(p) ((GoolObject *)(p))

#define FLAGS_A8_OFF 0xA8
#define FLAGS_100_OFF 0x100
#define ANIM_OFF 0xE8
#define SUB_OFF 0x20

#define FLAG_AC_DONE 0x80
#define FLAG_A8_ABORT 0x1
#define FLAG_100_ABORT 0x20000000

#define STATE_IDLE 0
#define STATE_SKIP 1
#define STATE_SPIN 2
#define STATE_WAIT_1C4 3
#define STATE_WAIT_3D 4
#define STATE_WAIT_25B 5
#define STATE_FINISH_A 6
#define STATE_FINISH_B 7

#define LIMIT_FINISH 0x10
#define LIMIT_3D 0x3D
#define LIMIT_1C4 0x1C4
#define LIMIT_25B 0x25B

#define SPIN_SIZE 0x3C
#define SPIN_PERIOD 4
#define SPIN_SHIFT 10
#define ANIM_END_TYPE 4
#define RETURN_VALUE (-0xFF)

extern s32 D_80060944;
extern u32 D_8006CB64;


static inline void spin(u8 *s) {
    s32 t = D_80060944;
    s32 a = t;

    if (t < 0) {
        a = t + (SPIN_PERIOD - 1);
    }
    FIELD(s, s16, 0x14) = SPIN_SIZE;
    FIELD(s, s16, 0x16) = SPIN_SIZE;
    FIELD(s, s16, 0x18) = SPIN_SIZE;
    FIELD(s, s16, 4) = (t - ((a >> 2) * SPIN_PERIOD)) << SPIN_SHIFT;
}

static inline void copy4(u8 *s, s32 from, s16 sel) {
    FIELD(s, s16, 6) = sel;
    FIELD(s, s16, 0x1A) = FIELD(s, s16, from);
    FIELD(s, s16, 0x1C) = FIELD(s, s16, from + 2);
    FIELD(s, s16, 0x1E) = FIELD(s, s16, from + 4);
}

s32 func_8001BFDC(void *arg0) {
    u8 *s = (u8 *)arg0 + SUB_OFF;
    u8 *anim;
    s16 v;
    u32 elapsed;

    switch (OBJ(arg0)->display_mode) {
    case STATE_SKIP:
        break;
    case STATE_SPIN:
        spin(s);
        break;
    case STATE_WAIT_1C4:
        elapsed = D_8006CB64 - OBJ(arg0)->unk_10C;
        if (elapsed >= LIMIT_1C4) {
            OBJ(arg0)->display_mode = STATE_IDLE;
        } else {
            /* state stays 3: the later state checks cannot fire */
        }
        spin(s);
        break;
    case STATE_WAIT_3D:
        elapsed = D_8006CB64 - OBJ(arg0)->unk_10C;
        if (elapsed >= LIMIT_3D) {
            OBJ(arg0)->display_mode = STATE_IDLE;
        } else {
            /* keep waiting */
        }
        break;
    case STATE_WAIT_25B:
        elapsed = D_8006CB64 - OBJ(arg0)->unk_10C;
        if (elapsed >= LIMIT_25B || (OBJ(arg0)->state_flags & FLAG_100_ABORT)) {
            OBJ(arg0)->display_mode = STATE_IDLE;
        } else {
            /* keep waiting */
        }
        spin(s);
        break;
    case STATE_FINISH_A:
        elapsed = D_8006CB64 - OBJ(arg0)->unk_10C;
        if (elapsed >= LIMIT_FINISH) {
            OBJ(arg0)->display_mode = STATE_IDLE;
            OBJ(arg0)->status_b |= FLAG_AC_DONE;
        } else {
            /* keep waiting */
        }
        break;
    case STATE_FINISH_B:
        elapsed = D_8006CB64 - OBJ(arg0)->unk_10C;
        if (elapsed >= LIMIT_FINISH || (OBJ(arg0)->status_a & FLAG_A8_ABORT)) {
            OBJ(arg0)->display_mode = STATE_IDLE;
            OBJ(arg0)->status_b |= FLAG_AC_DONE;
        } else {
            /* keep waiting */
        }
        break;
    default:
        func_8001BFD4(arg0, 1);
        break;
    }

    anim = OBJ(arg0)->anim;
    if (anim == 0) {
        return RETURN_VALUE;
    } else if (*anim == ANIM_END_TYPE) {
        return RETURN_VALUE;
    } else {
        copy4(s, 0xE, FIELD(s, s16, 2));
        v = FIELD(s, s16, 0);
        if (v != 0) {
            copy4(s, 8, v);
        } else {
            /* no override */
        }
        v = FIELD(s, s16, 4);
        if (v != 0) {
            copy4(s, 0x14, v);
        } else {
            /* no override */
        }
        return RETURN_VALUE;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001BFDC);
#endif

extern u8 g_GoolOpcodeHandlers;
extern s32 g_Player;
extern s32 D_80060944;
extern GfxFrame *g_DrawBuffer;
extern u32 D_8006CB64;
extern s32 D_8006CCAC;
extern s32 func_80018CAC(void *, void *, void *);
/* Without a prototype: its equivalent C under NON_MATCHING defines the parameters' types */

void Obj_UpdateAll(void *arg0) {
    D_8006CB64 = (u32)g_DrawBuffer->start_ticks / 34;
    func_8002CA98();
    D_8006CCAC = D_80060944;
    Gool_InitScratchpad();
    if (g_Player != 0) {
        Obj_ForEachList(&func_80018CAC, &Obj_Update, arg0);
    }
    *(u8 **)SCRATCH_GOOL_TABLE = &g_GoolOpcodeHandlers;
}

#ifdef NON_MATCHING
typedef struct Unk8001C340Entry {
    s32 unk_0;
    s32 unk_4;
    u16 unk_8;
    u16 unk_A;
    u16 unk_C;
    u16 unk_E;
} Unk8001C340Entry;

#define FIELD(T, o) (*(T *)((char *)arg0 + (o)))

#define ENTRY_STRIDE 0x10
#define OFFSET_NONE 0x3FFF
#define OFFSET_MASK 0x3FFF
#define OFFSET_FROM_OWN_BASE 0x4000
#define FLAG_BLOCKED 0x10000000
#define FLAGS_STARTED 0x20020
#define ARG_MASK 0xFFFF
#define RESULT_BAD_ENTRY -0xE
#define RESULT_FAIL -0xFF
#define RESULT_MIN_OK -0xFE
#define ENTRY_INDEX_NONE 0xFF
#define CMD_RESUME 0x13
#define CMD_START 3

extern s32 D_8006CB60;

s32 Gool_ChangeState(void *arg0, s32 arg1, s32 arg2, s32 *arg3) {
    s32 prev_d0;
    Unk8001C340Entry *entry;
    char *own;
    u16 word;
    s32 r;
    char *base;
    s32 i;
    s32 *sp;

    prev_d0 = OBJ(arg0)->unk_D0;
    if (arg1 == ENTRY_INDEX_NONE || (OBJ(arg0)->status_b & FLAG_BLOCKED)) {
        return RESULT_BAD_ENTRY;
    }
    OBJ(arg0)->state = arg1;
    entry = (Unk8001C340Entry *)(OBJ(arg0)->exe->items[GOOL_ITEM_STATES] + arg1 * ENTRY_STRIDE);
    OBJ(arg0)->block_flags = entry->unk_4;
    own = Nsf_OpenEntry(((NsfEntry *)OBJ(arg0)->unk_10)->items[GOOL_ITEM_POOL] + entry->unk_8 * 4, 1, 0);
    OBJ(arg0)->state_exe = own;

    word = entry->unk_E;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            r = (s32)((NsfEntry *)own)->items[GOOL_ITEM_CODE];
        } else {
            r = (s32)OBJ(arg0)->exe->items[GOOL_ITEM_CODE];
        }
        r += (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    OBJ(arg0)->pc = r;

    word = entry->unk_A;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            base = OBJ(arg0)->state_exe;
        } else {
            base = (char *)OBJ(arg0)->exe;
        }
        r = (s32)((NsfEntry *)base)->items[GOOL_ITEM_CODE] + (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    OBJ(arg0)->event_pc = r;

    word = entry->unk_C;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            base = OBJ(arg0)->state_exe;
        } else {
            base = (char *)OBJ(arg0)->exe;
        }
        r = (s32)((NsfEntry *)base)->items[GOOL_ITEM_CODE] + (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    OBJ(arg0)->trans_pc = r;

    OBJ(arg0)->unk_D0 = 0;
    OBJ(arg0)->fp = (u8 *)OBJ(arg0)->links;
    OBJ(arg0)->sp = (s32 *)OBJ(arg0)->links + *(s32 *)(OBJ(arg0)->exe->items[GOOL_ITEM_HEADER] + 0xC);
    OBJ(arg0)->status_a |= FLAGS_STARTED;
    OBJ(arg0)->state_flags = entry->unk_0;
    for (i = 0; i < arg2; i++) {
        sp = OBJ(arg0)->sp;
        OBJ(arg0)->sp = sp + 1;
        *sp = *arg3++;
    }
    Gool_PushFrame(arg0, arg2, ARG_MASK);
    if (prev_d0 != 0) {
        Gool_PushFrame(arg0, 0, ARG_MASK);
        OBJ(arg0)->pc = prev_d0;
        i = Gool_Interpret(arg0, CMD_RESUME, 0);
        if (i >= RESULT_MIN_OK) {
            return i;
        }
    } else {
        sp = OBJ(arg0)->sp;
        OBJ(arg0)->sp = sp + 1;
        *sp = 0;
    }
    OBJ(arg0)->state_time = D_8006CB64;
    if (OBJ(arg0)->trans_pc != 0) {
        if ((s32)arg0 == D_8006CB60) {
            Gool_PushFrame(arg0, 0, ARG_MASK);
            OBJ(arg0)->pc = OBJ(arg0)->trans_pc;
            i = Gool_Interpret(arg0, CMD_START, 0);
            if (i >= RESULT_MIN_OK) {
                return i;
            }
        } else {
            return RESULT_FAIL;
        }
    }
    return RESULT_FAIL;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", Gool_ChangeState);
#endif

#ifdef NON_MATCHING
s32 Gool_PushFrame(GoolObject *arg0, s32 arg1, s32 arg2) {
    u8 *cur;
    s32 prev;
    s32 *p;

    cur = (u8 *)arg0->sp;
    p = arg0->sp;
    prev = (s32)arg0->fp;
    arg0->fp = cur;
    arg0->sp = p + 1;
    *p = arg2;
    p = arg0->sp;
    arg0->sp = p + 1;
    *p = arg0->pc;
    p = arg0->sp;
    arg0->sp = p + 1;
    *p = ((cur - arg1 * 4 - 0x40) - (u8 *)arg0) | ((prev - 0x40 - (s32)arg0) << 16);
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", Gool_PushFrame);
#endif

#ifdef NON_MATCHING
s32 Gool_PopFrame(GoolObject *arg0, u32 *arg1) {
    u32 temp_a3;
    u32 temp_t0;
    u32 *v1;
    u32 *v1b;
    u32 w;

    v1 = (u32 *)arg0->fp;
    arg0->sp = (s32 *)(v1 + 2);
    temp_a3 = v1[2];
    temp_t0 = temp_a3 >> 16;
    if (temp_t0 == 0) {
        return -0x19;
    }
    arg0->sp = (s32 *)(v1 + 1);
    arg0->pc = v1[1];
    v1b = *(u32 * volatile *)&arg0->sp;
    arg0->sp = (s32 *)(v1b - 1);
    w = v1b[-1];
    if (arg1 != 0) {
        *arg1 = (*arg1 & 0xFFFF0000) | (w & 0xFFFF);
    }
    arg0->sp = (s32 *)&((u8 *)arg0)[(temp_a3 & 0xFFFF) + 0x40];
    arg0->fp = &((u8 *)arg0)[temp_t0 + 0x40];
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", Gool_PopFrame);
#endif

#ifdef NON_MATCHING
#define OBJ_STATE_ACTIVE 3
#define RESULT_NOT_ACTIVE (-0xFF)
#define RESULT_ERROR_LIMIT (-0xFE)
#define MASK_LOW24 0xFFFFFF
#define SHIFT_HIGH8 24
#define ARG_FULL_RANGE 0xFFFF

#define OFF_MODEL 0x0C
#define OFF_MODEL_INFO 0x10
#define OFF_VEC_A 0x60
#define OFF_VEC_B 0x6C

#define AC_BIT_COUNTDOWN 0x10000000
#define AC_BIT_COPY_VEC 0x10
#define AC_BIT_SKIP 0x100
#define AC_BIT_ALT 0x2000000
#define A8_BIT_CLEAR 0x20
#define F100_BIT_FORCE 0x100000
#define F100_BIT_ALT 0x20000

#define PAD_ANY 0x8
#define PAD_FORCE 0x8000
#define PAD_FIRE 0x4
#define PAD_MODE_A 0x10
#define PAD_FORCE2 0x4000
#define PAD_HOLD 0x8000000

#define FIELD(o, off, T) (*(T *)((u8 *)(o) + (off)))
#define MODEL_KIND(o) FIELD(FIELD(FIELD(o, OFF_MODEL, u8 *), OFF_MODEL_INFO, u8 *), 4, u32)

extern u32 g_FrameFlags;
extern s32 g_Paused;

s32 Obj_Update(u8 *obj, s32 flag) {
    s32 out[2];
    s32 vec[6];
    u32 pad;
    u32 flags100;
    u32 sel;
    u32 hit;
    s32 r;
    u32 top;

    if (OBJ(obj)->unk_4 != OBJ_STATE_ACTIVE) {
        return RESULT_NOT_ACTIVE;
    }
    if (obj == (u8 *)g_Player && !FRAME_BETWEEN) {
        Input_Update();
    } else {
    }
    D_8006CB60 = (s32)obj;
    pad = g_FrameFlags;
    hit = pad & PAD_ANY;
    if (hit != 0) {
        flags100 = ((u32)OBJ(obj)->state_flags);
        if (flags100 & F100_BIT_FORCE) {
            flag = 1;
        } else {
        }
        if (((((u32)OBJ(obj)->status_b) & AC_BIT_ALT) || (flags100 & F100_BIT_ALT)) && (pad & PAD_FORCE)) {
            hit = 1;
        } else {
            sel = MODEL_KIND(obj);
            switch (sel) {
            case 0x100: hit = g_FrameFlags & 0x20; break;
            case 0x200: hit = g_FrameFlags & 0x100; break;
            case 0x300:
            case 0x500:
            case 0x600: hit = g_FrameFlags & 0x80; break;
            case 0x400:
            case 0x900: hit = g_FrameFlags & 0x400; break;
            default: break;
            }
        }
    } else {
    }
    if (flag != 0 && hit != 0 && !FRAME_BETWEEN) {
        if ((((u32)OBJ(obj)->status_b) & AC_BIT_COUNTDOWN) && OBJ(obj)->unk_E4 != 0) {
            s32 n = OBJ(obj)->unk_E4 - 1;
            OBJ(obj)->unk_E4 = n;
            if (n == 0) {
                OBJ(obj)->status_b &= ~AC_BIT_COUNTDOWN;
            } else {
            }
            func_8001AA80((GoolObject *)obj, out);
        } else {
            OBJ(obj)->frame_time = D_8006CB64;
            if (((u32)OBJ(obj)->status_b) & AC_BIT_COPY_VEC) {
                vec[0] = OBJ(obj)->pos[0];
                vec[1] = FIELD(obj, OFF_VEC_A + 4, s32);
                vec[2] = FIELD(obj, OFF_VEC_A + 8, s32);
                vec[3] = OBJ(obj)->rot[0];
                vec[4] = FIELD(obj, OFF_VEC_B + 4, s32);
                vec[5] = FIELD(obj, OFF_VEC_B + 8, s32);
            } else {
            }
            if (OBJ(obj)->trans_pc != 0) {
                Gool_PushFrame((GoolObject *)obj, 0, ARG_FULL_RANGE);
                OBJ(obj)->pc = OBJ(obj)->trans_pc;
                r = Gool_Interpret(obj, 3, 0);
                if (r >= RESULT_ERROR_LIMIT) {
                    return r;
                } else {
                }
            } else {
            }
            top = (u32)OBJ(obj)->sp;
            sel = *(u32 *)(top - 4);
            if (D_8006CB64 - (sel & MASK_LOW24) >= (sel >> SHIFT_HIGH8)) {
                OBJ(obj)->sp = (s32 *)(top - 4);
                r = Gool_Interpret(obj, 4, 0);
                if (r >= RESULT_ERROR_LIMIT) {
                    return r;
                } else {
                }
            } else {
            }
            ((s32 (*)(void *, s32))func_8001BFDC)(obj, flag);
            Obj_UpdatePhysics(obj, vec, out);
            OBJ(obj)->status_a &= ~A8_BIT_CLEAR;
        }
    } else {
        func_8001AA80((GoolObject *)obj, out);
    }
    if (OBJ(obj)->anim != 0 && OBJ(obj)->unk_120 != 1
        && !(((u32)OBJ(obj)->status_b) & AC_BIT_SKIP) && (g_FrameFlags & PAD_FIRE)) {
        pad = g_FrameFlags;
        if (((((u32)OBJ(obj)->status_b) & AC_BIT_ALT) || (((u32)OBJ(obj)->state_flags) & F100_BIT_ALT))
            && (pad & PAD_FORCE2)) {
            hit = 1;
        } else {
            sel = MODEL_KIND(obj);
            hit = 0;
            switch (sel) {
            case 0x100:
                if (g_FrameFlags & PAD_MODE_A) {
                    if ((D_80060944 & 1) || g_Paused != 0 || OBJ(obj)->display_mode != 4) {
                        hit = 1;
                    } else {
                    }
                } else {
                }
                break;
            case 0x200: hit = g_FrameFlags & 0x200; break;
            case 0x300:
            case 0x500:
            case 0x600: hit = g_FrameFlags & 0x40; break;
            case 0x400:
            case 0x900: hit = g_FrameFlags & 0x800; break;
            default: break;
            }
        }
        if (flag == 0 && !(g_FrameFlags & PAD_HOLD) && !(((u32)OBJ(obj)->state_flags) & F100_BIT_FORCE)) {
            hit = 0;
        } else {
        }
        if (hit != 0) {
            Gfx_QueueObject(obj, out);
        } else {
        }
    } else {
    }
    D_8006CB60 = 0;
    return RESULT_NOT_ACTIVE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", Obj_Update);
#endif

#ifdef NON_MATCHING
#define SCRIPT_NONE 0xFF
#define SCRIPT_RUNS 0x8000
#define SCRIPT_INDEX_MASK 0x3FFF
#define RESULT_NONE -0xFF
#define RESULT_PENDING -0x1A
#define FLAG_BIT2 2
#define FLAG_FORCE 0x1002
#define OBJ_FLAG_A8_SET 0x10000
#define ID_1800 0x1800
#define ID_1900 0x1900
#define ID_2500 0x2500
#define ID_2600 0x2600
#define PUSH_ARG_COUNT 2
#define SCRIPT_ARG_LIMIT 0xFFFF
#define STATE_FIRST 2

typedef struct Unk8001CC10Tab {
    u8 pad0[0x10];
    s32 count;
} Unk8001CC10Tab;

/* GoolObject.exe's items, as this function reads them */
#define BANK_TAB(o) ((Unk8001CC10Tab *)(o)->exe->items[GOOL_ITEM_HEADER])
#define BANK_CODE(o) ((s32 *)(o)->exe->items[GOOL_ITEM_CODE])
#define BANK_INDEX(o) ((u16 *)(o)->exe->items[GOOL_ITEM_STATE_MAP])
#define BANK_FLAGS(o) ((s32 *)(o)->exe->items[GOOL_ITEM_STATES])

s32 Gool_SendEvent(s32 arg0, s32 arg1, s32 id, s32 n, void *arg4) {
#define self ((GoolObject *)arg0)
#define obj ((GoolObject *)arg1)
#define args ((s32 *)arg4)
    s32 out[22];
    s32 r;
    s32 s3;
    s32 i;
    s32 *src;
    s32 fl;
    s32 mask;
    s32 st;
    s32 cur;

    r = RESULT_PENDING;
    if (obj != 0) {
        if (self != 0) {
            self->misc = 1;
        }
        obj->links[GOOL_LINK_INTERRUPTER] = self;
        if (obj->event_pc != 0) {
            *obj->sp++ = id;
            *obj->sp++ = (s32)args;
            Gool_PushFrame(obj, PUSH_ARG_COUNT, SCRIPT_ARG_LIMIT);
            obj->pc = obj->event_pc;
            r = Gool_Interpret(obj, 8, (s32)out);
            if (r != RESULT_PENDING) {
                if (r >= -0xFE) {
                    return r;
                } else {
                    s3 = out[0];
                }
            }
            if (self != 0) {
                self->misc = out[1];
            }
        }
        if (r >= -0xFE) {
            if ((id >> 8) < BANK_TAB(obj)->count) {
                s3 = BANK_INDEX(obj)[id >> 8];
            } else {
                s3 = SCRIPT_NONE;
            }
            if (self != 0) {
                self->misc = s3 != SCRIPT_NONE;
            }
            if (s3 & SCRIPT_RUNS) {
                obj->event_received = id;
                i = 0;
                if (n > 0) {
                    src = args;
                    do {
                        *obj->sp++ = *src;
                        i++;
                        src++;
                    } while (i < n);
                }
                Gool_PushFrame(obj, n, SCRIPT_ARG_LIMIT);
                obj->pc = (s32)(BANK_CODE(obj) + (s3 & SCRIPT_INDEX_MASK));
                return Gool_Interpret(obj, 3, 0);
            }
        }
        if (s3 == SCRIPT_NONE) {
            return RESULT_NONE;
        }
        fl = obj->block_flags;
        if (id == ID_1800 || id == ID_2600 || id == ID_1900 || id == ID_2500) {
            fl &= ~FLAG_BIT2;
        }
        cur = g_Player;
        mask = BANK_FLAGS(obj)[s3 * 4];
        st = fl & mask;
        if ((s32)obj == cur) {
            if ((u32)(obj->display_mode - 3) < 2U || obj->display_mode == STATE_FIRST) {
                st = fl;
                st = (st | FLAG_FORCE) & mask;
            }
        }
        if (st == 0) {
            obj->event_received = id;
            if (id == ID_1800 || id == ID_1900) {
                obj->status_a |= OBJ_FLAG_A8_SET;
            }
            Gool_ChangeState(obj, s3, n, (s32 *)args);
            return RESULT_NONE;
        }
    }
    if (self != 0) {
        self->misc = 0;
    }
    return RESULT_NONE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", Gool_SendEvent);
#endif
