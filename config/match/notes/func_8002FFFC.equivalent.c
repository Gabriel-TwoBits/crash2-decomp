#include "common.h"
#include "sdk.h"
#include "game.h"

#define POLY_CORNERS 4
#define POLY_EDGES 3
#define POLY_CLIP_GROUPS 4
#define POLY_OUT_GROUPS 6
#define SLOT_COUNT (POLY_CLIP_GROUPS * POLY_EDGES)
#define FIXED_SHIFT 8

typedef struct Unk8002FFFCVec {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
} Unk8002FFFCVec;

typedef struct Unk8002FFFCSlot {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 hit;
    /* 0x8 */ s32 z;
} Unk8002FFFCSlot;

extern s32 D_8005BBB8[];
extern s8 D_8005BC98[];

s32 func_8002FFFC(s32 arg0, Unk8002FFFCVec *arg1, Unk8002FFFCVec *arg2) {
    Unk8002FFFCVec *pts[POLY_CORNERS];
    Unk8002FFFCSlot slots[SLOT_COUNT];
    s8 used[POLY_EDGES];
    Unk8002FFFCVec *prev;
    Unk8002FFFCVec *cur;
    Unk8002FFFCVec *first_on;
    Unk8002FFFCSlot *s;
    Unk8002FFFCSlot *first_hit;
    Unk8002FFFCSlot *slot;
    s32 *idx;
    s32 count;
    s32 g;
    s32 i;
    s32 t;
    s32 ylo;
    s32 yhi;
    s8 *flag;

    idx = D_8005BBB8;
    s = slots;
    for (g = 0; g < POLY_CLIP_GROUPS; g++) {
        for (i = 0; i < POLY_CORNERS; i++) {
            pts[i] = &arg1[*idx++];
        }
        prev = pts[0];
        ylo = prev->y;
        for (i = 0; i < POLY_EDGES; i++) {
            s->hit = 0;
            cur = pts[i + 1];
            yhi = cur->y;
            if (yhi >= ylo) {
                if (ylo < arg0 && arg0 < yhi) {
                    t = ((arg0 - ylo) << FIXED_SHIFT) / (yhi - ylo);
                    s->x = (((cur->x - prev->x) * t) >> FIXED_SHIFT) + prev->x;
                    s->hit = 1;
                    s->z = (((cur->z - prev->z) * t) >> FIXED_SHIFT) + prev->z;
                } else {
                    /* edge does not cross the plane */
                }
            } else {
                if (yhi < arg0 && arg0 < ylo) {
                    t = ((arg0 - yhi) << FIXED_SHIFT) / (ylo - yhi);
                    s->x = (((prev->x - cur->x) * t) >> FIXED_SHIFT) + cur->x;
                    s->hit = 1;
                    s->z = (((prev->z - cur->z) * t) >> FIXED_SHIFT) + cur->z;
                } else {
                    /* edge does not cross the plane */
                }
            }
            s++;
            prev = cur;
            ylo = yhi;
        }
    }

    idx = D_8005BBB8;
    count = 0;
    used[0] = 0;
    used[1] = 0;
    used[2] = 0;
    for (g = 0; g < POLY_OUT_GROUPS; g++) {
        first_hit = 0;
        first_on = 0;
        flag = &D_8005BC98[g * POLY_CORNERS];
        for (i = 0; i < POLY_CORNERS; i++) {
            pts[i] = &arg1[*idx++];
        }
        for (i = 0; i < POLY_CORNERS; i++) {
            slot = &slots[flag[i]];
            cur = pts[i];
            if (slot->hit == 1) {
                if (first_hit == 0) {
                    first_hit = slot;
                    continue;
                }
                arg2[0].x = first_hit->x;
                arg2[0].y = arg0;
                arg2[0].z = first_hit->z;
                arg2[1].x = slot->x;
                arg2[1].y = arg0;
                arg2[1].z = slot->z;
                arg2 += 2;
                count++;
                goto next_group;
            } else if (cur->y == arg0) {
                if (first_on == 0) {
                    first_on = cur;
                    continue;
                }
                if (pts[(i - 1) & 3]->y == arg0) {
                    s8 *u = &used[flag[(i - 1) & 3]];
                    if (*u != 0) {
                        goto next_group;
                    } else {
                        *u = 1;
                    }
                } else {
                    /* previous corner is off the plane */
                }
                arg2[0] = *first_on;
                arg2[1] = *cur;
                arg2 += 2;
                count++;
                goto next_group;
            } else {
                /* corner and edge both miss the plane */
            }
        }
        if (first_hit != 0) {
            arg2[0].x = first_hit->x;
            arg2[0].y = arg0;
            arg2[0].z = first_hit->z;
            arg2[1] = *first_on;
            arg2 += 2;
            count++;
        } else if (first_on != 0) {
            arg2[0] = *first_on;
            arg2[1] = *first_on;
            return 1;
        } else {
            /* group does not touch the plane */
        }
next_group:;
    }
    return count;
}
