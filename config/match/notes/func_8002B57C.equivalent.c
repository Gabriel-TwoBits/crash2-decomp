#include "common.h"
#include "sdk.h"
#include "game.h"

#define NO_HIT 0xC4653601
#define FLAG_BIG 0x40000000
#define FLAG_MODE_40 0x40
#define FLAG_SKIP_BOUND 0x200000
#define FLAG_PROBE_SRC 0x08000000
#define FLAG_PROBE_SKIP 0x4000
#define FLAG_QUERY_20000 0x20000
#define STATE_TOUCHED 0x80

#define HALF_SMALL 0x4B00
#define HALF_NEAR 0x2580
#define BIG_FWD 0x29900
#define BIG_BACK 0xA640
#define BIG_BACK_MID 0x5320
#define BIG_LIMIT_A 0x14C80
#define LIMIT_B 0x19000
#define BIG_FWD_FAR 0x53200
#define BIG_FWD_NEAR 0x33F40
#define BOUNDS_OFFSET 0x1008
#define BOX_OFFSET 0x1020
#define QUERY_FLAGS_OFFSET 0x29C
#define SKIP_THRESHOLD (-0xFE)
#define PARAM_6400 0x6400
#define PARAM_1700 0x1700

typedef struct Unk8002B57CScale {
    u8 pad_00[0x40];
    s32 unk_40;
} Unk8002B57CScale;

typedef struct Unk8002B57CInfo {
    u8 pad_00[0x4];
    s32 unk_04;
    u8 pad_08[0x8];
    s32 unk_10;
} Unk8002B57CInfo;

typedef struct Unk8002B57CLink {
    u8 pad_00[0x10];
    u8 *unk_10;
    Unk8002B57CInfo *unk_14;
} Unk8002B57CLink;

typedef struct Unk8002B57CObj {
    u8 pad_00[0x4];
    struct Unk8002B57CObj *unk_04;
    Unk8002B57CScale *unk_08;
    u8 pad_0C[0xC];
    Unk8002B57CLink *unk_18;
    u8 pad_1C[0x6C];
    s32 unk_88;
    u8 pad_8C[0x1C];
    s32 unk_A8;
    s32 unk_AC;
    u8 pad_B0[0x50];
    s32 unk_100;
} Unk8002B57CObj;

extern void *D_800608CC;
extern Unk8006CB6C D_8006CB6C;

s32 func_8003D040(void *, void *, void *, s32 *, s32, s32, s32, s32, s32);

