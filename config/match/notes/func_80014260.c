#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 value;
    s16 pad;
    s16 kind;
} Unk80014148;
Unk80014148 *func_80014148(u32);

void *func_80014260(void *a0, s32 arg1, s32 arg2) {
    u32 *arg0 = a0;
    u32 *p;
    u32 v;
    Unk80014148 *e;

    p = arg0;
    if (!(arg1 & 2)) {
        v = *p;
        if (v == 0x6396347F) {
            return (void *)-0xA;
        }
        if (v & 1) {
            p = (u32 *)func_80014B44(v);
            *arg0 = (u32)p;
        } else {
            p = (u32 *)v;
        }
    }
    v = *p;
    if (!(v & 1)) {
        if (arg2 != 0) {
            e = func_80014148(v);
            if (e != 0) {
                func_80013FD8(*(s32 *)((u8 *)e + 0x14), arg1, arg2, p[1]);
            }
        }
        return (void *)*p;
    }
    func_80013FD8(v, arg1, arg2, p[1]);
    v = *p;
    if (v & 1) {
        v = 0;
    }
    return (void *)v;
}
