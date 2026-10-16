#include "common.h"
#include "sdk.h"
#include "game.h"

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
