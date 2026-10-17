#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_80063450;

#define SCALE_CAP 0x66
#define SCALE_SHIFT 10
#define SCALE_ROUND 0x3FF
#define ANGLE_MASK 0xFFF
#define ANGLE_SIGN_SHIFT 20
#define FLAG_REACHED 0x800
#define SCALE_OFFSET 0x14
#define FLAGS_OFFSET 0xA8

s32 func_8001E544(s32 arg0, s32 arg1, s32 arg2, void *arg3) {
    u32 scale;
    s32 product;
    s32 step;
    s32 diff;
    s32 dist;
    s32 snap;

    scale = *(u32 *)((u8 *)D_80063450 + SCALE_OFFSET);
    if (scale > SCALE_CAP) {
        scale = SCALE_CAP;
    }
    product = arg2 * scale;
    if (product < 0) {
        product += SCALE_ROUND;
    }
    step = product >> SCALE_SHIFT;

    diff = (s32)((arg1 - arg0) << ANGLE_SIGN_SHIFT) >> ANGLE_SIGN_SHIFT;
    dist = diff;
    if (diff < 0) {
        dist = -diff;
    }

    if (diff == 0) {
        snap = 1;
    } else if (step >= 0) {
        snap = dist < step && (step ^ diff) >= 0;
    } else {
        snap = dist < -step && (step ^ diff) >= 0;
    }

    if (snap) {
        if (arg3 != 0) {
            *(s32 *)((u8 *)arg3 + FLAGS_OFFSET) |= FLAG_REACHED;
        }
        return arg1;
    } else {
        return (arg0 + step) & ANGLE_MASK;
    }
}
