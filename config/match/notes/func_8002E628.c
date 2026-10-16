#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk8002E628 {
    s32 lo;
    s32 hi;
    s32 *data;
} Unk8002E628;

s32 *func_8002E628(s32 *arg0, s32 arg1, s32 arg2, Unk8002E628 *arg3) {
    s32 q;

    if (arg3->data != 0) {
        do {
            if (arg1 >= arg3->lo && arg3->hi >= arg1) {
                q = (arg1 - arg3->lo) / arg2;
                *arg0 = arg3->lo + q * arg2;
                return arg3->data + (q >> 8);
            }
            arg3++;
        } while (arg3->data != 0);
    }
    return 0;
}
