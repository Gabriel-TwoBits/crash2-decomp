#include "common.h"
#include "sdk.h"
#include "game.h"

#define OBJ_FLAGS 0xAC
#define OBJ_LINK 0x44
#define OBJ_ID 0x138
#define OBJ_FLAG_ACTIVE 0x04000000
#define MODE_AXIS_Y 1
#define MODE_AXIS_Z 2
#define MODE_LINKED 4
#define MODE_MASK 3
#define Y_OFFSET 0x61A8
#define SCORE_MIN (-0x3B9AC9FF)
#define LIST_COUNT 0x190
#define LIST_ENTRIES 0x194
#define LIST_FLAGS 0x1B4
#define LIST_FLAG_CHECK 4
#define BOX_FRAME_BYTES 8
#define BOX_ALT_BYTES 0x2C
#define SLOT_KIND_MASK 7
#define SLOT_KIND_ALT 1
#define SLOT_TABLE_BYTES 0x290
#define NO_ID 0xFFFF
#define POS_X 0
#define POS_Y 1
#define POS_Z 2
#define BOX_SCALE_SHIFT 8
#define ID_MASK 0x3F0
#define ID_SHIFT 4
#define ID_BASE 0x30
#define ID_RANGE 0x10
#define LEVEL_OFFSET 0x10

extern s32 D_8006CB6C;

typedef void *(*Unk8003DA00Fn)(s32, void *, s32, s32, s32, s32, s32, s32, s32, s32);

void func_80028FE0(s32 arg0, s32 *arg1, s32 arg2, s32 arg3) {
    s32 start[3];
    s32 pos[3];
    s32 best;
    s32 hitMode;
    s32 i;
    s32 id;
    s32 val;
    s32 axis;
    s32 *slots;
    s32 *entry;
    s32 *box;
    s32 *rec;
    s32 *found;
    char *level;
    char *hit;
    char *cand;
    char *pend;

    hit = 0;
    pend = 0;
    hitMode = 0;
    best = SCORE_MIN;
    if (!(*(s32 *)(arg0 + OBJ_FLAGS) & OBJ_FLAG_ACTIVE)) {
        return;
    } else if ((arg2 & MODE_LINKED) &&
               !(*(s32 *)(*(s32 *)(arg0 + OBJ_LINK) + OBJ_FLAGS) & OBJ_FLAG_ACTIVE)) {
        return;
    } else {
        /* active: continue */
    }
    pos[POS_X] = arg1[0];
    pos[POS_Y] = arg1[1] + Y_OFFSET;
    pos[POS_Z] = arg1[2];
    level = *(char **)(D_800608CC + LEVEL_OFFSET);
    start[POS_X] = pos[POS_X];
    start[POS_Y] = pos[POS_Y];
    start[POS_Z] = pos[POS_Z];

    for (;;) {
        found = 0;
        for (i = 0; i < *(s32 *)(level + LIST_COUNT); i++) {
            if (*(s32 *)(level + LIST_FLAGS + i * 4) & LIST_FLAG_CHECK) {
                s32 *r = func_80014260(level + LIST_ENTRIES + i * 4, 1, 0);
                if (func_80030D74(r, pos) != 0) {
                    found = r;
                    break;
                }
            }
        }
        if (found != 0) {
            rec = (s32 *)found[5];
            hit = ((Unk8003DA00Fn)func_8003DA00)((s32)rec, pos, arg2 & MODE_MASK, arg3,
                rec[0] << BOX_SCALE_SHIFT, rec[1] << BOX_SCALE_SHIFT, rec[2] << BOX_SCALE_SHIFT,
                rec[3] << BOX_SCALE_SHIFT, rec[4] << BOX_SCALE_SHIFT, rec[5] << BOX_SCALE_SHIFT);
        }
        if (i == *(s32 *)(level + LIST_COUNT)) {
            hit = 0;
            break;
        } else if (hit == 0) {
            continue;
        } else {
            break;
        }
    }

    if (D_8006CB6C > 0) {
        slots = (s32 *)((char *)&D_8006CB6C + SLOT_TABLE_BYTES);
        for (i = 0; i < D_8006CB6C; i++, slots++) {
            entry = (s32 *)*slots;
            cand = (char *)entry[1];
            if (cand == (char *)arg0 || *(s32 *)(cand + OBJ_ID) == NO_ID) {
                continue;
            }
            box = (s32 *)((char *)entry + BOX_FRAME_BYTES);
            if ((entry[0] & SLOT_KIND_MASK) == SLOT_KIND_ALT) {
                box = (s32 *)((char *)entry + BOX_ALT_BYTES);
            }
            if (arg2 & MODE_AXIS_Y) {
                if (start[POS_X] < box[0] || box[3] < start[POS_X] ||
                    start[POS_Z] < box[2] || box[5] < start[POS_Z]) {
                    continue;
                }
                val = box[4];
                axis = POS_Y;
            } else if (arg2 & MODE_AXIS_Z) {
                if (start[POS_X] < box[0] || box[3] < start[POS_X] ||
                    start[POS_Y] < box[1] || box[4] < start[POS_Y]) {
                    continue;
                }
                val = box[5];
                axis = POS_Z;
            } else {
                continue;
            }
            if (val >= pos[axis] && pos[axis] >= box[axis]) {
                hit = cand;
                hitMode = 1;
                pos[axis] = box[axis];
                break;
            }
            if (best < val && start[axis] >= val) {
                pend = cand;
                best = val;
            }
        }
        if (hitMode == 0 && pend != 0) {
            if (arg2 & MODE_AXIS_Y) {
                axis = POS_Y;
            } else if (arg2 & MODE_AXIS_Z) {
                axis = POS_Z;
            } else {
                axis = -1;
            }
            if (axis >= 0 && (hit == 0 || best >= pos[axis])) {
                pos[axis] = best;
                hit = pend;
                hitMode = 1;
            }
        }
    }

    id = -1;
    if (hit != 0) {
        if (hitMode != 0) {
            id = *(s32 *)(hit + OBJ_ID);
            if (id < 0) {
                id = -id;
                arg3 = 0;
            }
        } else {
            id = ((u32)hit & ID_MASK) >> ID_SHIFT;
            if ((u32)(id - ID_BASE) >= ID_RANGE) {
                id = -1;
            }
        }
    }
    func_800289C4((s16 *)arg0, id, arg3);
    if (arg2 & MODE_LINKED) {
        func_800289C4(*(s16 **)(arg0 + OBJ_LINK), id, arg3);
    }
}
