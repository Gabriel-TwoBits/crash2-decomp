#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
#define FIELD(base, type, off) (*(type *)((u8 *)(base) + (off)))

#define STATE_OFF 0x108
#define STATE_TIME_OFF 0x10C
#define FLAGS_A8_OFF 0xA8
#define FLAGS_AC_OFF 0xAC
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

    switch (FIELD(arg0, s32, STATE_OFF)) {
    case STATE_SKIP:
        break;
    case STATE_SPIN:
        spin(s);
        break;
    case STATE_WAIT_1C4:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_1C4) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
        } else {
            /* state stays 3: the later state checks cannot fire */
        }
        spin(s);
        break;
    case STATE_WAIT_3D:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_3D) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
        } else {
            /* keep waiting */
        }
        break;
    case STATE_WAIT_25B:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_25B || (FIELD(arg0, s32, FLAGS_100_OFF) & FLAG_100_ABORT)) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
        } else {
            /* keep waiting */
        }
        spin(s);
        break;
    case STATE_FINISH_A:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_FINISH) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
            FIELD(arg0, s32, FLAGS_AC_OFF) |= FLAG_AC_DONE;
        } else {
            /* keep waiting */
        }
        break;
    case STATE_FINISH_B:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_FINISH || (FIELD(arg0, s32, FLAGS_A8_OFF) & FLAG_A8_ABORT)) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
            FIELD(arg0, s32, FLAGS_AC_OFF) |= FLAG_AC_DONE;
        } else {
            /* keep waiting */
        }
        break;
    default:
        func_8001BFD4(arg0, 1);
        break;
    }

    anim = FIELD(arg0, u8 *, ANIM_OFF);
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

extern u8 D_8005C514;
extern s32 D_8005F38C;
extern s32 D_80060944;
extern u32 *g_DrawBuffer;
extern u32 D_8006CB64;
extern s32 D_8006CCAC;
extern u8 func_80018CAC;
/* Without a prototype: its equivalent C under NON_MATCHING defines the parameters' types */

