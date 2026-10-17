#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
#define FIELD(base, type, off) (*(type *)((u8 *)(base) + (off)))

#define STATE_OFF 0x108
#define STATE_TIME_OFF 0x10C
#define FLAGS_A8_OFF 0xA8
#define FLAGS_AC_OFF 0xAC
#define FLAGS_100_OFF 0x100
#define ANIM_OFF 0xE8
#define SUB_OFF 0x20

#define FLAG_AC_DONE 0x80
#define FLAG_A8_ABORT 0x1
#define FLAG_100_ABORT 0x20000000

#define STATE_IDLE 0
#define STATE_SKIP 1
#define STATE_SPIN 2
#define STATE_WAIT_1C4 3
#define STATE_WAIT_3D 4
#define STATE_WAIT_25B 5
#define STATE_FINISH_A 6
#define STATE_FINISH_B 7

#define LIMIT_FINISH 0x10
#define LIMIT_3D 0x3D
#define LIMIT_1C4 0x1C4
#define LIMIT_25B 0x25B

#define SPIN_SIZE 0x3C
#define SPIN_PERIOD 4
#define SPIN_SHIFT 10
#define ANIM_END_TYPE 4
#define RETURN_VALUE (-0xFF)

extern s32 D_80060944;
extern u32 D_8006CB64;


static inline void spin(u8 *s) {
    s32 t = D_80060944;
    s32 a = t;

    if (t < 0) {
        a = t + (SPIN_PERIOD - 1);
    }
    FIELD(s, s16, 0x14) = SPIN_SIZE;
    FIELD(s, s16, 0x16) = SPIN_SIZE;
    FIELD(s, s16, 0x18) = SPIN_SIZE;
    FIELD(s, s16, 4) = (t - ((a >> 2) * SPIN_PERIOD)) << SPIN_SHIFT;
}

static inline void copy4(u8 *s, s32 from, s16 sel) {
    FIELD(s, s16, 6) = sel;
    FIELD(s, s16, 0x1A) = FIELD(s, s16, from);
    FIELD(s, s16, 0x1C) = FIELD(s, s16, from + 2);
    FIELD(s, s16, 0x1E) = FIELD(s, s16, from + 4);
}

s32 func_8001BFDC(void *arg0) {
    u8 *s = (u8 *)arg0 + SUB_OFF;
    u8 *anim;
    s16 v;
    u32 elapsed;

    switch (FIELD(arg0, s32, STATE_OFF)) {
    case STATE_SKIP:
        break;
    case STATE_SPIN:
        spin(s);
        break;
    case STATE_WAIT_1C4:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_1C4) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
        } else {
            /* state stays 3: the later state checks cannot fire */
        }
        spin(s);
        break;
    case STATE_WAIT_3D:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_3D) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
        } else {
            /* keep waiting */
        }
        break;
    case STATE_WAIT_25B:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_25B || (FIELD(arg0, s32, FLAGS_100_OFF) & FLAG_100_ABORT)) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
        } else {
            /* keep waiting */
        }
        spin(s);
        break;
    case STATE_FINISH_A:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_FINISH) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
            FIELD(arg0, s32, FLAGS_AC_OFF) |= FLAG_AC_DONE;
        } else {
            /* keep waiting */
        }
        break;
    case STATE_FINISH_B:
        elapsed = D_8006CB64 - FIELD(arg0, u32, STATE_TIME_OFF);
        if (elapsed >= LIMIT_FINISH || (FIELD(arg0, s32, FLAGS_A8_OFF) & FLAG_A8_ABORT)) {
            FIELD(arg0, s32, STATE_OFF) = STATE_IDLE;
            FIELD(arg0, s32, FLAGS_AC_OFF) |= FLAG_AC_DONE;
        } else {
            /* keep waiting */
        }
        break;
    default:
        func_8001BFD4(arg0, 1);
        break;
    }

    anim = FIELD(arg0, u8 *, ANIM_OFF);
    if (anim == 0) {
        return RETURN_VALUE;
    } else if (*anim == ANIM_END_TYPE) {
        return RETURN_VALUE;
    } else {
        copy4(s, 0xE, FIELD(s, s16, 2));
        v = FIELD(s, s16, 0);
        if (v != 0) {
            copy4(s, 8, v);
        } else {
            /* no override */
        }
        v = FIELD(s, s16, 4);
        if (v != 0) {
            copy4(s, 0x14, v);
        } else {
            /* no override */
        }
        return RETURN_VALUE;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001BFDC);
#endif

extern u8 D_8005C514;
extern s32 D_8005F38C;
extern s32 D_80060944;
extern u32 *D_80063454;
extern u32 D_8006CB64;
extern s32 D_8006CCAC;
extern u8 func_80018CAC;
extern u8 func_8001C718;

void func_8001C29C(void *arg0) {
    D_8006CB64 = D_80063454[3] / 34;
    func_8002CA98();
    D_8006CCAC = D_80060944;
    func_80019F08();
    if (D_8005F38C != 0) {
        func_80018DD0(&func_80018CAC, &func_8001C718, arg0);
    }
    *(u8 **)0x1F80005C = &D_8005C514;
}

INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C340);

#ifdef NON_MATCHING
s32 func_8001C620(u8 *arg0, s32 arg1, s32 arg2) {
    u8 *cur;
    s32 prev;
    s32 *p;

    cur = *(u8 **)(arg0 + 0xBC);
    p = *(s32 **)(arg0 + 0xBC);
    prev = *(s32 *)(arg0 + 0xC4);
    *(u8 **)(arg0 + 0xC4) = cur;
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = arg2;
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = *(s32 *)(arg0 + 0xC0);
    p = *(s32 **)(arg0 + 0xBC);
    *(s32 **)(arg0 + 0xBC) = p + 1;
    *p = ((cur - arg1 * 4 - 0x40) - arg0) | ((prev - 0x40 - (s32)arg0) << 16);
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C620);
#endif

#ifdef NON_MATCHING
s32 func_8001C688(u8 *arg0, u32 *arg1) {
    u32 temp_a3;
    u32 temp_t0;
    u32 *v1;
    u32 *v1b;
    u32 w;

    v1 = *(u32 **)(arg0 + 0xC4);
    *(u32 **)(arg0 + 0xBC) = v1 + 2;
    temp_a3 = v1[2];
    temp_t0 = temp_a3 >> 16;
    if (temp_t0 == 0) {
        return -0x19;
    }
    *(u32 **)(arg0 + 0xBC) = v1 + 1;
    *(u32 *)(arg0 + 0xC0) = v1[1];
    v1b = *(u32 * volatile *)(arg0 + 0xBC);
    *(u32 **)(arg0 + 0xBC) = v1b - 1;
    w = v1b[-1];
    if (arg1 != 0) {
        *arg1 = (*arg1 & 0xFFFF0000) | (w & 0xFFFF);
    }
    *(u8 **)(arg0 + 0xBC) = &arg0[(temp_a3 & 0xFFFF) + 0x40];
    *(u8 **)(arg0 + 0xC4) = &arg0[temp_t0 + 0x40];
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C688);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001C718);

INCLUDE_ASM("asm/nonmatchings/game/8001BFDC", func_8001CC10);
