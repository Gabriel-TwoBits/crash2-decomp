#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80023E60 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
} Unk80023E60;

void func_80023E60(Unk80023E60 *arg0, s32 arg1, Unk80023E60 *arg2, s32 arg3) {
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
        if (arg0->unkC == 2) {
            a = ((func_800322C0(arg0->unk4, 0x4B, 0, 2) - 1) << 8) - a;
        }
        dist = a;
        a = last->unk8;
        if (last->unkC == 1) {
            a = ((func_800322C0(last->unk4, 0x4B, 0, 2) - 1) << 8) - a;
        }
        dist += a;
        i = arg1 - 2;
        if (i > 0) {
            p = arg0 + 1;
            do {
                dist = dist + ((func_800322C0(p->unk4, 0x4B, 0, 2) - 1) << 8) - 2;
                p++;
                i--;
            } while (i != 0);
        }
    }
    if (arg3 < 0) {
        speed = func_800321D4(arg0->unk4, 0xC9, 0, 0, 0, 2, 0);
        end = (dist * speed) >> 8;
        a = end;
        if (end < 9) {
            *arg2 = *last;
            return;
        }
        if (a > 0x7D000) {
            a = 0x7D000;
        }
        a = (((a * 300) / 512000 + 100) << 16) / speed;
        p = arg0;
        if (a < dist) {
            dist = a;
        }
    } else {
        dist = (dist * arg3) >> 8;
        p = arg0;
    }
    if (p->unkC == 2) {
        end = (func_800322C0(p->unk4, 0x4B, 0, 2) - 1) << 8;
    } else {
        end = 0;
    }
    pos = p->unk8;
    while (1) {
        if (p->unkC == 2) {
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
        a = (func_800322C0(p->unk4, 0x4B, 0, 2) - 1) << 8;
        if (p->unkC == 2) {
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
