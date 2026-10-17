#include "common.h"
#include "sdk.h"
#include "game.h"

#define POS_TOLERANCE_INPUT 10   /* snap window for the stored input vector */
#define POS_TOLERANCE_OUTPUT 2   /* window for "position unchanged" after the move */
#define SPEED_FLAGS_MASK 0x20000000
#define SPEED_DEFAULT 0xF424
#define SPEED_BOOSTED 0x27AC4
#define FLAG_ACTIVE 0x400
#define STATE_MAX_SIMPLE 0x3FFFF
#define STATE_RANGE_END 0x40024
#define STATE_RANGE_BASE 0x4001D
#define SHIFT_COUNT_MASK 31
#define STATE_ARG_DEFAULT 0x6400
#define LEVEL_FLOOR_MARGIN 0x19000
#define NEAR_ABS(a, b, n) ((((a) - (b)) >= 0) ? (((a) - (b)) < (n)) : (((b) - (a)) < (n)))

extern void *D_8006759C;
extern s32 D_8006CD48;
extern s32 D_8005F204;
extern s32 D_8005F2D0;
extern s32 D_8005F4A0;
extern s32 D_8005F4A4;
extern s32 D_8005F4A8;
void func_80029D0C(void *, s32, s32 *, s32 *);

void func_80029884(void *arg0, s32 arg1, s32 arg2) {
    s32 pos[4];
    s32 vel[4];
    s32 shiftArg[2];
    s32 *in = (s32 *)arg1;
    u32 state;
    s32 speed;
    u32 kind;

    if (arg0 == 0) {
        D_8005F2D0 = 0;
        return;
    }
    *(s32 *)((u8 *)*(void **)((u8 *)arg0 + 8) + 0x48) = 0;
    kind = *(u32 *)((u8 *)D_8006759C + 8);
    if (kind == 0x1D || kind == 0x17 || kind == 0x22 || kind == 0x25) {
        if (*(s32 *)((u8 *)arg0 + 0x100) & SPEED_FLAGS_MASK) {
            speed = SPEED_BOOSTED;
        } else {
            speed = SPEED_DEFAULT;
        }
    } else {
        speed = SPEED_DEFAULT;
    }
    D_8005F204 = speed;
    pos[0] = *(s32 *)((u8 *)arg0 + 0x60);
    pos[1] = *(s32 *)((u8 *)arg0 + 0x64);
    pos[2] = *(s32 *)((u8 *)arg0 + 0x68);
    vel[0] = in[0];
    vel[1] = in[1];
    vel[2] = in[2];
    if (D_8005F2D0 != 0 && NEAR_ABS(D_8005F4A0, in[0], POS_TOLERANCE_INPUT) &&
        NEAR_ABS(D_8005F4A4, in[1], POS_TOLERANCE_INPUT) &&
        NEAR_ABS(D_8005F4A8, in[2], POS_TOLERANCE_INPUT)) {
        vel[0] = 0;
        vel[2] = 0;
    }
    func_80029D0C(arg0, arg2, pos, vel);
    if (D_8005F2D0 == 0 && (in[0] != 0 || in[2] != 0) &&
        NEAR_ABS(*(s32 *)((u8 *)arg0 + 0x60), pos[0], POS_TOLERANCE_OUTPUT) &&
        NEAR_ABS(*(s32 *)((u8 *)arg0 + 0x64), pos[1], POS_TOLERANCE_OUTPUT) &&
        NEAR_ABS(*(s32 *)((u8 *)arg0 + 0x68), pos[2], POS_TOLERANCE_OUTPUT)) {
        D_8005F4A0 = in[0];
        D_8005F4A4 = in[1];
        D_8005F2D0 = 1;
        D_8005F4A8 = in[2];
    } else {
        D_8005F2D0 = 0;
    }
    *(s32 *)((u8 *)arg0 + 0x60) = pos[0];
    *(s32 *)((u8 *)arg0 + 0x64) = pos[1];
    *(s32 *)((u8 *)arg0 + 0x68) = pos[2];
    if (!(*(s32 *)((u8 *)arg0 + 0xA8) & FLAG_ACTIVE)) {
        return;
    }
    state = *(u32 *)((u8 *)arg0 + 0x11C);
    if (state > STATE_MAX_SIMPLE) {
        if (state > STATE_RANGE_END) {
            shiftArg[0] = (state - STATE_RANGE_END) << 8;
            func_80025FDC(0x100, shiftArg);
        } else {
            shiftArg[0] = 0x100 << ((state - STATE_RANGE_BASE) & SHIFT_COUNT_MASK);
            func_80025FDC(0, shiftArg);
        }
        return;
    }
    shiftArg[1] = STATE_ARG_DEFAULT;
    switch (state) {
    case 0x3000:
    case 0x3100:
        func_8001D194(0, state, 0, 0, 0);
        return;
    case 0x1F00:
        if (*(s32 *)((u8 *)arg2 + 0x1040) != 0) {
            return;
        }
        break;
    case 0x900:
        if (*(s32 *)((u8 *)arg0 + 0xA8) & 1) {
            return;
        }
        break;
    case 0x4200:
        if (!(*(s32 *)((u8 *)arg0 + 0x64) < (D_8006CD48 - LEVEL_FLOOR_MARGIN))) {
            return;
        }
        *(u32 *)((u8 *)arg0 + 0x11C) = 0x2100;
        shiftArg[1] = D_8006CD48;
        break;
    default:
        break;
    }
    func_8001CC10(0, (s32)arg0, *(u32 *)((u8 *)arg0 + 0x11C), 1, &shiftArg[1]);
}
