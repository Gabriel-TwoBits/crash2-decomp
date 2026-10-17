#include "common.h"
#include "sdk.h"
#include "game.h"

#define FIXED_SHIFT 4
#define POS_OFFSET 0x60
#define MATRIX_OFFSET 8

void func_8001E9EC(void *arg0, void *arg1, void *arg2) {
    s32 *in = (s32 *)arg1;
    s32 *out = (s32 *)arg2;
    s32 *base = (s32 *)((u8 *)arg0 + POS_OFFSET);
    s16 *matrix = *(s16 **)((u8 *)arg0 + MATRIX_OFFSET);
    s32 vec[3];
    s32 res[3];

    vec[0] = in[0] / (1 << FIXED_SHIFT);
    vec[1] = in[1] / (1 << FIXED_SHIFT);
    vec[2] = in[2] / (1 << FIXED_SHIFT);
    func_80018194(vec, matrix, res);
    out[0] = (res[0] << FIXED_SHIFT) + base[0];
    out[1] = (res[1] << FIXED_SHIFT) + base[1];
    out[2] = (res[2] << FIXED_SHIFT) + base[2];
}
