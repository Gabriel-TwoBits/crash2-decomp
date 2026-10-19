#include "common.h"
#include "sdk.h"
#include "game.h"

extern u8 D_8005B88C[];

u8 *D_8005F1EC = D_8005B88C;
s32 D_8005F1F0 = 0x80;

#ifdef NON_MATCHING
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

extern s32 *g_DisplayBuffer;
extern s32 g_Player;
extern u8 D_8005B7B0[];
extern u8 D_8005B7D8[];
extern u8 D_8005B7EC[];
extern u8 D_8005B800[];
extern u8 D_8005B850[];
extern s32 D_8005B8B8;
extern u8 g_GoolOpcodeHandlers[];
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

s32 func_8001F054(GoolObject *, u32, s32 *);

static inline u32 tick_step(void) {
    u32 n;

    n = FIELD_U(g_DisplayBuffer, GLOBAL_TICK);
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

void func_8001D63C(void *obj, s32 *arg1, s32 *arg2) {
    s32 *arg0 = obj;
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
    if ((s32)arg0 == g_Player &&
        ((flagsA & A_STUCK) || (FIELD(arg0, OBJ_FLAGS_C) & C_NO_STEER) ||
         (((u32)(D_8006CB64 - FIELD(arg0, OBJ_FRAME_STAMP)) < FRAME_WINDOW) &&
          !(FIELD(arg0, OBJ_FLAGS_C) & C_NO_FRAME_MASK)))) {
        if (!(FIELD(arg0, OBJ_FLAGS_C) & C_BLOCK_MASK)) {
            D_8005F1EC = D_8005B88C;
            func_8001D2C4((GoolObject *)arg0, (s32 *)D_8005B88C, &D_8005F494);
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
                cosv = Math_Cos(diff);
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
        FIELD(arg0, OBJ_VEL_X) = ((Math_Sin(FIELD(arg0, OBJ_ANGLE)) >> SIN_SHIFT) * steer) >> FIXED_ONE_SHIFT;
        FIELD(arg0, OBJ_VEL_Z) = ((Math_Cos(FIELD(arg0, OBJ_ANGLE)) >> SIN_SHIFT) * steer) >> FIXED_ONE_SHIFT;
    } else {
        /* Not steerable this frame: nothing to do. */
    }
    if ((s32)arg0 == g_Player) {
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
    func_8001AA80((GoolObject *)arg0, arg2);
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
            *SCRATCH_REG = g_GoolOpcodeHandlers;
            func_8002CAB8();
            func_80029884((GoolObject *)arg0, (s32)sp18, (s32)D_80067FE4);
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
            Gool_InitScratchpad();
        } else {
            FIELD(arg0, OBJ_POS) += sp18[0];
            FIELD(arg0, OBJ_POS + 4) += sp18[1];
            FIELD(arg0, OBJ_POS + 8) += sp18[2];
        }
        if (FIELD(arg0, OBJ_FLAGS_B) & B_FLOOR_PROBE) {
            func_8001F054((GoolObject *)arg0, 0, sp38);
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
        func_800310EC(arg0, arg0 + OBJ_POS / 4, (s32 *)(g_Player + OBJ_POS), NEAR_DIST, FAR_DIST, NEAR_DIST) == 0) {
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
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001D63C);
#endif

extern s32 *g_DisplayBuffer;

s32 func_8001E3DC(s32 arg0, s32 arg1, s32 arg2, s32 *arg3) {
    u32 n;
    s32 prod;
    s32 step;
    s32 diff;
    s32 dist;
    s32 res;

    n = g_DisplayBuffer[5];
    if (n >= 0x67) {
        n = 0x66;
    }
    prod = arg2 * n;
    step = prod / 1024;
    diff = (s32)((arg1 - arg0) << 20) >> 20;
    dist = diff;
    if (dist < 0) {
        dist = -dist;
    }
    if (arg3 != 0 && (arg3[0xAC / 4] & 0x20000000) && dist < step * 4) {
        if (dist < step * 2) {
            if (dist < step) {
                step = step / 8;
            } else {
                step = step / 4;
            }
        } else if (dist < step * 3) {
            step = step / 2;
        } else {
            step = (step * 3) / 4;
        }
    }
    if (dist < step) {
        arg0 = arg1;
        if (arg3 != 0) {
            arg3[0xA8 / 4] |= 0x800;
        }
    } else {
        if ((dist < 0 ? -dist : dist) == 0x800 && arg1 >= 0x800) {
            diff = -diff;
        }
        if (diff < 0) {
            arg0 = (arg0 - step) & 0xFFF;
            if (arg3 != 0) {
                arg3[0xA8 / 4] |= 8;
            }
        } else {
            arg0 = (arg0 + step) & 0xFFF;
            if (arg3 != 0) {
                arg3[0xA8 / 4] &= ~8;
            }
        }
    }
    return arg0;
}

#ifdef NON_MATCHING
#define SCALE_CAP 0x66
#define SCALE_SHIFT 10
#define SCALE_ROUND 0x3FF
#define ANGLE_MASK 0xFFF
#define ANGLE_SIGN_SHIFT 20
#define FLAG_REACHED 0x800
#define SCALE_OFFSET 0x14
#define FLAGS_OFFSET 0xA8

s32 func_8001E544(s32 arg0, s32 arg1, s32 arg2, void *arg3) {
    u32 scale;
    s32 product;
    s32 step;
    s32 diff;
    s32 dist;
    s32 snap;

    scale = *(u32 *)((u8 *)g_DisplayBuffer + SCALE_OFFSET);
    if (scale > SCALE_CAP) {
        scale = SCALE_CAP;
    }
    product = arg2 * scale;
    if (product < 0) {
        product += SCALE_ROUND;
    }
    step = product >> SCALE_SHIFT;

    diff = (s32)((arg1 - arg0) << ANGLE_SIGN_SHIFT) >> ANGLE_SIGN_SHIFT;
    dist = diff;
    if (diff < 0) {
        dist = -diff;
    }

    if (diff == 0) {
        snap = 1;
    } else if (step >= 0) {
        snap = dist < step && (step ^ diff) >= 0;
    } else {
        snap = dist < -step && (step ^ diff) >= 0;
    }

    if (snap) {
        if (arg3 != 0) {
            *(s32 *)((u8 *)arg3 + FLAGS_OFFSET) |= FLAG_REACHED;
        }
        return arg1;
    } else {
        return (arg0 + step) & ANGLE_MASK;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E544);
#endif

s32 func_8001E5FC(s32 arg0, s32 arg1, s32 arg2) {
    s32 diff;

    diff = arg1 - arg0;
    if (arg2 > 0 && (diff < 0 ? -diff : diff) < arg2) {
        return arg1;
    }
    if (diff > 0) {
        return arg0 + arg2;
    }
    return arg0 - arg2;
}

#ifdef NON_MATCHING
s32 func_8001E640(s32 arg0, s32 arg1, s32 arg2) {
    s32 a;
    s32 sh;
    s32 v;
    s32 c;

    a = arg0 & 0xFFF;
    sh = (arg1 > 0xFFFF) * 2;
    v = (Math_Sin(((a % arg1) << 11) / arg1 - 0x400) * arg2) >> 12;
    if (v < 0x1000) {
        c = -0x1000;
        if (v >= -0x1000) {
            c = v;
        }
    } else {
        c = 0x1000;
    }
    return (((c + 0x1000) >> sh) * arg1 >> (13 - sh)) + a / arg1 * arg1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E640);
#endif

#ifdef NON_MATCHING
extern void MulMatrix(s16 *, s16 *);
extern void ScaleMatrix(s16 *, s32);

void Math_MatrixFromAngles(s16 *arg0, s32 *arg1, s32 arg2) {
    s16 sp[16];
    s16 c;
    s32 s;
    s32 one;

    one = 0x1000;
    s = Math_Sin(arg1[2] & 0xFFF);
    c = Math_Cos(arg1[2] & 0xFFF);
    arg0[0] = c;
    arg0[1] = 0;
    arg0[2] = s;
    arg0[3] = 0;
    arg0[4] = one;
    arg0[5] = 0;
    arg0[6] = -s;
    arg0[7] = 0;
    arg0[8] = c;
    s = Math_Sin(arg1[0]);
    c = Math_Cos(arg1[0]);
    sp[0] = one;
    sp[1] = 0;
    sp[2] = 0;
    sp[3] = 0;
    sp[4] = c;
    sp[5] = -s;
    sp[6] = 0;
    sp[7] = s;
    sp[8] = c;
    MulMatrix(arg0, sp);
    s = Math_Sin(-((arg1[2] - arg1[1]) & 0xFFF));
    c = Math_Cos(-((arg1[2] - arg1[1]) & 0xFFF));
    sp[0] = c;
    sp[1] = 0;
    sp[2] = s;
    sp[3] = 0;
    sp[4] = one;
    sp[5] = 0;
    sp[6] = -s;
    sp[7] = 0;
    sp[8] = c;
    MulMatrix(arg0, sp);
    if (arg2 != 0) {
        ScaleMatrix(arg0, arg2);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", Math_MatrixFromAngles);
#endif

#ifdef NON_MATCHING
#define FIXED_SHIFT 4
#define MATRIX_ELEMS 9


void func_8001E8E4(s32 *arg0, s32 *arg1, s32 *arg2, s32 arg3, s32 *arg4) {
    s16 matrix[MATRIX_ELEMS];
    s32 vec[3];
    s32 rotated[3];

    Math_MatrixFromAngles(matrix, arg2, arg3);
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
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E8E4);
#endif

#ifdef NON_MATCHING
#define FIXED_SHIFT 4
#define POS_OFFSET 0x60
#define MATRIX_OFFSET 8

void func_8001E9EC(void *arg0, void *arg1, void *arg2) {
    s32 *in = (s32 *)arg1;
    s32 *out = (s32 *)arg2;
    s32 *base = (s32 *)((u8 *)arg0 + POS_OFFSET);
    s16 *matrix = *(s16 **)((u8 *)arg0 + MATRIX_OFFSET);
    s32 vec[3];
    s32 res[3];

    vec[0] = in[0] / (1 << FIXED_SHIFT);
    vec[1] = in[1] / (1 << FIXED_SHIFT);
    vec[2] = in[2] / (1 << FIXED_SHIFT);
    func_80018194(vec, matrix, res);
    out[0] = (res[0] << FIXED_SHIFT) + base[0];
    out[1] = (res[1] << FIXED_SHIFT) + base[1];
    out[2] = (res[2] << FIXED_SHIFT) + base[2];
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001E9EC);
#endif

#ifdef NON_MATCHING
#define MATRIX_ONE 0x1000
#define CALL_RESULT -0xFF

extern s32 g_CameraAngleZ;
extern s32 D_80060914;

/* Retail returns CALL_RESULT; the declared signature is void. */
void func_8001EAD8(s32 *arg0, s32 *arg1) {
    s16 a[9];
    s16 b[9];
    s32 s;
    s32 c;

    s = Math_Sin(-g_CameraAngleZ);
    c = Math_Cos(-g_CameraAngleZ);
    a[0] = c; a[1] = -s; a[2] = 0;
    a[3] = s; a[4] = c;  a[5] = 0;
    a[6] = 0; a[7] = 0;  a[8] = MATRIX_ONE;

    s = Math_Sin(-D_80060914);
    c = Math_Cos(-D_80060914);
    b[0] = c; b[1] = 0;          b[2] = s;
    b[3] = 0; b[4] = MATRIX_ONE; b[5] = 0;
    b[6] = -s; b[7] = 0;         b[8] = c;

    MulMatrix(a, b);

    /* b = transpose of a */
    b[0] = a[0]; b[1] = a[3]; b[2] = a[6];
    b[3] = a[1]; b[4] = a[4]; b[5] = a[7];
    b[6] = a[2]; b[7] = a[5]; b[8] = a[8];
    func_80018194(arg0, b, arg1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001EAD8);
#endif

#ifdef NON_MATCHING
extern s32 RotTrans(s16 *, s32 *, s32 *, s32);
extern void SetRotMatrix(void *);
extern void SetTransMatrix(void *);
extern char D_80060130;
extern char g_ViewMatrix;
extern char D_800607D4;
extern s32 g_CameraX;
extern s32 g_CameraY;
extern s32 g_CameraZ;
extern s32 D_8006083C;
extern s32 D_80060840;
extern s32 D_80060844;

s32 Gfx_WorldToView(s32 *arg0, s32 *arg1, s32 arg2) {
    s16 sv[3];
    s32 wv[3];
    s32 out[2];
    void *m;
    s32 a2;
    s32 a3;
    s32 x;

    wv[0] = arg0[0];
    wv[1] = arg0[1];
    m = &g_ViewMatrix;
    wv[2] = arg0[2];
    if (arg2 != 0) {
        m = &D_800607D4;
    }
    SetRotMatrix(m);
    SetTransMatrix(&D_80060130);
    if (arg2 != 0) {
        a2 = D_80060840;
        a3 = D_80060844;
        x = wv[0] - D_8006083C;
    } else {
        a2 = g_CameraY;
        a3 = g_CameraZ;
        x = wv[0] - g_CameraX;
    }
    sv[0] = x >> 8;
    sv[1] = (wv[1] - a2) >> 8;
    sv[2] = (wv[2] - a3) >> 8;
    RotTrans(sv, arg1, out, a3);
    a2 = arg1[0];
    a3 = arg1[1];
    x = arg1[2];
    arg1[0] = a2 << 8;
    arg1[1] = -(a3 << 8);
    arg1[2] = x << 8;
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", Gfx_WorldToView);
#endif

#ifdef NON_MATCHING
#define POS_SHIFT 8
#define SCREEN_CENTER_X 0x100
#define SCREEN_CENTER_Y 0x6C
#define SXY_Y_SHIFT 16
#define RESULT_CONST (-0xFF)

extern s32 ReadSZfifo3(s32 *, s32 *, s32 *);
extern s32 RotTransPers(s16 *, s32 *, s32 *, s32 *);

s32 Gfx_WorldToScreen(s32 *arg0, s32 *arg1) {
    /* One struct pins the stack slots the callees see. */
    struct {
        s16 vec[3];
        s32 pos[4];
        s32 sxy;
        s32 p;
        s32 flag;
        s32 sz[3];
        s32 pad[2];
    } l;

    l.pos[0] = arg0[0];
    l.pos[1] = arg0[1];
    l.pos[2] = arg0[2];
    SetRotMatrix(&g_ViewMatrix);
    SetTransMatrix(&D_80060130);
    l.vec[2] = (l.pos[2] - g_CameraZ) >> POS_SHIFT;
    l.vec[0] = (l.pos[0] - g_CameraX) >> POS_SHIFT;
    l.vec[1] = (l.pos[1] - g_CameraY) >> POS_SHIFT;
    RotTransPers(l.vec, &l.sxy, &l.p, &l.flag);
    arg1[0] = ((s16)l.sxy - SCREEN_CENTER_X) << POS_SHIFT;
    arg1[1] = (SCREEN_CENTER_Y - (l.sxy >> SXY_Y_SHIFT)) << POS_SHIFT;
    ReadSZfifo3(&l.sz[0], &l.sz[1], &l.sz[2]);
    arg1[2] = l.sz[2] << POS_SHIFT;
    return RESULT_CONST;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", Gfx_WorldToScreen);
#endif

#ifdef NON_MATCHING
#define ENTRY_COUNT 0x60
#define ENTRY_SIZE 0x23C
#define POOL_BYTES 0xD680
#define HEADER_BYTES 0x33C
#define SLOT_COUNT 8
#define ERR_ALLOC (-0xF)
#define ERR_INIT (-0xFF)

extern s32 g_Player;
extern s32 g_PlayerSlot;
extern s32 g_ObjPool;
extern s32 g_ObjLists[];
extern s32 g_ObjFreeList;
extern s32 D_8006CB60;
extern s32 D_8006CB64;

/* g_PlayerSlot and g_ObjPool are adjacent; the table offsets below are taken
   from their addresses, not from the blocks they point to. */
s32 Obj_InitPool(void) {
    s32 i;
    s32 pool;
    s32 header;
    s32 entry;
    s32 next;
    s32 list;
    s32 ring;
    s32 *p;

    g_Player = 0;
    g_ObjPool = Heap_Alloc(POOL_BYTES);
    g_PlayerSlot = Heap_Alloc(HEADER_BYTES);
    if (g_ObjPool == 0 || g_PlayerSlot == 0) {
        return ERR_ALLOC;
    }
    D_8006CB60 = 0;
    D_8006CB64 = 0;
    ring = (s32)&g_ObjPool + 0x1468;
    *(s32 *)(g_PlayerSlot + 8) = ring;
    g_ObjFreeList = 2;
    *(s32 *)((s32)&g_ObjPool + 0x48) = g_ObjPool;
    list = (s32)&g_PlayerSlot + 0x14D8;
    next = ENTRY_SIZE;
    for (i = 0; i < ENTRY_COUNT; i++) {
        entry = g_ObjPool + i * ENTRY_SIZE;
        *(s32 *)(entry + 8) = list;
        *(s32 *)entry = 0;
        *(s32 *)(entry + 0x44) = (s32)&g_PlayerSlot + 0x48;
        *(s32 *)(entry + 0x4C) = 0;
        if (i < ENTRY_COUNT - 1) {
            *(s32 *)(entry + 0x48) = g_ObjPool + next;
        } else {
            *(s32 *)(entry + 0x48) = 0;
        }
        next += ENTRY_SIZE;
        list += 0x30;
    }
    p = g_ObjLists;
    for (i = 0; i < SLOT_COUNT; i++) {
        p[0] = 2;
        p[1] = 0;
        p += 2;
    }
    *(s32 *)g_PlayerSlot = 0;
    *(s32 *)(g_PlayerSlot + 0x44) = 0;
    *(s32 *)(g_PlayerSlot + 0x4C) = 0;
    *(s32 *)(g_PlayerSlot + 0x48) = 0;
    return ERR_INIT;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", Obj_InitPool);
#endif

extern s32 g_Player;
extern s32 g_PlayerSlot;
extern s32 g_ObjPool;

void Obj_FreePool(void) {
    Heap_Free(g_ObjPool);
    g_Player = 0;
    Heap_Free(g_PlayerSlot);
}

extern void *D_8006759C;
extern s32 g_GoolGlobals;

s32 func_8001F018(void) {
    g_GoolGlobals = *(s32 *)((s8 *)D_8006759C + 8) << 8;
    if (*(s32 *)((s8 *)D_8006759C + 4) == 0) {
        return -0xE;
    }
    return -0xFF;
}

#ifdef NON_MATCHING
#define NODE_TYPE_SHIFTED 7
#define NODE_SHIFT_SHIFTED 2
#define PATH_KIND 0x4B
#define PATH_ARG_SHIFT 2
#define FRAC_BITS 8
#define FRAC_ONE 0x100
#define FRAC_MAX 0xFF
#define FRAC_MASK 0xFF
#define STEP_SHIFT 4
#define ERR_NO_PATH -0xE
#define FLAG_AC_FACE 0x4
#define FLAG_AC_MIRROR 0x2
#define FLAG_AC_PITCH 0x800
#define FLAG_AC_SEGMENT 0x8000
#define FLAG_A8_EDGE 0x4
#define FLAG_A8_EDGE_CHANGED 0x10
#define FLAG_A8_OFF_SEGMENT 0x200
#define PATH_POINT_SIZE 3
#define INDEX_SIGN_SHIFT 8

struct Unk8001F054Node {
    /* 0x00 */ u8 unk_00[4];
    /* 0x04 */ struct Unk8001F054Info *unk_04;
};

struct Unk8001F054Info {
    /* 0x00 */ u8 unk_00[8];
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ u8 unk_0C[8];
    /* 0x14 */ s32 *unk_14;
};

s32 func_8001F054(GoolObject *arg0, u32 arg1, s32 *out) {
    GoolObject *obj;
    s32 pos;
    s32 *base;
    s32 shift;
    s32 count;
    s32 seg;
    s32 frac;
    s32 len;
    s32 t;
    s32 t4;
    s32 delta[3];
    s32 da;
    s32 db;
    s32 flags;
    s32 oldA8;
    s32 curA8;
    s32 next[3];
    s32 ax;
    s32 az;
    s32 posBase;
    s16 *pt;
    struct Unk8001F054Node *node;
    struct Unk8001F054Info *info;

    obj = arg0;
    pos = arg1;
    node = obj->entity;
    if (obj == 0 || node == 0) {
        return ERR_NO_PATH;
    }
    info = node->unk_04;
    /* base and shift stay unset for any other node type, as in the retail code */
    if (info->unk_08 == NODE_TYPE_SHIFTED) {
        base = info->unk_14;
        shift = NODE_SHIFT_SHIFTED;
    }
    count = Ent_GetPropertyRowLength((s32)node, PATH_KIND, 0, PATH_ARG_SHIFT);
    while (1) {
        if (pos >= 0) {
            seg = pos >> FRAC_BITS;
        } else {
            seg = -pos >> FRAC_BITS;
        }
        t = Ent_GetPropertyRowLength((s32)node, PATH_KIND, 0, PATH_ARG_SHIFT);
        frac = pos & FRAC_MASK;
        if (seg == t - 1 && seg != 0) {
            frac = (pos & FRAC_MASK) + FRAC_ONE;
            seg = t - 2;
        }
        pt = (s16 *)Ent_GetProperty((s32)node, PATH_KIND, 0, 0, 0, PATH_ARG_SHIFT, 0, 0, 0);
        /* The segment index keeps its low 24 bits, sign-extended */
        pt += ((seg << INDEX_SIGN_SHIFT) >> INDEX_SIGN_SHIFT) * PATH_POINT_SIZE;
        out[0] = ((pt[0] << shift) + base[0]) << FRAC_BITS;
        out[1] = ((pt[1] << shift) + base[1]) << FRAC_BITS;
        out[2] = ((pt[2] << shift) + base[2]) << FRAC_BITS;
        if (count == 1) {
            return 1;
        }
        pt += PATH_POINT_SIZE;
        next[0] = ((pt[0] << shift) + base[0]) << FRAC_BITS;
        next[1] = ((pt[1] << shift) + base[1]) << FRAC_BITS;
        next[2] = ((pt[2] << shift) + base[2]) << FRAC_BITS;
        delta[0] = next[0] - out[0];
        delta[1] = next[1] - out[1];
        delta[2] = next[2] - out[2];
        if (obj->status_b & FLAG_AC_SEGMENT) {
            ax = delta[0] >> FRAC_BITS;
            az = delta[2] >> FRAC_BITS;
            len = func_80041A78(ax * ax + az * az);
            posBase = pos & ~FRAC_MASK;
            if (len != 0) {
                t = ((((obj->pos[0] - out[0]) >> STEP_SHIFT) * (delta[0] >> STEP_SHIFT)) +
                     (((obj->pos[2] - out[2]) >> STEP_SHIFT) * (delta[2] >> STEP_SHIFT))) / len;
                frac = t / len;
                if (frac >= FRAC_ONE && (pos >> FRAC_BITS) < count - 1) {
                    pos = (seg + 1) << FRAC_BITS;
                    continue;
                }
                t4 = t >> STEP_SHIFT;
                ax = ((t4 * (delta[0] >> STEP_SHIFT)) / len) + out[0];
                az = ((t4 * (delta[2] >> STEP_SHIFT)) / len) + out[2];
                da = ax - obj->pos[0];
                if (da < 0) {
                    da = obj->pos[0] - ax;
                }
                db = az - obj->pos[2];
                if (db < 0) {
                    db = obj->pos[2] - az;
                }
                /* Octagon-style distance estimate: max + min / 2 */
                if (db < da) {
                    obj->unk_9C[1] = da + db / 2;
                } else {
                    obj->unk_9C[1] = db + da / 2;
                }
                if (obj->unk_134 < obj->unk_9C[1] || (frac < 0 && seg == 0) || frac >= FRAC_ONE) {
                    obj->status_a |= FLAG_A8_OFF_SEGMENT;
                }
                if (((delta[2] * (obj->pos[0] >> FRAC_BITS)) - (delta[0] * (obj->pos[2] >> FRAC_BITS)) -
                     (next[2] * (out[0] >> FRAC_BITS)) + (next[0] * (out[2] >> FRAC_BITS))) < 0) {
                    obj->unk_9C[1] = -obj->unk_9C[1];
                }
            } else {
                obj->status_a |= FLAG_A8_OFF_SEGMENT;
            }
            if (frac < FRAC_MAX) {
                obj->misc = posBase;
                if (frac >= 0) {
                    obj->misc = posBase + frac;
                }
            } else {
                obj->misc = posBase + FRAC_MAX;
            }
            if (len == 0) {
                obj->unk_9C[1] = 0;
            }
        }
        break;
    }

    oldA8 = obj->status_a;
    if (obj->unk_F4 < 0 || (pos >> FRAC_BITS) >= count - 1) {
        obj->status_a = oldA8 | FLAG_A8_EDGE;
    } else {
        obj->status_a = oldA8 & ~FLAG_A8_EDGE;
    }
    curA8 = obj->status_a;
    if ((curA8 & FLAG_A8_EDGE) != (oldA8 & FLAG_A8_EDGE) && !(curA8 & FLAG_A8_EDGE_CHANGED)) {
        obj->status_a = curA8 | FLAG_A8_EDGE_CHANGED;
    } else {
        obj->status_a = obj->status_a & ~FLAG_A8_EDGE_CHANGED;
    }

    flags = obj->status_b;
    if (flags & FLAG_AC_FACE) {
        if ((flags & FLAG_AC_MIRROR) && (obj->status_a & FLAG_A8_EDGE)) {
            obj->target_rot[1] = ratan2(-delta[0], -delta[2]);
        } else {
            obj->target_rot[1] = ratan2(delta[0], delta[2]);
        }
        if (obj->status_b & FLAG_AC_PITCH) {
            obj->rot[2] = ratan2(delta[0], delta[2]);
            if (delta[0] < 0) {
                delta[0] = -delta[0];
            }
            if (delta[2] < 0) {
                delta[2] = -delta[2];
            }
            if (delta[2] < delta[0]) {
                da = delta[0] + delta[2] / 2;
            } else {
                da = delta[2] + delta[0] / 2;
            }
            obj->target_rot[2] = -ratan2(delta[1], da);
        }
    } else if (flags & FLAG_AC_PITCH) {
        if (delta[0] < 0) {
            da = -delta[0];
        } else {
            da = delta[0];
        }
        if (delta[2] < 0) {
            db = -delta[2];
        } else {
            db = delta[2];
        }
        if (db < da) {
            da = da + db / 2;
        } else {
            da = db + da / 2;
        }
        obj->target_rot[2] = ratan2(delta[1], da);
        obj->rot[2] = ratan2(-delta[0], -delta[2]);
    } else {
        /* neither flag: no angles are written */
    }

    out[0] += ((next[0] - out[0]) * frac) >> FRAC_BITS;
    out[1] += ((next[1] - out[1]) * frac) >> FRAC_BITS;
    out[2] += ((next[2] - out[2]) * frac) >> FRAC_BITS;
    return count;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", func_8001F054);
#endif

extern u16 g_PermFlags[];
extern s32 g_EntityFlags[];

void func_8001F858(s32 arg0) {
    u16 *p;

    for (p = g_PermFlags; *p != 0; p++) {
        if ((*p >> 9) == arg0) {
            g_EntityFlags[*p & 0x1FF] |= 8;
        }
    }
}

#ifdef NON_MATCHING
#define SLOT_FREE 1
#define OWNER_OFFSET 0x10
#define FLAGS_OFFSET 0x29C
#define FLAG_LOCKED 0x2000
#define KEY_HI_SHIFT 9
#define KEY_LO_SHIFT 8

extern char *g_CurrentZone;

void Ent_SetPermFlag(s32 arg0) {
    u16 *p;
    u16 *free_slot;
    u16 key;

    key = (arg0 >> KEY_LO_SHIFT) | (*(s32 *)((char *)D_8006759C + 8) << KEY_HI_SHIFT);
    free_slot = 0;
    p = g_PermFlags;
    if (*(s32 *)(*(char **)(g_CurrentZone + OWNER_OFFSET) + FLAGS_OFFSET) & FLAG_LOCKED) {
        return;
    }
    for (; *p != 0; p++) {
        if (*p == key) {
            return;
        }
        if (*p == SLOT_FREE) {
            free_slot = p;
        }
    }
    if (free_slot == 0) {
        free_slot = p;
    }
    *free_slot = key;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", Ent_SetPermFlag);
#endif

#ifdef NON_MATCHING
#define KEY_SHIFT_LOW 8
#define KEY_SHIFT_HIGH 9
#define KEY_MASK 0xFFFF


void Ent_ClearPermFlag(s32 arg0) {
    u16 *p;
    s32 key;

    key = (arg0 >> KEY_SHIFT_LOW) | (((s32 *)D_8006759C)[2] << KEY_SHIFT_HIGH);
    key &= KEY_MASK;
    for (p = g_PermFlags; *p != 0; p++) {
        if (*p == key) {
            if (p[1] != 0) {
                *p = 1;
            } else {
                *p = 0;
            }
            return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001D63C", Ent_ClearPermFlag);
#endif
