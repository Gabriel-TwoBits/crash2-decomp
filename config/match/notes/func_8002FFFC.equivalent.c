#include "common.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Unk8002FFFCVertex;

/* Point where a face edge crosses the plane; flag is 1 when it does. */
typedef struct {
    s32 x;
    s32 flag;
    s32 z;
} Unk8002FFFCCross;

#define FACE_COUNT 6
#define SIDE_FACE_COUNT 4
#define FACE_VERTS 4
#define FACE_EDGE_SLOTS 3
#define EDGE_COUNT 12
#define USED_BYTES 16
#define FIXED_SHIFT 8

/* One frame: the table-indexed edge and flag accesses stay inside it. */
typedef struct {
    Unk8002FFFCVertex *v[FACE_VERTS];
    Unk8002FFFCCross edge[EDGE_COUNT];
    s8 used[USED_BYTES];
} Unk8002FFFCFrame;

extern s32 D_8005BBB8[];
extern s8 D_8005BC98[];

s32 func_8002FFFC(s32 arg0, Unk8002FFFCVertex *arg1, Unk8002FFFCVertex *arg2) {
    Unk8002FFFCFrame fr;
    Unk8002FFFCCross *first;
    Unk8002FFFCVertex *touch;
    Unk8002FFFCVertex *p;
    Unk8002FFFCVertex *q;
    Unk8002FFFCVertex *cur;
    s32 count;
    s32 f;
    s32 k;
    s32 t;
    s32 prev;
    s32 py;
    s32 qy;
    s32 e;
    s8 *flag;

    for (f = 0; f < SIDE_FACE_COUNT; f++) {
        for (k = 0; k < FACE_VERTS; k++) {
            fr.v[k] = arg1 + D_8005BBB8[f * FACE_VERTS + k];
        }
        for (k = 0; k < FACE_EDGE_SLOTS; k++) {
            p = fr.v[k];
            e = f * FACE_EDGE_SLOTS + k;
            fr.edge[e].flag = 0;
            q = fr.v[k + 1];
            py = p->y;
            qy = q->y;
            if (qy >= py) {
                if (py < arg0 && arg0 < qy) {
                    t = ((arg0 - py) << FIXED_SHIFT) / (qy - py);
                    fr.edge[e].x = (((q->x - p->x) * t) >> FIXED_SHIFT) + p->x;
                    fr.edge[e].flag = 1;
                    fr.edge[e].z = (((q->z - p->z) * t) >> FIXED_SHIFT) + p->z;
                } else {
                    /* edge does not cross the plane */
                }
            } else if (qy < arg0 && arg0 < py) {
                t = ((arg0 - qy) << FIXED_SHIFT) / (py - qy);
                fr.edge[e].x = (((p->x - q->x) * t) >> FIXED_SHIFT) + q->x;
                fr.edge[e].flag = 1;
                fr.edge[e].z = (((p->z - q->z) * t) >> FIXED_SHIFT) + q->z;
            } else {
                /* edge does not cross the plane */
            }
        }
    }

    count = 0;
    fr.used[0] = 0;
    fr.used[1] = 0;
    fr.used[2] = 0;
    fr.used[3] = 0;
    fr.used[4] = 0;
    fr.used[5] = 0;
    fr.used[6] = 0;
    fr.used[7] = 0;
    fr.used[8] = 0;
    fr.used[9] = 0;
    fr.used[10] = 0;
    fr.used[11] = 0;
    for (f = 0; f < FACE_COUNT; f++) {
        for (k = 0; k < FACE_VERTS; k++) {
            fr.v[k] = arg1 + D_8005BBB8[f * FACE_VERTS + k];
        }
        first = 0;
        touch = 0;
        for (k = 0; k < FACE_VERTS; k++) {
            e = D_8005BC98[f * FACE_VERTS + k];
            cur = fr.v[k];
            if (fr.edge[e].flag == 1) {
                if (first == 0) {
                    first = &fr.edge[e];
                    continue;
                }
                arg2[0].x = first->x;
                arg2[0].y = arg0;
                arg2[0].z = first->z;
                arg2[1].x = fr.edge[e].x;
                arg2[1].y = arg0;
                arg2[1].z = fr.edge[e].z;
                count++;
                arg2 += 2;
                goto next_face;
            } else if (cur->y == arg0) {
                if (touch == 0) {
                    touch = cur;
                    continue;
                }
                prev = (k - 1) & 3;
                if (fr.v[prev]->y == arg0) {
                    flag = &fr.used[D_8005BC98[f * FACE_VERTS + prev]];
                    if (*flag == 0) {
                        *flag = 1;
                        goto emit_touch;
                    } else {
                        /* edge already emitted by an earlier face */
                    }
                } else {
emit_touch:
                    arg2[0] = *touch;
                    arg2[1] = *cur;
                    arg2 += 2;
                    count++;
                }
                goto next_face;
            } else {
                /* vertex off the plane */
            }
        }
        if (first == 0) {
            if (touch != 0) {
                arg2[0] = *touch;
                arg2[1] = *touch;
                return 1;
            } else {
                continue;
            }
        } else {
            arg2[0].x = first->x;
            arg2[0].y = arg0;
            arg2[0].z = first->z;
            count++;
            arg2[1] = *touch;
            arg2 += 2;
        }
next_face:;
    }
    return count;
}
