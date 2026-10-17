#include "common.h"
#include "sdk.h"
#include "game.h"

#define SEGMENT_LENGTH_QUERY 0x4B
#define SPEED_QUERY 0xC9
#define POSITION_SHIFT 8
#define DIR_FORWARD 1
#define DIR_BACKWARD 2
#define SPEED_MIN_DISTANCE 9
#define SPEED_MAX_DISTANCE 0x7D000
#define SPEED_DIVISOR 512000
#define SPEED_NUMERATOR 300
#define SPEED_BASE 100
#define SPEED_FRAC_SHIFT 16

typedef struct Unk80023E60 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
} Unk80023E60;

void func_80023E60(s32 *path, s32 arg1, s32 *out, s32 arg3) {
    Unk80023E60 *arg0 = (Unk80023E60 *)path;
    Unk80023E60 *arg2 = (Unk80023E60 *)out;
    Unk80023E60 *last;
    Unk80023E60 *p;
    s32 dist;
    s32 a;
    s32 i;
    s32 speed;
    s32 end;
    s32 pos;

    last = &arg0[arg1 - 1];
    if (arg0->unk4 == last->unk4) {
        dist = last->unk8 - arg0->unk8;
        if (dist < 0) {
            dist = -dist;
        }
    } else {
        a = arg0->unk8;
        if (arg0->unkC == DIR_BACKWARD) {
            a = ((func_800322C0(arg0->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - a;
        }
        dist = a;
        a = last->unk8;
        if (last->unkC == DIR_FORWARD) {
            a = ((func_800322C0(last->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - a;
        }
        dist += a;
        i = arg1 - 2;
        if (i > 0) {
            p = arg0 + 1;
            do {
                dist = dist + ((func_800322C0(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - 2;
                p++;
                i--;
            } while (i != 0);
        }
    }
    if (arg3 < 0) {
        speed = func_800321D4(arg0->unk4, SPEED_QUERY, 0, 0, 0, 2, 0);
        end = (dist * speed) >> POSITION_SHIFT;
        a = end;
        if (end < SPEED_MIN_DISTANCE) {
            *arg2 = *last;
            return;
        }
        if (a > SPEED_MAX_DISTANCE) {
            a = SPEED_MAX_DISTANCE;
        }
        a = (((a * SPEED_NUMERATOR) / SPEED_DIVISOR + SPEED_BASE) << SPEED_FRAC_SHIFT) / speed;
        p = arg0;
        if (a < dist) {
            dist = a;
        }
    } else {
        dist = (dist * arg3) >> POSITION_SHIFT;
        p = arg0;
    }
    if (p->unkC == DIR_BACKWARD) {
        end = (func_800322C0(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT;
    } else {
        end = 0;
    }
    pos = p->unk8;
    while (1) {
        if (p->unkC == DIR_BACKWARD) {
            a = end - pos;
            if (a >= dist) {
                pos += dist;
                break;
            }
            dist -= a;
        } else {
            if (pos >= dist) {
                pos -= dist;
                break;
            }
            dist -= pos;
        }
        p++;
        a = (func_800322C0(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT;
        if (p->unkC == DIR_BACKWARD) {
            end = a;
            pos = 1;
        } else {
            end = 0;
            pos = a - 1;
        }
    }
    arg2->unk0 = p->unk0;
    arg2->unk8 = pos;
    arg2->unk4 = p->unk4;
    arg2->unkC = p->unkC;
}
