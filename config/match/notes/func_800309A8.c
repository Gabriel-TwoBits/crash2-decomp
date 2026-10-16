#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 min[3];
    s32 max[3];
} Unk800309A8Box;

void func_80018194(s32 *, s16 *, s32 *);

void func_800309A8(Unk800309A8Box *out, Unk800309A8Box *in, s16 *m, s32 *trans) {
    s32 cz;
    s32 cy;
    s32 center[4];
    s32 mid[4];
    s32 ext[3][4];
    s32 extent;

    mid[0] = (in->min[0] + in->max[0]) >> 1;
    mid[1] = (in->min[1] + in->max[1]) >> 1;
    mid[2] = (in->min[2] + in->max[2]) >> 1;
    mid[0] /= 16;
    mid[1] /= 16;
    mid[2] /= 16;
    func_80018194(mid, m, center);
    center[0] <<= 4;
    center[1] <<= 4;
    center[2] <<= 4;
    center[0] += trans[0];
    center[1] += trans[1];
    center[2] += trans[2];

    extent = (in->max[0] - in->min[0]) >> 2;
    ext[0][0] = (extent * m[0]) >> 12;
    if (ext[0][0] < 0) {
        ext[0][0] = -ext[0][0];
    }
    ext[0][1] = (extent * m[3]) >> 12;
    if (ext[0][1] < 0) {
        ext[0][1] = -ext[0][1];
    }
    ext[0][2] = (extent * m[6]) >> 12;
    if (ext[0][2] < 0) {
        ext[0][2] = -ext[0][2];
    }
    extent = (in->max[1] - in->min[1]) >> 2;
    ext[1][0] = (extent * m[1]) >> 12;
    if (ext[1][0] < 0) {
        ext[1][0] = -ext[1][0];
    }
    ext[1][1] = (extent * m[4]) >> 12;
    if (ext[1][1] < 0) {
        ext[1][1] = -ext[1][1];
    }
    ext[1][2] = (extent * m[7]) >> 12;
    if (ext[1][2] < 0) {
        ext[1][2] = -ext[1][2];
    }
    extent = (in->max[2] - in->min[2]) >> 2;
    ext[2][0] = (extent * m[2]) >> 12;
    if (ext[2][0] < 0) {
        ext[2][0] = -ext[2][0];
    }
    ext[2][1] = (extent * m[5]) >> 12;
    if (ext[2][1] < 0) {
        ext[2][1] = -ext[2][1];
    }
    ext[2][2] = (extent * m[8]) >> 12;
    if (ext[2][2] < 0) {
        ext[2][2] = -ext[2][2];
    }

    extent = (ext[0][0] + ext[1][0] + ext[2][0]) * 2;
    out->max[0] = center[0] + extent;
    out->min[0] = center[0] - extent;
    cy = center[1];
    extent = (ext[0][1] + ext[1][1] + ext[2][1]) * 2;
    out->max[1] = cy + extent;
    out->min[1] = center[1] - extent;
    cz = center[2];
    extent = (ext[0][2] + ext[1][2] + ext[2][2]) * 2;
    out->max[2] = cz + extent;
    out->min[2] = center[2] - extent;
}
