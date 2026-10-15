#include "common.h"
#include "sdk.h"
#include "game.h"

void bzero(void *, s32);

void *func_80011764(s32 count, s32 size) {
    s32 total;
    void *p;

    total = size * count;
    p = (void *)func_8001148C(total);
    if (p != 0) {
        bzero(p, total);
    } else {
        return 0;
    }
    return p;
}
