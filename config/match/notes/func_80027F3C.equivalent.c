#include "common.h"
#include "sdk.h"
#include "game.h"

#define AXES 3
#define NODE_LEAF_FLAG 1
#define NODE_OFFSET_MASK 0xFFFF
#define SPLIT_DEPTHS_OFFSET 0x1E
#define FIXED_SHIFT 8
#define FIXED_ONE 0x100
#define S32_MAX_VALUE 0x7FFFFFFF
#define S32_MIN_VALUE (-0x7FFFFFFF - 1)
#define AXIS_NONE (-1)
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

#define ABS_S32(v) (((v) < 0) ? -(v) : (v))

s32 func_80027F3C(void *arg0, u16 arg1, s32 *arg2, void *arg3, void *arg4, s32 arg5) {
    s32 *box = arg2;
    s32 *pos = (s32 *)arg3;
    s32 *vel = (s32 *)arg4;
    s16 *splitDepth = (s16 *)((u8 *)arg0 + SPLIT_DEPTHS_OFFSET);
    u16 *node;
    s32 edge[AXES];
    s32 half[AXES];
    s32 child[AXES * 2];
    s32 split[AXES];
    s32 best;
    s32 axis;
    s32 quot;
    s32 i;
    s32 ix;
    s32 iy;
    s32 iz;
    s32 nx;
    s32 ny;
    s32 nz;
    s32 index;

    if ((arg1 & NODE_LEAF_FLAG) || (arg1 & NODE_OFFSET_MASK) == 0) {
        node = (u16 *)(arg1 & NODE_OFFSET_MASK);
    } else {
        node = (u16 *)((u8 *)arg0 + (arg1 & NODE_OFFSET_MASK));
    }

    if (!(arg1 & NODE_LEAF_FLAG) && (arg1 & NODE_OFFSET_MASK) != 0) {
        for (i = 0; i < AXES; i++) {
            split[i] = arg5 < splitDepth[i];
            if (split[i]) {
                half[i] = box[AXES + i] / 2;
            } else {
                half[i] = box[AXES + i];
            }
        }
        nx = split[0] ? 2 : 1;
        ny = split[1] ? 2 : 1;
        nz = split[2] ? 2 : 1;
        index = 0;
        for (ix = 0; ix < nx; ix++) {
            for (iy = 0; iy < ny; iy++) {
                for (iz = 0; iz < nz; iz++) {
                    s32 hit;

                    child[0] = box[0];
                    if (ix != 0) {
                        child[0] += half[0];
                    }
                    child[1] = box[1];
                    if (iy != 0) {
                        child[1] += half[1];
                    }
                    child[2] = box[2];
                    if (iz != 0) {
                        child[2] += half[2];
                    }
                    child[3] = half[0];
                    child[4] = half[1];
                    child[5] = half[2];
                    index++;
                    if (child[0] < pos[0] && child[1] < pos[1] && child[2] < pos[2]
                        && child[0] + half[0] >= pos[0] && child[1] + half[1] >= pos[1]
                        && child[2] + half[2] >= pos[2]) {
                        hit = func_80027F3C(arg0, node[index - 1], child, arg3, arg4, arg5 + 1);
                        if (hit != 0) {
                            return hit;
                        }
                    } else {
                        /* box does not reach the point */
                    }
                }
            }
        }
        return 0;
    } else if (node == 0) {
        /* free path: advance to the first face hit */
        for (i = 0; i < AXES; i++) {
            if (vel[i] > 0) {
                edge[i] = box[i] + box[AXES + i];
            } else {
                edge[i] = box[i];
            }
        }
        best = S32_MAX_VALUE;
        for (i = 0; i < AXES; i++) {
            if (vel[i] != 0) {
                quot = ((edge[i] - pos[i]) << FIXED_SHIFT) / vel[i];
                if (quot < best) {
                    best = quot;
                }
            } else {
                /* no motion on this axis */
            }
        }
        best += FIXED_ONE;
        for (i = 0; i < AXES; i++) {
            pos[i] += (best * vel[i]) >> FIXED_SHIFT;
        }
        return (s32)node;
    } else {
        /* blocked path: undo one step on the axis that leaves the box last */
        for (i = 0; i < AXES; i++) {
            if (vel[i] < 0) {
                edge[i] = box[i] + box[AXES + i];
            } else {
                edge[i] = box[i];
            }
        }
        best = S32_MIN_VALUE;
        axis = AXIS_NONE;
        if (vel[0] != 0) {
            quot = ((edge[0] - (pos[0] - vel[0])) << FIXED_SHIFT) / vel[0];
            if (best < quot) {
                best = quot;
                axis = AXIS_X;
            }
        }
        if (vel[2] != 0) {
            quot = ((edge[2] - (pos[2] - vel[2])) << FIXED_SHIFT) / vel[2];
            if (best < quot || ABS_S32(vel[0]) < ABS_S32(vel[2])) {
                best = quot;
                axis = AXIS_Z;
            }
        }
        if (vel[1] != 0) {
            quot = ((edge[1] - (pos[1] - vel[1])) << FIXED_SHIFT) / vel[1];
            if (best < quot) {
                best = quot;
                axis = AXIS_Y;
            } else if (axis == AXIS_X) {
                if (ABS_S32(vel[0]) < ABS_S32(vel[1])) {
                    best = quot;
                    axis = AXIS_Y;
                }
            } else if (axis == AXIS_Z) {
                if (ABS_S32(vel[2]) < ABS_S32(vel[1])) {
                    best = quot;
                    axis = AXIS_Y;
                }
            } else {
                /* no axis chosen yet */
            }
        }
        if (axis == AXIS_X || axis == AXIS_Y || axis == AXIS_Z) {
            vel[axis] = -vel[axis];
            pos[axis] = edge[axis];
        } else {
            /* no axis chosen: position and velocity stay */
        }
        return (s32)node;
    }
}
