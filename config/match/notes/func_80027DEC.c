#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80030D74(void *, s32);
extern char *D_800608CC;

typedef struct {
    char pad[0x190];
    s32 count;
    s32 ent[8];
    s32 flags[8];
} Unk80027DEC;

void *func_80027DEC(char *arg0, s32 arg1) {
    Unk80027DEC *p;
    void *r;
    char **global;
    s32 i;

    p = *(Unk80027DEC **)(arg0 + 0x10);
    global = &D_800608CC;
    for (i = p->count - 1; i >= 0; i--) {
        if (p->flags[i] & 4) {
            r = func_80014260(&p->ent[i], 1, 0);
            if (func_80030D74(r, arg1)) {
                return r;
            }
        }
    }
    if (arg0 != *global) {
        p = *(Unk80027DEC **)(D_800608CC + 0x10);
        for (i = p->count - 1; i >= 0; i--) {
            if (p->flags[i] & 4) {
                r = func_80014260(&p->ent[i], 1, 0);
                if (func_80030D74(r, arg1)) {
                    return r;
                }
            }
        }
    }
    return (void *)-0x17;
}
