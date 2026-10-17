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
#define BOX_SCALE_SHIFT 8
#define ID_MASK 0x3F0
#define ID_SHIFT 4
#define ID_BASE 0x30
#define ID_RANGE 0x10
#define LEVEL_OFFSET 0x10
#define WORD_BYTES 4

extern s32 D_8006CB6C;

typedef void *(*Unk8003DA00Fn)(s32, void *, s32, s32, s32, s32, s32, s32, s32, s32);

void func_80028FE0(s32 arg0, s32 *arg1, s32 arg2, s32 arg3) {
    s32 start[3];
    s32 pos[3];
    s32 bestTop;
    s32 hitIsBox;
    s32 i;
    s32 id;
    s32 val;
    s32 count;
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
    hitIsBox = 0;
    bestTop = SCORE_MIN;
    if (!(*(s32 *)(arg0 + OBJ_FLAGS) & OBJ_FLAG_ACTIVE)) {
        return;
    } else if ((arg2 & MODE_LINKED) &&
               !(*(s32 *)(*(s32 *)(arg0 + OBJ_LINK) + OBJ_FLAGS) & OBJ_FLAG_ACTIVE)) {
        return;
    } else {
        /* active: continue */
    }
    pos[0] = arg1[0];
    pos[1] = arg1[1];
    pos[2] = arg1[2];
    pos[1] += Y_OFFSET;
    level = *(char **)(D_800608CC + LEVEL_OFFSET);
    start[0] = pos[0];
    start[1] = pos[1];
    start[2] = pos[2];

    do {
        i = 0;
        found = 0;
        if (*(s32 *)(level + LIST_COUNT) > 0) {
            do {
                if (*(s32 *)(level + LIST_FLAGS + i * WORD_BYTES) & LIST_FLAG_CHECK) {
                    found = func_80014260(level + LIST_ENTRIES + i * WORD_BYTES, 1, 0);
                    if (func_80030D74(found, pos) != 0) {
                        rec = (s32 *)found[5];
                        hit = ((Unk8003DA00Fn)func_8003DA00)((s32)rec, pos, arg2 & MODE_MASK, arg3,
                            rec[0] << BOX_SCALE_SHIFT, rec[1] << BOX_SCALE_SHIFT, rec[2] << BOX_SCALE_SHIFT,
                            rec[3] << BOX_SCALE_SHIFT, rec[4] << BOX_SCALE_SHIFT, rec[5] << BOX_SCALE_SHIFT);
                        break;
                    }
                }
                i++;
            } while (i < *(s32 *)(level + LIST_COUNT));
        }
        count = *(s32 *)(level + LIST_COUNT);
        if (i == count) {
            hit = 0;
            break;
        } else {
            /* hit found: retry while the result is null */
        }
    } while (hit == 0);

    if (D_8006CB6C > 0) {
        count = D_8006CB6C;
        slots = (s32 *)((char *)&D_8006CB6C + SLOT_TABLE_BYTES);
        for (i = 0; i < count; i++, slots++) {
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
                val = box[4];
                if (start[0] < box[0] || box[3] < start[0] ||
                    start[2] < box[2] || box[5] < start[2]) {
                    continue;
                }
                if (val >= pos[1] && pos[1] >= box[1]) {
                    hit = cand;
                    hitIsBox = 1;
                    pos[1] = box[1];
                    break;
                }
                if (bestTop < val && start[1] >= val) {
                    pend = cand;
                    bestTop = val;
                }
            } else if (arg2 & MODE_AXIS_Z) {
                val = box[5];
                if (start[0] < box[0] || box[3] < start[0] ||
                    start[1] < box[1] || box[4] < start[1]) {
                    continue;
                }
                if (val >= pos[2] && pos[2] >= box[2]) {
                    hit = cand;
                    hitIsBox = 1;
                    pos[2] = box[2];
                    break;
                }
                if (bestTop < val && start[2] >= val) {
                    pend = cand;
                    bestTop = val;
                }
            } else {
                continue;
            }
        }
        if (!hitIsBox && pend != 0) {
            if (arg2 & MODE_AXIS_Y) {
                if (hit == 0 || bestTop >= pos[1]) {
                    pos[1] = bestTop;
                    hit = pend;
                    hitIsBox = 1;
                }
            } else if (arg2 & MODE_AXIS_Z) {
                if (hit == 0 || bestTop >= pos[2]) {
                    pos[2] = bestTop;
                    hit = pend;
                    hitIsBox = 1;
                }
            } else {
                /* no axis: the pending box is dropped */
            }
        }
    }

    id = -1;
    if (hit != 0) {
        if (hitIsBox) {
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
