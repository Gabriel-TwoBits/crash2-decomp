#include "common.h"
#include "sdk.h"
#include "game.h"

#define FIELD(p, off) (*(s32 *)((u8 *)(p) + (off)))
#define FIELD_U(p, off) (*(u32 *)((u8 *)(p) + (off)))
#define FIELD_P(p, off) (*(void **)((u8 *)(p) + (off)))

/* Object field offsets. */
#define OBJ_MATRIX 0x08
#define OBJ_ANIM 0x0C
#define OBJ_PARENT 0x58
#define OBJ_POS 0x60
#define OBJ_ROT_X 0x6C
#define OBJ_ROT_Y 0x70
#define OBJ_ROT_Z 0x74
#define OBJ_ROT 0x78
#define OBJ_VEL_X 0x84
#define OBJ_VEL_Y 0x88
#define OBJ_VEL_Z 0x8C
#define OBJ_TURN_RATE 0x90
#define OBJ_ANGLE 0x94
#define OBJ_ANGLE_X 0x98
#define OBJ_FLAGS_A 0xA8
#define OBJ_FLAGS_B 0xAC
#define OBJ_GROUND_PTR 0xE8
#define OBJ_FLOOR_Y 0xFC
#define OBJ_FLAGS_C 0x100
#define OBJ_SPEED 0x104
#define OBJ_MODE 0x108
#define OBJ_LAND_FRAME 0x110
#define OBJ_LAND_VEL 0x114
#define OBJ_ALPHA 0x11C
#define OBJ_ANGLE_X_RATE 0x124
#define OBJ_TARGET_ANGLE 0x168
#define OBJ_FRAME_STAMP 0x16C
#define PARENT_ANGLE 0x94
#define ANIM_FRAME_PTR 0x10
#define GLOBAL_TICK 0x14

/* Mask bits of FLAGS_A (offset 0xA8). */
#define A_GROUNDED 0x1
#define A_ON_PLATFORM 0x200
#define A_LOCKED_ROT 0x2000
#define A_PARENT_CARRY 0x4000
#define A_POS_LATCH 0x40000
#define A_STUCK 0x02000000
#define A_KEEP_MASK 0x207E
#define A_KEEP_HI_AIR 0xFACA0000
#define A_KEEP_HI_GROUND 0xF8CA0000

/* Bits of FLAGS_B (offset 0xAC). */
#define B_TURN 0x1
#define B_MOVE 0x8
#define B_COLLIDE 0x10
#define B_GRAVITY 0x20
#define B_APPLY_VEL 0x40
#define B_STEER 0x80
#define B_LIMIT_VEL 0x1000
#define B_TURN_X 0x2000
#define B_FLOOR 0x4000
#define B_FLOOR_PROBE 0x8000
#define B_TURN_SMOOTH 0x80000
#define B_CARRY 0x200000

/* Bits of FLAGS_C (offset 0x100). */
#define C_RUN 0x4
#define C_WALK 0x8
#define C_FAST 0x10
#define C_SPIN 0x40
#define C_SLIDE 0x100
#define C_SLIDE_BOOST 0x200000
#define C_JUMP 0x800000
#define C_BOUNCE 0x08000000
#define C_NO_STEER 0x04000000
#define C_BLOCK_MASK 0x04A08000
#define C_NO_FRAME_MASK 0xA08000
#define C_BLOCK_DEC_MASK 0xA00008

#define TICK_CAP 0x66
#define TICK_SCALE_SHIFT 10
#define TICK_SCALE 1024
#define SPEED_DAMP_NUM 0xE6
#define SPEED_DAMP_SHIFT 8
#define FRAME_WINDOW 0xB
#define FIXED_ONE_SHIFT 8
#define ANGLE_MASK 0xFFF
#define ANGLE_HALF 0x800
#define ANGLE_FULL 0x1000
#define ANGLE_NEAR 0x7C8
#define ANGLE_QUARTER 0x400
#define SPEED_MIN 0x100
#define MODE_STOP 5
#define CAM_STOP_MODE 8
#define STEER_ID 0x100
#define SIN_SHIFT 4
#define SIN_STEP_SHIFT 6
#define ANGLE_SHIFT 20
#define GRAVITY_PER_TICK 0xFA0
#define FALL_LIMIT (-0x2EE000)
#define ALPHA_OPAQUE 0xFF
#define NEAR_DIST 0x7D000
#define FAR_DIST 0xAF000
#define GROUND_TYPE_PLAIN 1
#define VEC_STRIDE_ROW 4
#define RING_COUNT 7
#define RING_PREV 0x48
#define RING_NEXT 0x4C
#define SCRATCH_REG ((void **)0x1F80005C)