void func_8001C29C(void *arg0) {
    D_8006CB64 = g_DrawBuffer[3] / 34;
    func_8002CA98();
    D_8006CCAC = D_80060944;
    func_80019F08();
    if (D_8005F38C != 0) {
        func_80018DD0(&func_80018CAC, &func_8001C718, arg0);
    }
    *(u8 **)0x1F80005C = &D_8005C514;
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
#define BLOCK_BASE_OFFSET 0x14
#define STACK_BASE_OFFSET 0x40
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

s32 func_8001C340(void *arg0, s32 arg1, s32 arg2, s32 *arg3) {
    s32 prev_d0;
    Unk8001C340Entry *entry;
    char *own;
    u16 word;
    s32 r;
    char *base;
    s32 i;
    s32 *sp;

    prev_d0 = FIELD(s32, 0xD0);
    if (arg1 == ENTRY_INDEX_NONE || (FIELD(s32, 0xAC) & FLAG_BLOCKED)) {
        return RESULT_BAD_ENTRY;
    }
    FIELD(s32, 0x1C) = arg1;
    entry = (Unk8001C340Entry *)(*(char **)(*(char **)((char *)arg0 + 0xC) + 0x20) + arg1 * ENTRY_STRIDE);
    FIELD(s32, 0xB0) = entry->unk_4;
    own = func_80014260(*(char **)(*(char **)((char *)arg0 + 0x10) + 0x18) + entry->unk_8 * 4, 1, 0);
    FIELD(void *, 0x14) = own;

    word = entry->unk_E;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            r = *(s32 *)(own + BLOCK_BASE_OFFSET);
        } else {
            r = *(s32 *)(FIELD(char *, 0xC) + BLOCK_BASE_OFFSET);
        }
        r += (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    FIELD(s32, 0xC0) = r;

    word = entry->unk_A;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            base = FIELD(char *, 0x14);
        } else {
            base = FIELD(char *, 0xC);
        }
        r = *(s32 *)(base + BLOCK_BASE_OFFSET) + (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    FIELD(s32, 0xCC) = r;

    word = entry->unk_C;
    if ((word & 0xFFFF) != OFFSET_NONE) {
        if (word & OFFSET_FROM_OWN_BASE) {
            base = FIELD(char *, 0x14);
        } else {
            base = FIELD(char *, 0xC);
        }
        r = *(s32 *)(base + BLOCK_BASE_OFFSET) + (word & OFFSET_MASK) * 4;
    } else {
        r = 0;
    }
    FIELD(s32, 0xC8) = r;

    FIELD(s32, 0xD0) = 0;
    FIELD(char *, 0xC4) = (char *)arg0 + STACK_BASE_OFFSET;
    FIELD(char *, 0xBC) = (char *)arg0 + STACK_BASE_OFFSET + (*(s32 *)(*(char **)(*(char **)((char *)arg0 + 0xC) + 0x10) + 0xC)) * 4;
    FIELD(s32, 0xA8) |= FLAGS_STARTED;
    FIELD(s32, 0x100) = entry->unk_0;
    for (i = 0; i < arg2; i++) {
        sp = FIELD(s32 *, 0xBC);
        FIELD(s32 *, 0xBC) = sp + 1;
        *sp = *arg3++;
    }
    func_8001C620(arg0, arg2, ARG_MASK);
    if (prev_d0 != 0) {
        func_8001C620(arg0, 0, ARG_MASK);
        FIELD(s32, 0xC0) = prev_d0;
        i = func_8003A014(arg0, CMD_RESUME, 0);
        if (i >= RESULT_MIN_OK) {
            return i;
        }
    } else {
        sp = FIELD(s32 *, 0xBC);
        FIELD(s32 *, 0xBC) = sp + 1;
        *sp = 0;
    }
    FIELD(u32, 0xE0) = D_8006CB64;
    if (FIELD(s32, 0xC8) != 0) {
        if ((s32)arg0 == D_8006CB60) {
            func_8001C620(arg0, 0, ARG_MASK);
            FIELD(s32, 0xC0) = FIELD(s32, 0xC8);
            i = func_8003A014(arg0, CMD_START, 0);
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
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C340);
#endif

#ifdef NON_MATCHING
s32 func_8001C620(u8 *arg0, s32 arg1, s32 arg2) {
    u8 *cur;
    s32 prev;
    s32 *p;

    cur = *(u8 **)(arg0 + 0xBC);
    p = *(s32 **)(arg0 + 0xBC);
    prev = *(s32 *)(arg0 + 0xC4);
    *(u8 **)(arg0 + 0xC4) = cur;
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = arg2;
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = *(s32 *)(arg0 + 0xC0);
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = ((cur - arg1 * 4 - 0x40) - arg0) | ((prev - 0x40 - (s32)arg0) << 16);
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C620);
#endif

#ifdef NON_MATCHING
s32 func_8001C688(u8 *arg0, u32 *arg1) {
    u32 temp_a3;
    u32 temp_t0;
    u32 *v1;
    u32 *v1b;
    u32 w;

    v1 = *(u32 **)(arg0 + 0xC4);
    *(u32 **)(arg0 + 0xBC) = v1 + 2;
    temp_a3 = v1[2];
    temp_t0 = temp_a3 >> 16;
    if (temp_t0 == 0) {
        return -0x19;
    }
    *(u32 **)(arg0 + 0xBC) = v1 + 1;
    *(u32 *)(arg0 + 0xC0) = v1[1];
    v1b = *(u32 * volatile *)(arg0 + 0xBC);
    *(u32 **)(arg0 + 0xBC) = v1b - 1;
    w = v1b[-1];
    if (arg1 != 0) {
        *arg1 = (*arg1 & 0xFFFF0000) | (w & 0xFFFF);
    }
    *(u8 **)(arg0 + 0xBC) = &arg0[(temp_a3 & 0xFFFF) + 0x40];
    *(u8 **)(arg0 + 0xC4) = &arg0[temp_t0 + 0x40];
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C688);
#endif

#ifdef NON_MATCHING
#define OBJ_STATE_ACTIVE 3
#define RESULT_NOT_ACTIVE (-0xFF)
#define RESULT_ERROR_LIMIT (-0xFE)
#define MASK_LOW24 0xFFFFFF
#define SHIFT_HIGH8 24
#define ARG_FULL_RANGE 0xFFFF

#define OFF_KIND 0x04
#define OFF_MODEL 0x0C
#define OFF_MODEL_INFO 0x10
#define OFF_VEC_A 0x60
#define OFF_VEC_B 0x6C
#define OFF_FLAGS_A8 0xA8
#define OFF_FLAGS_AC 0xAC
#define OFF_STACK_TOP 0xBC
#define OFF_SEL_C0 0xC0
#define OFF_SEL_C8 0xC8
#define OFF_TIME_DC 0xDC
#define OFF_COUNT_E4 0xE4
#define OFF_FIELD_E8 0xE8
#define OFF_FLAGS_100 0x100
#define OFF_FIELD_108 0x108
#define OFF_BYTE_120 0x120

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

extern u32 D_8006CB94;
extern s32 D_8005F180;

s32 func_8001C718(u8 *obj, s32 flag) {
    s32 out[2];
    s32 vec[6];
    u32 pad;
    u32 flags100;
    u32 sel;
    u32 hit;
    s32 r;
    u32 top;

    if (FIELD(obj, OFF_KIND, s32) != OBJ_STATE_ACTIVE) {
        return RESULT_NOT_ACTIVE;
    }
    if (obj == (u8 *)D_8005F38C) {
        func_800158E0();
    } else {
    }
    D_8006CB60 = (s32)obj;
    pad = D_8006CB94;
    hit = pad & PAD_ANY;
    if (hit != 0) {
        flags100 = FIELD(obj, OFF_FLAGS_100, u32);
        if (flags100 & F100_BIT_FORCE) {
            flag = 1;
        } else {
        }
        if (((FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_ALT) || (flags100 & F100_BIT_ALT)) && (pad & PAD_FORCE)) {
            hit = 1;
        } else {
            sel = MODEL_KIND(obj);
            switch (sel) {
            case 0x100: hit = D_8006CB94 & 0x20; break;
            case 0x200: hit = D_8006CB94 & 0x100; break;
            case 0x300:
            case 0x500:
            case 0x600: hit = D_8006CB94 & 0x80; break;
            case 0x400:
            case 0x900: hit = D_8006CB94 & 0x400; break;
            default: break;
            }
        }
    } else {
    }
    if (flag != 0 && hit != 0) {
        if ((FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_COUNTDOWN) && FIELD(obj, OFF_COUNT_E4, s32) != 0) {
            s32 n = FIELD(obj, OFF_COUNT_E4, s32) - 1;
            FIELD(obj, OFF_COUNT_E4, s32) = n;
            if (n == 0) {
                FIELD(obj, OFF_FLAGS_AC, u32) &= ~AC_BIT_COUNTDOWN;
            } else {
            }
            func_8001AA80(obj, out);
        } else {
            FIELD(obj, OFF_TIME_DC, u32) = D_8006CB64;
            if (FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_COPY_VEC) {
                vec[0] = FIELD(obj, OFF_VEC_A, s32);
                vec[1] = FIELD(obj, OFF_VEC_A + 4, s32);
                vec[2] = FIELD(obj, OFF_VEC_A + 8, s32);
                vec[3] = FIELD(obj, OFF_VEC_B, s32);
                vec[4] = FIELD(obj, OFF_VEC_B + 4, s32);
                vec[5] = FIELD(obj, OFF_VEC_B + 8, s32);
            } else {
            }
            if (FIELD(obj, OFF_SEL_C8, s32) != 0) {
                func_8001C620(obj, 0, ARG_FULL_RANGE);
                FIELD(obj, OFF_SEL_C0, s32) = FIELD(obj, OFF_SEL_C8, s32);
                r = func_8003A014(obj, 3, 0);
                if (r >= RESULT_ERROR_LIMIT) {
                    return r;
                } else {
                }
            } else {
            }
            top = FIELD(obj, OFF_STACK_TOP, u32);
            sel = *(u32 *)(top - 4);
            if (D_8006CB64 - (sel & MASK_LOW24) >= (sel >> SHIFT_HIGH8)) {
                FIELD(obj, OFF_STACK_TOP, u32) = top - 4;
                r = func_8003A014(obj, 4, 0);
                if (r >= RESULT_ERROR_LIMIT) {
                    return r;
                } else {
                }
            } else {
            }
            ((s32 (*)(void *, s32))func_8001BFDC)(obj, flag);
            func_8001D63C(obj, vec, out);
            FIELD(obj, OFF_FLAGS_A8, u32) &= ~A8_BIT_CLEAR;
        }
    } else {
        func_8001AA80(obj, out);
    }
    if (FIELD(obj, OFF_FIELD_E8, s32) != 0 && FIELD(obj, OFF_BYTE_120, u8) != 1
        && !(FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_SKIP) && (D_8006CB94 & PAD_FIRE)) {
        pad = D_8006CB94;
        if (((FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_ALT) || (FIELD(obj, OFF_FLAGS_100, u32) & F100_BIT_ALT))
            && (pad & PAD_FORCE2)) {
            hit = 1;
        } else {
            sel = MODEL_KIND(obj);
            hit = 0;
            switch (sel) {
            case 0x100:
                if (D_8006CB94 & PAD_MODE_A) {
                    if ((D_80060944 & 1) || D_8005F180 != 0 || FIELD(obj, OFF_FIELD_108, s32) != 4) {
                        hit = 1;
                    } else {
                    }
                } else {
                }
                break;
            case 0x200: hit = D_8006CB94 & 0x200; break;
            case 0x300:
            case 0x500:
            case 0x600: hit = D_8006CB94 & 0x40; break;
            case 0x400:
            case 0x900: hit = D_8006CB94 & 0x800; break;
            default: break;
            }
        }
        if (flag == 0 && !(D_8006CB94 & PAD_HOLD) && !(FIELD(obj, OFF_FLAGS_100, u32) & F100_BIT_FORCE)) {
            hit = 0;
        } else {
        }
        if (hit != 0) {
            func_8001ABF8(obj, out);
        } else {
        }
    } else {
    }
    D_8006CB60 = 0;
    return RESULT_NOT_ACTIVE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C718);
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

typedef struct Unk8001CC10Bank {
    u8 pad0[0x10];
    Unk8001CC10Tab *tab;
    s32 *code;
    u8 pad18[0x04];
    u16 *index;
    s32 *flags;
} Unk8001CC10Bank;

typedef struct Unk8001CC10Obj {
    u8 pad0[0xC];
    Unk8001CC10Bank *bank;
    u8 pad10[0x4C];
    s32 p5C;
    u8 pad60[0x48];
    s32 fA8;
    u8 padAC[0x04];
    s32 fB0;
    u8 padB4[0x08];
    s32 *stack;
    s32 pC0;
    u8 padC4[0x08];
    s32 pCC;
    u8 padD0[0x04];
    s32 fD4;
    u8 padD8[0x30];
    s32 f108;
    u8 pad10C[0x10];
    s32 f11C;
} Unk8001CC10Obj;

s32 func_8001CC10(s32 arg0, s32 arg1, s32 id, s32 n, void *arg4) {
#define self ((Unk8001CC10Obj *)arg0)
#define obj ((Unk8001CC10Obj *)arg1)
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
            self->fD4 = 1;
        }
        obj->p5C = (s32)self;
        if (obj->pCC != 0) {
            *obj->stack++ = id;
            *obj->stack++ = (s32)args;
            func_8001C620((u8 *)obj, PUSH_ARG_COUNT, SCRIPT_ARG_LIMIT);
            obj->pC0 = obj->pCC;
            r = func_8003A014(obj, 8, (s32)out);
            if (r != RESULT_PENDING) {
                if (r >= -0xFE) {
                    return r;
                } else {
                    s3 = out[0];
                }
            }
            if (self != 0) {
                self->fD4 = out[1];
            }
        }
        if (r >= -0xFE) {
            if ((id >> 8) < obj->bank->tab->count) {
                s3 = obj->bank->index[id >> 8];
            } else {
                s3 = SCRIPT_NONE;
            }
            if (self != 0) {
                self->fD4 = s3 != SCRIPT_NONE;
            }
            if (s3 & SCRIPT_RUNS) {
                obj->f11C = id;
                i = 0;
                if (n > 0) {
                    src = args;
                    do {
                        *obj->stack++ = *src;
                        i++;
                        src++;
                    } while (i < n);
                }
                func_8001C620((u8 *)obj, n, SCRIPT_ARG_LIMIT);
                obj->pC0 = (s32)(obj->bank->code + (s3 & SCRIPT_INDEX_MASK));
                return func_8003A014(obj, 3, 0);
            }
        }
        if (s3 == SCRIPT_NONE) {
            return RESULT_NONE;
        }
        fl = obj->fB0;
        if (id == ID_1800 || id == ID_2600 || id == ID_1900 || id == ID_2500) {
            fl &= ~FLAG_BIT2;
        }
        cur = D_8005F38C;
        mask = obj->bank->flags[s3 * 4];
        st = fl & mask;
        if ((s32)obj == cur) {
            if ((u32)(obj->f108 - 3) < 2U || obj->f108 == STATE_FIRST) {
                st = fl;
                st = (st | FLAG_FORCE) & mask;
            }
        }
        if (st == 0) {
            obj->f11C = id;
            if (id == ID_1800 || id == ID_1900) {
                obj->fA8 |= OBJ_FLAG_A8_SET;
            }
            func_8001C340(obj, s3, n, (s32 *)args);
            return RESULT_NONE;
        }
    }
    if (self != 0) {
        self->fD4 = 0;
    }
    return RESULT_NONE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001CC10);
#endif
