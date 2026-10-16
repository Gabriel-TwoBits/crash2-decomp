#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800236DC(s32 *arg0, s32 *arg1, s32 arg2) {
    s32 cur;
    s32 goal;
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
        goal = (~arg2 >> 31) & 0x400;
        diff = goal - cur;
        if (cur != goal) {
            step = diff;
            if (diff < 0) {
                step = -step;
            }
            vel = arg0[1];
            if (arg1[0] >= step) {
                step = step >> arg1[3];
                if (step == 0) {
                    step = 1;
                }
                if (diff < 0) {
                    t = -step;
                } else {
                    t = step;
                }
                diff = t;
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
