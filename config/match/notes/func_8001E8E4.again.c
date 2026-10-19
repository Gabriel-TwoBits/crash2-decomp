#include "common.h"
#include "sdk.h"
#include "game.h"

void func_8001E8E4(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3, s32 *arg4) {
    s16 mtx[16];
    s32 out[4];
    s32 in[4];

    Math_MatrixFromAngles(mtx, arg2, arg3);
    in[0] = arg0[0] / 16;
    in[1] = arg0[1] / 16;
    in[2] = arg0[2] / 16;
    func_80018194(in, mtx, out);
    out[0] *= 16;
    out[1] *= 16;
    out[2] *= 16;
    /* A second set of arg4, folded away, drops its REG_EQUIV to the stack slot: the retail prologue order. */
    arg4 = (s32 *)((s32)arg4 + (s32)arg1 - (s32)arg1);
    arg4[0] = out[0] + arg1[0];
    arg4[1] = out[1] + arg1[1];
    arg4[2] = out[2] + arg1[2];
}