s32 func_8002B57C(void *arg0, s32 *arg1, void *arg2, s32 arg3) {
    Unk8002B57CObj *self;
    Unk8002B57CObj *best_obj;
    Unk8002B57CObj *obj;
    Unk8002B57CInfo *info;
    s32 *box;
    s32 best;
    s32 hit;
    s32 i;
    s32 big;
    s32 base;
    s32 back;
    s32 half;
    s32 limit;
    s32 bound;
    s32 sp28[3];
    s32 sp38[4];

    self = arg0;
    best = NO_HIT;
    best_obj = 0;
    box = (s32 *)((u8 *)arg2 + BOX_OFFSET);
    big = (self->unk_100 & FLAG_BIG) != 0;

    box[0] = arg1[0] - HALF_SMALL;
    if (self->unk_100 & FLAG_MODE_40) {
        if (!big) {
            box[1] = arg1[1] - self->unk_08->unk_40 / 4;
        } else {
            box[1] = arg1[1] - BIG_BACK;
        }
        box[2] = arg1[2] - HALF_SMALL;
        box[3] = arg1[0] + HALF_SMALL;
        if (!big) {
            box[4] = arg1[1] + self->unk_08->unk_40 * 2;
        } else {
            box[4] = arg1[1] + BIG_FWD_FAR;
        }
        box[5] = arg1[2] + HALF_SMALL;
    } else {
        if (!big) {
            base = arg1[1] + self->unk_08->unk_40;
        } else {
            base = arg1[1] + BIG_FWD;
        }
        if (arg3 != 0) {
            if (!big) {
                box[1] = base - self->unk_08->unk_40 / 8;
            } else {
                box[1] = base - BIG_BACK_MID;
            }
        } else {
            if (!big) {
                back = self->unk_08->unk_40 / 4;
                half = self->unk_08->unk_40 / 2;
                if (half > LIMIT_B - 1) {
                    limit = back > LIMIT_B;
                } else {
                    limit = half < back;
                }
            } else {
                back = BIG_BACK;
                limit = back > BIG_LIMIT_A;
            }
            if (limit) {
                if (!big) {
                    box[1] = base - self->unk_08->unk_40 / 4;
                } else {
                    box[1] = base - BIG_BACK;
                }
            } else if (!big) {
                half = self->unk_08->unk_40 / 2;
                if (half > LIMIT_B - 1) {
                    box[1] = base - LIMIT_B;
                } else {
                    box[1] = base - half;
                }
            } else {
                box[1] = base - BIG_LIMIT_A;
            }
        }
        box[2] = arg1[2] - HALF_SMALL;
        box[3] = arg1[0] + HALF_SMALL;
        if (!big) {
            box[4] = arg1[1] + self->unk_08->unk_40;
        } else {
            box[4] = arg1[1] + BIG_FWD;
        }
        box[5] = arg1[2] + HALF_SMALL;
    }

    for (i = 0; i < D_8006CB6C.n; i++) {
        obj = (Unk8002B57CObj *)D_8006CB6C.p[i];
        if ((obj->unk_04->unk_AC & FLAG_PROBE_SRC) && !(obj->unk_04->unk_A8 & FLAG_PROBE_SKIP)) {
            hit = func_8002A8D0(obj, box, 0, 1, best, NO_HIT, 0);
            if (hit != NO_HIT) {
                best = hit;
                best_obj = obj->unk_04;
            }
        }
    }

    box[0] = arg1[0] - HALF_NEAR;
    box[2] = arg1[2] - HALF_NEAR;
    box[3] = arg1[0] + HALF_NEAR;
    box[5] = arg1[2] + HALF_NEAR;
    if (!big) {
        box[4] = arg1[1] + self->unk_08->unk_40 * 5 / 4;
    } else {
        box[4] = arg1[1] + BIG_FWD_NEAR;
    }
    hit = func_8003D040(arg0, arg2, (u8 *)arg2 + BOUNDS_OFFSET, box, 2, 6, 1, NO_HIT, 1);

    if (*(s32 *)(self->unk_18->unk_10 + QUERY_FLAGS_OFFSET) & FLAG_QUERY_20000) {
        sp28[0] = arg1[0];
        sp28[1] = box[4];
        sp28[2] = arg1[2];
        if ((s32)func_80027DEC(D_800608CC, sp28) >= SKIP_THRESHOLD) {
            info = self->unk_18->unk_14;
            bound = (info->unk_04 + info->unk_10) << 8;
            if (bound < sp28[1]) {
                hit = bound;
            }
        }
    }

    if (hit != NO_HIT) {
        self->unk_A8 |= STATE_TOUCHED;
    }
    if (best != NO_HIT && (hit == NO_HIT || best < hit)) {
        best_obj->unk_A8 |= STATE_TOUCHED;
        self->unk_A8 |= STATE_TOUCHED;
        if (self->unk_88 >= 0 || best_obj->unk_88 < 0) {
            sp38[0] = PARAM_6400;
            func_8001CC10((s32)self, (s32)best_obj, PARAM_1700, 1, sp38);
        }
        if (!(self->unk_100 & FLAG_SKIP_BOUND)) {
            return best;
        }
    }
    if (self->unk_100 & FLAG_MODE_40) {
        return hit;
    } else if (!big) {
        if (arg1[1] + self->unk_08->unk_40 < hit) {
            return NO_HIT;
        } else {
            return hit;
        }
    } else if (arg1[1] + BIG_FWD < hit) {
        return NO_HIT;
    } else {
        return hit;
    }
}
