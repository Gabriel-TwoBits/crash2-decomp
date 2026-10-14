#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 value;
    s16 pad;
    s16 kind;
} Entry;

Entry *func_80014148();

s32 func_80014220(void) {
    Entry *e;

    e = func_80014148();
    if (e && e->kind == 1) {
        return e->value;
    }
    return 0;
}
