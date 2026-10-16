#include "common.h"
#include "sdk.h"
#include "game.h"

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

extern Unk8001CC10Obj *D_8005F38C;

void func_8001C340(Unk8001CC10Obj *, u16, s32, void *);
void func_8001C620(Unk8001CC10Obj *, s32, s32);
s32 func_8003A014(Unk8001CC10Obj *, s32, s32 *);

s32 func_8001CC10(s32 arg0, s32 arg1, s32 id, s32 n, void *arg4) {
#define self ((Unk8001CC10Obj *)arg0)
#define obj ((Unk8001CC10Obj *)arg1)
#define args ((s32 *)arg4)
    s32 out[20];
    s32 r;
    s32 s3;
    s32 i;
    s32 *src;
    s32 fl;
    s32 mask;
    s32 st;

    r = -0x1A;
    if (obj != 0) {
        if (self != 0) {
            self->fD4 = 1;
        }
        obj->p5C = (s32)self;
        if (obj->pCC != 0) {
            *obj->stack++ = id;
            *obj->stack++ = (s32)args;
            func_8001C620(obj, 2, 0xFFFF);
            obj->pC0 = obj->pCC;
            r = func_8003A014(obj, 8, out);
            if (r != -0x1A) {
                if (r >= -0xFE) {
                    return r;
                }
                s3 = out[0];
            }
            if (self != 0) {
                self->fD4 = out[1];
            }
        }
        if (r >= -0xFE) {
            if ((id >> 8) < obj->bank->tab->count) {
                s3 = obj->bank->index[id >> 8];
            } else {
                s3 = 0xFF;
            }
            if (self != 0) {
                self->fD4 = s3 != 0xFF;
            }
            if (s3 & 0x8000) {
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
                func_8001C620(obj, n, 0xFFFF);
                obj->pC0 = (s32)(obj->bank->code + (s3 & 0x3FFF));
                return func_8003A014(obj, 3, 0);
            }
        }
        if (s3 == 0xFF) {
            return -0xFF;
        }
        fl = obj->fB0;
        if (id == 0x1800 || id == 0x2600 || id == 0x1900 || id == 0x2500) {
            fl &= ~2;
        }
        mask = obj->bank->flags[s3 * 4];
        st = fl & mask;
        if (obj == D_8005F38C) {
            if ((u32)(obj->f108 - 3) < 2U || obj->f108 == 2) {
                st = (fl | 0x1002) & mask;
            }
        }
        if (st == 0) {
            obj->f11C = id;
            if (id == 0x1800 || id == 0x1900) {
                obj->fA8 |= 0x10000;
            }
            func_8001C340(obj, s3, n, args);
            return -0xFF;
        }
    }
    if (self != 0) {
        self->fD4 = 0;
    }
    return -0xFF;
}
