#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { char pad[0x100]; s32 flags; } T;

T *func_80019074(T *a, s32 m) {
    if (a->flags & m) {
        return a;
    }
    return 0;
}
