#include "common.h"
#include "sdk.h"
#include "game.h"

#define FIXED_SHIFT 4
#define MATRIX_ELEMS 9

void func_80018194(s32 *v, s16 *m, s32 *out);
void func_8001E780(s16 *arg0, s32 *arg1, s32 arg2);

void func_8001E8E4(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3, s32 *arg4) {
    s16 matrix[MATRIX_ELEMS];
    s32 vec[3];
    s32 rotated[3];

    func_8001E780(matrix, arg2, arg3);
    vec[0] = arg0[0] / (1 << FIXED_SHIFT);
    vec[1] = arg0[1] / (1 << FIXED_SHIFT);
    vec[2] = arg0[2] / (1 << FIXED_SHIFT);
    func_80018194(vec, matrix, rotated);
    rotated[0] <<= FIXED_SHIFT;
    rotated[1] <<= FIXED_SHIFT;
    rotated[2] <<= FIXED_SHIFT;
    arg4[0] = rotated[0] + arg1[0];
    arg4[1] = rotated[1] + arg1[1];
    arg4[2] = rotated[2] + arg1[2];
}