/* Offsets of the parameter blocks (bytes from D_8005B7B0). */
#define PARAMS_SPIN 0xC8
#define PARAMS_BOUNCE 0x12C
#define PARAMS_JUMP 0xB4
#define PARAMS_SLIDE 0x8C
#define PARAMS_SLIDE_FAST 0x8C
#define PARAMS_WALK 0x14
#define PARAMS_LOCKED_EXTRA 0x50
#define PARAMS_FAST_SLIDE_EXTRA 0x78

extern s32 *D_80063450;
extern s32 D_8005F38C;
extern u8 D_8005B88C[];
extern u8 D_8005B7B0[];
extern u8 D_8005B7D8[];
extern u8 D_8005B7EC[];
extern u8 D_8005B800[];
extern u8 D_8005B850[];
extern s32 D_8005B8B8;
extern u8 D_8005C514[];
extern s32 D_8005F494;
extern s32 D_80060914;
extern u8 D_80067FE4[];
extern s32 D_80069958;
extern s32 D_8006995C;
extern s32 D_80069960;
extern s32 D_8006CB64;
extern s32 D_8006CBB4;
extern s32 D_8006CD54;
extern s32 D_8006CD58;
extern s32 D_8006CD5C;

struct Obj;
s32 func_8001F054(struct Obj *, u32, s32 *);

static inline u32 tick_step(void) {
    u32 n;

    n = FIELD_U(D_80063450, GLOBAL_TICK);
    if (n >= TICK_CAP + 1) {
        n = TICK_CAP;
    }
    return n;
}

/* Divides by 1024, rounding toward zero. */
static inline s32 scale_div(s32 v) {
    if (v < 0) {
        v += (TICK_SCALE - 1);
    }
    return v >> TICK_SCALE_SHIFT;
}

static inline s32 clamp_sym(s32 v, s32 lim) {
    if (v < lim) {
        if (v >= -lim) {
            return v;
        } else {
            return -lim;
        }
    } else {
        return lim;
    }
}

