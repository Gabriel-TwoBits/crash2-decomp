#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800236DC(s32 *arg0, s32 *arg1, s32 arg2) {
    s32 cur;
    s32 diff;
    s32 step;
    s32 vel;
    s32 limit;
    s32 t;

    cur = arg0[0];
    if (arg2 == 0) {
        arg2 = arg0[2];
    }
    if (arg2 != 0) {
        step = (~arg2 >> 31) & 0x400;
        diff = step - cur;
        if (cur != step) {
            t = arg1[0];
            vel = arg0[1];
            step = (diff >= 0) ? diff : -diff;
            if (t >= step) {
                step = step >> arg1[3];
                if (step == 0) {
                    step = 1;
                }
                diff = (diff < 0) ? -step : step;
            }
            limit = arg1[1];
            if (limit < step) {
                t = limit;
                if (diff < 0) {
                    t = -t;
                }
                diff = t;
            }
            if (vel < diff) {
                vel += arg1[2];
                if (diff < vel) {
                    vel = diff;
                }
            } else if (diff < vel) {
                vel -= arg1[2];
                if (vel < diff) {
                    vel = diff;
                }
            }
            cur += vel;
            if (cur < 0) {
                cur = 0;
            } else if (cur >= 0x401) {
                cur = 0x400;
            }
            arg0[1] = vel;
            arg0[0] = cur;
        } else {
            arg0[1] = 0;
        }
    }
    return cur;
}
