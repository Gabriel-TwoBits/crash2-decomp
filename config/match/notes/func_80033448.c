#include "common.h"
#include "sdk.h"
#include "game.h"

void func_80013E10(void *arg0);
void func_80051078(s32 arg0, s32 arg1);

void func_80033448(void *arg0) {
    do { func_80051078(0, 1 << ((u16 *)arg0)[3]); } while (0);
    ((s32 *)arg0)[1] &= ~8;
    if (((void **)arg0)[6] != 0) {
        func_80013E10(((void **)arg0)[6]);
    }
}