void func_8001D63C(s32 *arg0, void *arg1, s32 *arg2) {
    s32 sp18[3];
    s32 sp28[3];
    s32 sp38[4];
    s32 sp48[3];
    s32 flagsA;
    void *parent;
    s32 *params;
    s32 vel_scale;
    s32 target;
    s32 delta;
    s32 diff;
    s32 cosv;
    s32 steer;
    s32 n;
    s32 i;
    s32 count;
    u8 *ground;
    s32 keep;
    s32 t;

    flagsA = FIELD(arg0, OBJ_FLAGS_A);
    parent = FIELD_P(arg0, OBJ_PARENT);
    if ((s32)arg0 == D_8005F38C &&
        ((flagsA & A_STUCK) || (FIELD(arg0, OBJ_FLAGS_C) & C_NO_STEER) ||
         (((u32)(D_8006CB64 - FIELD(arg0, OBJ_FRAME_STAMP)) < FRAME_WINDOW) &&
          !(FIELD(arg0, OBJ_FLAGS_C) & C_NO_FRAME_MASK)))) {
        if (!(FIELD(arg0, OBJ_FLAGS_C) & C_BLOCK_MASK)) {
            D_8005F1EC = D_8005B88C;
            func_8001D2C4(arg0, (s32 *)D_8005B88C, &D_8005F494);
        } else if (!(FIELD(arg0, OBJ_FLAGS_C) & C_NO_STEER)) {
            if (!(FIELD(arg0, OBJ_FLAGS_A) & A_STUCK)) {
                FIELD(arg0, OBJ_VEL_X) = (FIELD(arg0, OBJ_VEL_X) * SPEED_DAMP_NUM) >> SPEED_DAMP_SHIFT;
                FIELD(arg0, OBJ_VEL_Z) = (FIELD(arg0, OBJ_VEL_Z) * SPEED_DAMP_NUM) >> SPEED_DAMP_SHIFT;
            }
            FIELD(arg0, OBJ_VEL_X) = clamp_sym(FIELD(arg0, OBJ_VEL_X), D_8005B8B8);
            FIELD(arg0, OBJ_VEL_Y) = clamp_sym(FIELD(arg0, OBJ_VEL_Y), D_8005B8B8);
            FIELD(arg0, OBJ_VEL_Z) = clamp_sym(FIELD(arg0, OBJ_VEL_Z), D_8005B8B8);
        } else {
            /* Steering blocked: nothing to do. */
        }
    } else if ((FIELD(arg0, OBJ_FLAGS_B) & B_STEER) && D_8006CBB4 == STEER_ID) {
        params = (s32 *)D_8005B7B0;
        if (FIELD(arg0, OBJ_FLAGS_C) & C_RUN) {
            if (FIELD(arg0, OBJ_FLAGS_C) & C_SPIN) {
                params = (s32 *)(D_8005B7B0 + PARAMS_SPIN);
            } else if (FIELD(arg0, OBJ_FLAGS_C) & C_BOUNCE) {
                params = (s32 *)(D_8005B7B0 + PARAMS_BOUNCE);
            } else if (FIELD(arg0, OBJ_FLAGS_C) & C_JUMP) {
                params = (s32 *)(D_8005B7B0 + PARAMS_JUMP);
            } else if (FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE_BOOST) {
                params = (s32 *)(D_8005B7B0 + PARAMS_SLIDE);
                if (FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE) {
                    params = (s32 *)((u8 *)params + PARAMS_SLIDE_FAST);
                }
            } else {
                params = (s32 *)(D_8005B7B0 + PARAMS_WALK);
                if (FIELD(arg0, OBJ_FLAGS_A) & A_LOCKED_ROT) {
                    params = (s32 *)((u8 *)params + PARAMS_LOCKED_EXTRA);
                }
            }
        }
        if (FIELD(arg0, OBJ_MODE) == MODE_STOP) {
            params = (s32 *)D_8005B800;
        }
        if (FIELD(arg0, OBJ_FLAGS_C) & C_WALK) {
            if (FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE_BOOST) {
                params = (s32 *)D_8005B850;
                if (FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE) {
                    params = (s32 *)(D_8005B850 + PARAMS_FAST_SLIDE_EXTRA);
                }
            } else {
                params = (s32 *)D_8005B7D8;
                if (FIELD(arg0, OBJ_FLAGS_A) & A_LOCKED_ROT) {
                    params = (s32 *)(D_8005B7D8 + PARAMS_LOCKED_EXTRA);
                }
            }
        }
        if (FIELD(arg0, OBJ_FLAGS_C) & C_FAST) {
            params = (s32 *)D_8005B7EC;
        }
        steer = D_8006995C;
        if (D_80069960 != CAM_STOP_MODE) {
            target = (D_80060914 + D_80069958) & ANGLE_MASK;
            diff = target - FIELD(arg0, OBJ_ANGLE);
            FIELD(arg0, OBJ_TARGET_ANGLE) = target;
            if (diff < 0) {
                diff = FIELD(arg0, OBJ_ANGLE) - target;
            }
            if (diff > ANGLE_HALF) {
                diff = ANGLE_FULL - diff;
            }
            if (FIELD(arg0, OBJ_FLAGS_C) & C_WALK) {
                cosv = func_8003D360(diff);
                n = tick_step();
                delta = scale_div(params[0] * n);
                FIELD(arg0, OBJ_SPEED) += ((delta * (cosv >> SIN_STEP_SHIFT)) >> SIN_STEP_SHIFT);
                if (FIELD(arg0, OBJ_SPEED) <= SPEED_MIN) {
                    if (!(FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE_BOOST)) {
                        FIELD(arg0, OBJ_ANGLE) = target;
                    } else {
                        FIELD(arg0, OBJ_ANGLE) = func_8001E3DC(FIELD(arg0, OBJ_ANGLE), target, params[4], 0);
                    }
                } else if (diff < ANGLE_NEAR || (FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE_BOOST)) {
                    FIELD(arg0, OBJ_ANGLE) = func_8001E3DC(FIELD(arg0, OBJ_ANGLE), target, params[4], 0);
                } else {
                    n = tick_step();
                    FIELD(arg0, OBJ_SPEED) -= scale_div(params[3] * n);
                    if (FIELD(arg0, OBJ_SPEED) < 0) {
                        FIELD(arg0, OBJ_SPEED) = 0;
                    }
                }
            } else if (FIELD(arg0, OBJ_SPEED) <= SPEED_MIN || diff <= ANGLE_QUARTER) {
                if (FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE_BOOST) {
                    FIELD(arg0, OBJ_ANGLE) = func_8001E3DC(FIELD(arg0, OBJ_ANGLE), target, params[4], 0);
                } else {
                    FIELD(arg0, OBJ_ANGLE) = target;
                }
                n = tick_step();
                FIELD(arg0, OBJ_SPEED) += scale_div(params[0] * n);
            } else if ((FIELD(arg0, OBJ_FLAGS_C) & C_SLIDE_BOOST) || (FIELD(arg0, OBJ_FLAGS_A) & A_STUCK)) {
                n = tick_step();
                FIELD(arg0, OBJ_SPEED) -= scale_div(params[3] * n);
                if (FIELD(arg0, OBJ_SPEED) < 0) {
                    FIELD(arg0, OBJ_SPEED) = 0;
                }
            } else {
                FIELD(arg0, OBJ_SPEED) = 0;
            }
            if (params[1] < FIELD(arg0, OBJ_SPEED)) {
                FIELD(arg0, OBJ_SPEED) = params[1];
            }
        } else if ((FIELD(arg0, OBJ_FLAGS_A) & A_STUCK) ||
                   ((FIELD(arg0, OBJ_FLAGS_C) & C_BLOCK_DEC_MASK) == C_SLIDE_BOOST)) {
            steer = SPEED_MIN;
        } else {
            n = tick_step();
            FIELD(arg0, OBJ_SPEED) -= scale_div(params[3] * n);
            if (FIELD(arg0, OBJ_SPEED) < 0) {
                FIELD(arg0, OBJ_SPEED) = 0;
            }
        }
        steer = (steer * FIELD(arg0, OBJ_SPEED)) >> FIXED_ONE_SHIFT;
        FIELD(arg0, OBJ_VEL_X) = ((func_8003D2EC(FIELD(arg0, OBJ_ANGLE)) >> SIN_SHIFT) * steer) >> FIXED_ONE_SHIFT;
        FIELD(arg0, OBJ_VEL_Z) = ((func_8003D360(FIELD(arg0, OBJ_ANGLE)) >> SIN_SHIFT) * steer) >> FIXED_ONE_SHIFT;
    } else {
        /* Not steerable this frame: nothing to do. */
    }
    if ((s32)arg0 == D_8005F38C) {
        D_8005F494 = FIELD(arg0, OBJ_POS);
        (&D_8005F494)[1] = FIELD(arg0, OBJ_POS + 4);
        (&D_8005F494)[2] = FIELD(arg0, OBJ_POS + 8);
        i = RING_COUNT;
        do {
            t = (i * 4) + FIELD(arg0, OBJ_MATRIX);
            FIELD(t, RING_NEXT) = FIELD(t, RING_PREV);
            i -= 1;
        } while (i > 0);
        FIELD(FIELD(arg0, OBJ_MATRIX), RING_NEXT) = flagsA;
    }
    t = FIELD(arg0, OBJ_FLAGS_A);
    keep = A_KEEP_HI_AIR;
    if (t & A_GROUNDED) {
        keep = A_KEEP_HI_AIR;
        if (!(FIELD(arg0, OBJ_FLAGS_C) & C_WALK)) {
            keep = A_KEEP_HI_GROUND;
        }
    }
    FIELD(arg0, OBJ_FLAGS_A) = t & (keep | A_KEEP_MASK);
    if (FIELD(arg0, OBJ_FLAGS_B) & B_TURN) {
        FIELD(arg0, OBJ_ROT_Y) = func_8001E3DC(FIELD(arg0, OBJ_ROT_Y), FIELD(arg0, OBJ_ANGLE), FIELD(arg0, OBJ_TURN_RATE), arg0);
    }
    if (FIELD(arg0, OBJ_FLAGS_B) & B_TURN_SMOOTH) {
        FIELD(arg0, OBJ_ROT_Y) = func_8001E544(FIELD(arg0, OBJ_ROT_Y), FIELD(arg0, OBJ_ANGLE), FIELD(arg0, OBJ_TURN_RATE), arg0);
    }
    if (FIELD(arg0, OBJ_FLAGS_B) & B_TURN_X) {
        FIELD(arg0, OBJ_ROT_X) = func_8001E3DC(FIELD(arg0, OBJ_ROT_X), FIELD(arg0, OBJ_ANGLE_X), FIELD(arg0, OBJ_ANGLE_X_RATE), 0);
    }
    func_8001A4A4(arg0, (s32 *)((u8 *)arg0 + OBJ_ROT));
    func_8001AA80((u8 *)arg0, arg2);
    if (*(s32 *)FIELD(FIELD(arg0, OBJ_ANIM), ANIM_FRAME_PTR) != MODE_STOP) {
        FIELD_P(arg0, OBJ_PARENT) = 0;
    }
    if (FIELD(arg0, OBJ_FLAGS_B) & B_APPLY_VEL) {
        sp28[0] = FIELD(arg0, OBJ_POS);
        sp28[1] = FIELD(arg0, OBJ_POS + 4);
        sp28[2] = FIELD(arg0, OBJ_POS + 8);
        sp18[0] = scale_div(FIELD(arg0, OBJ_VEL_X) * tick_step());
        sp18[1] = scale_div(FIELD(arg0, OBJ_VEL_Y) * tick_step());
        sp18[2] = scale_div(FIELD(arg0, OBJ_VEL_Z) * tick_step());
        if (FIELD(arg0, OBJ_FLAGS_B) & B_MOVE) {
            FIELD(arg0, OBJ_ALPHA) = ALPHA_OPAQUE;
            *SCRATCH_REG = D_8005C514;
            func_8002CAB8();
            func_80029884(arg0, (s32)sp18, (s32)D_80067FE4);
            if (flagsA & A_STUCK) {
                t = FIELD(arg0, OBJ_FLAGS_A);
                if (!(t & A_GROUNDED) && (flagsA & A_GROUNDED)) {
                    FIELD(arg0, OBJ_FLAGS_A) = t | A_STUCK;
                }
            }
            if (FIELD(arg0, OBJ_FLAGS_A) & A_POS_LATCH) {
                D_8006CD54 = FIELD(arg0, OBJ_POS);
                D_8006CD58 = FIELD(arg0, OBJ_POS + 4);
                D_8006CD5C = FIELD(arg0, OBJ_POS + 8);
            }
            func_80019F08();
        } else {
            FIELD(arg0, OBJ_POS) += sp18[0];
            FIELD(arg0, OBJ_POS + 4) += sp18[1];
            FIELD(arg0, OBJ_POS + 8) += sp18[2];
        }
        if (FIELD(arg0, OBJ_FLAGS_B) & B_FLOOR_PROBE) {
            func_8001F054((struct Obj *)arg0, 0, sp38);
            FIELD(arg0, OBJ_FLOOR_Y) = sp38[1];
            if (FIELD(arg0, OBJ_FLAGS_A) & A_ON_PLATFORM) {
                FIELD(arg0, OBJ_POS) = sp28[0];
                FIELD(arg0, OBJ_POS + 4) = sp28[1];
                FIELD(arg0, OBJ_POS + 8) = sp28[2];
            }
        }
        if (FIELD(arg0, OBJ_FLAGS_B) & B_LIMIT_VEL) {
            FIELD(arg0, OBJ_VEL_X) = clamp_sym(FIELD(arg0, OBJ_VEL_X), FIELD(arg0, OBJ_ANGLE_X));
            FIELD(arg0, OBJ_VEL_Y) = clamp_sym(FIELD(arg0, OBJ_VEL_Y), FIELD(arg0, OBJ_ANGLE_X));
            FIELD(arg0, OBJ_VEL_Z) = clamp_sym(FIELD(arg0, OBJ_VEL_Z), FIELD(arg0, OBJ_ANGLE_X));
        }
        if (FIELD(arg0, OBJ_FLAGS_B) & B_FLOOR) {
            t = FIELD(arg0, OBJ_FLOOR_Y);
            if (t >= FIELD(arg0, OBJ_POS + 4)) {
                FIELD(arg0, OBJ_POS + 4) = t;
                t = FIELD(arg0, OBJ_VEL_Y);
                FIELD(arg0, OBJ_FLAGS_A) |= A_GROUNDED;
                FIELD(arg0, OBJ_LAND_FRAME) = D_8006CB64;
                if (t < 0) {
                    FIELD(arg0, OBJ_LAND_VEL) = t;
                    FIELD(arg0, OBJ_VEL_Y) = 0;
                }
            }
        }
    }
    if ((FIELD(arg0, OBJ_FLAGS_B) & B_CARRY) && parent != 0 && (flagsA & A_PARENT_CARRY)) {
        sp28[0] = FIELD(arg0, OBJ_POS) - FIELD(arg1, 0);
        sp28[1] = FIELD(arg0, OBJ_POS + 4) - FIELD(arg1, 4);
        sp28[2] = FIELD(arg0, OBJ_POS + 8) - FIELD(arg1, 8);
        FIELD(parent, OBJ_POS) += sp28[0];
        FIELD(parent, OBJ_POS + 4) += sp28[1];
        FIELD(parent, OBJ_POS + 8) += sp28[2];
        sp28[0] = FIELD(parent, OBJ_POS) - FIELD(arg0, OBJ_POS);
        sp28[1] = FIELD(parent, OBJ_POS + 4) - FIELD(arg0, OBJ_POS + 4);
        sp28[2] = FIELD(parent, OBJ_POS + 8) - FIELD(arg0, OBJ_POS + 8);
        sp48[0] = ((FIELD(arg0, OBJ_ROT_X) - FIELD(arg1, 0xC)) << ANGLE_SHIFT) >> ANGLE_SHIFT;
        sp48[1] = ((FIELD(arg0, OBJ_ROT_Y) - FIELD(arg1, 0x10)) << ANGLE_SHIFT) >> ANGLE_SHIFT;
        sp48[2] = FIELD(arg0, OBJ_ROT_Z);
        if (sp48[0] != 0 || sp48[1] != 0) {
            func_8001E8E4(sp28, arg0 + OBJ_POS / 4, sp48, 0, (s32 *)((u8 *)parent + OBJ_POS));
            FIELD(parent, PARENT_ANGLE) += sp48[1];
        }
    }
    if (FIELD(arg0, OBJ_FLAGS_B) & B_GRAVITY) {
        n = tick_step();
        t = FIELD(arg0, OBJ_VEL_Y) - (n * GRAVITY_PER_TICK);
        FIELD(arg0, OBJ_VEL_Y) = t;
        if (t < FALL_LIMIT) {
            FIELD(arg0, OBJ_VEL_Y) = FALL_LIMIT;
        }
    }
    if ((FIELD(arg0, OBJ_FLAGS_B) & B_COLLIDE) &&
        func_800310EC(arg0, arg0 + OBJ_POS / 4, (s32 *)(D_8005F38C + OBJ_POS), NEAR_DIST, FAR_DIST, NEAR_DIST) == 0) {
        ground = FIELD_P(arg0, OBJ_GROUND_PTR);
        if (ground != 0 && *ground == GROUND_TYPE_PLAIN) {
            count = FIELD(FIELD_P(FIELD_P(arg0, OBJ_MATRIX), 0x20), 0xC);
            for (i = 0; i < count; i++) {
                func_8002C324(arg0, 1, i);
            }
        } else {
            func_8002C324(arg0, 1, -1);
        }
    }
}
