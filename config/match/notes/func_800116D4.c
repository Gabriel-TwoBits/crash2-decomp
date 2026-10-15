#include "common.h"
#include "sdk.h"
#include "game.h"

void bcopy(s32, s32, s32);
s32 func_800112A4(s32, s32);

s32 func_800116D4(s32 arg0, s32 arg1) {
    s32 r;

    if (arg0 == 0) {
        r = func_8001148C(arg1);
    } else {
        if (arg1 == 0) {
            func_800116C0(arg0);
        } else {
            r = func_800112A4(arg0, arg1);
            if (r == 0) {
                r = func_8001148C(arg1);
                if (r != 0) {
                    bcopy(arg0, r, arg1);
                    func_800116C0(arg0);
                }
            }
        }
    }
    return r;
}
