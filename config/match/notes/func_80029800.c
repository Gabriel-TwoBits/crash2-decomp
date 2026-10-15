#include "common.h"
#include "sdk.h"
#include "game.h"

void func_8003AE54(void *, s32, u16 *);
void func_8003B24C(void *, s32, u16 *);
void func_8003B940(void *, void *, u16);

u16 *func_80029800(u16 *arg0, s32 arg1, u16 *arg2, s32 arg3) {
    if (!(arg0[1] & 1)) {
        func_8003B940(arg0 + 2, arg2 + 2, arg0[0]);
        *arg2 = arg0[0];
        return arg2;
    }
    if (arg3 == 0) {
        func_8003B24C(arg0, arg1, arg2);
        return arg2;
    }
    func_8003AE54(arg0, arg1, arg2);
    return arg2;
}
