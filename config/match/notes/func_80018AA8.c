#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80018AA8 {
    s32 f0;
    u8 pad[0x44];
    struct Unk80018AA8 *next;
    struct Unk80018AA8 *child;
} Unk80018AA8;

typedef s32 (*Cb80018AA8)(Unk80018AA8 *, void *);

s32 func_80018AA8(Unk80018AA8 *arg0, Cb80018AA8 arg1, void *arg2) {
    s32 ret;
    Unk80018AA8 *p;
    Unk80018AA8 *nx;

    ret = arg1(arg0, arg2);
    if (ret == -0x19) {
        func_80019BBC((s32)arg0, 0);
        return -0x19;
    }
    p = arg0->child;
    while (p != 0 && p->f0 != 0) {
        nx = p->next;
        func_80018AA8(p, arg1, arg2);
        p = nx;
    }
    return ret;
}
