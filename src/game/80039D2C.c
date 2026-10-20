/* The handwritten code's equivalent C (docs/WORKFLOW.md, "The handwritten code"). Only the equivalent build compiles
 * it: configure.py leaves each function defined here out of that build's copy of asm/game/80039D2C.s, and links this
 * file after it. The default build assembles the handwritten code as it is. */
#include "common.h"
#include "game.h"

#ifndef NON_MATCHING
#error "the handwritten code's C is the equivalent build's only"
#endif

/* An angle's units in a turn, and the sine table's quarter wave (0x401 entries of round(4096 sin)) */
#define ANGLE_TURN 0x1000
#define ANGLE_MASK (ANGLE_TURN - 1)
#define ANGLE_QUARTER 0x400
#define ANGLE_HALF 0x800
#define ANGLE_THREE_QUARTERS 0xC00

extern s16 g_SinTable[];

/* The fraction bits Math_SqrMagnitude2 and Math_SqrMagnitude3 drop from their sums of squares */
#define SQR_MAGNITUDE2_SHIFT 13
#define SQR_MAGNITUDE3_SHIFT 20

/* func_80041A78 takes two bits of its value a step, from the top */
#define SQRT_STEPS 16
#define SQRT_STEP_BITS 2
#define SQRT_TOP_SHIFT 30

s32 Math_SqrMagnitude2(s32 x, s32 y) {
    u64 sum = (s64)x * x + (s64)y * y;

    return sum >> SQR_MAGNITUDE2_SHIFT;
}

s32 Math_SqrMagnitude3(s32 x, s32 y, s32 z) {
    u64 sum = (s64)x * x + (s64)y * y + (s64)z * z;

    return sum >> SQR_MAGNITUDE3_SHIFT;
}

s32 Math_Sin(s32 angle) {
    angle &= ANGLE_MASK;
    if (angle < ANGLE_QUARTER) {
        return g_SinTable[angle];
    } else if (angle < ANGLE_HALF) {
        return g_SinTable[ANGLE_HALF - angle];
    } else if (angle < ANGLE_THREE_QUARTERS) {
        return -g_SinTable[angle - ANGLE_HALF];
    } else {
        return -g_SinTable[ANGLE_TURN - angle];
    }
}

s32 Math_Cos(s32 angle) {
    angle &= ANGLE_MASK;
    if (angle < ANGLE_QUARTER) {
        return g_SinTable[ANGLE_QUARTER - angle];
    } else if (angle < ANGLE_HALF) {
        return -g_SinTable[angle - ANGLE_QUARTER];
    } else if (angle < ANGLE_THREE_QUARTERS) {
        return -g_SinTable[ANGLE_THREE_QUARTERS - angle];
    } else {
        return g_SinTable[angle - ANGLE_THREE_QUARTERS];
    }
}

void func_8003DD0C(void) {
}

/* The integer square root, digit by digit in base 4 */
s32 func_80041A78(s32 value) {
    u32 bits = value;
    s32 root = 0;
    s32 remainder = 0;
    s32 trial;
    s32 step;

    for (step = 0; step < SQRT_STEPS; step++) {
        remainder |= bits >> SQRT_TOP_SHIFT;
        trial = remainder - ((root << SQRT_STEP_BITS) + 1);
        root <<= 1;
        if (trial >= 0) {
            root++;
            remainder = trial;
        } else {
            /* The digit is 0: the remainder stays */
        }
        bits <<= SQRT_STEP_BITS;
        remainder <<= SQRT_STEP_BITS;
    }
    return root;
}
