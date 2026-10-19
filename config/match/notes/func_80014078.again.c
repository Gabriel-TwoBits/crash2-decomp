#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_80067FC4;

typedef struct Unk80014078 {
    s8 pad0[6];
    s16 f6;
    s8 pad8[7];
    s8 fF;
} Unk80014078;

static inline s32 func_80014078_lookup(s32 id) {
    return D_80067FC4[id >> 1];
}

s32 func_80014078(s32 arg0, s32 arg1) {
    Unk80014078 *p;
    s32 i;

    p = (Unk80014078 *)func_80014078_lookup(arg0);
    if ((s32)p >= -0xFE) {
        return 0;
    }
    if ((s32)p == -0xFF || p->f6 == 3 || p->f6 == 5) {
        return 1;
    }
    if (arg1 < 0) {
        func_80013E7C((s8 *)p);
    } else {
        for (i = 0; i < arg1; i++) {
            Nsf_DropPageRef(p);
        }
    }
    return p->fF + 1;
}
