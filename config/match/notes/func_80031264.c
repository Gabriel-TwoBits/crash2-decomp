#include "common.h"
#include "sdk.h"

/* libgte */
long ratan2(long y, long x);

/* Angle (12-bit, 0x1000 per turn) from arg0 to arg1 on the plane of words 0 and 2 */
s32 func_80031264(s32 *arg0, s32 *arg1) {
    return ratan2(arg1[0] - arg0[0], arg1[2] - arg0[2]) & 0xFFF;
}
