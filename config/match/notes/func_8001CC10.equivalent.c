#include "common.h"
#include "sdk.h"
#include "game.h"

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
