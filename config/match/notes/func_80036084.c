#include "common.h"
#include "sdk.h"

s32 D_8005F264 = 0;
extern s32 D_8006CC5C;

s32 func_80036084(s32 a0) {
    s32 *p = (s32 *)D_8005F264;
    s32 v = p[1] | a0;
    p[1] = v;
    D_8006CC5C = v;
    return 0;
}
