#include "common.h"
#include "sdk.h"

extern void func_800163B4();

void func_800163C4(s32 *arg0) {
    arg0[2] = arg0[1];
    func_800163B4();
}
