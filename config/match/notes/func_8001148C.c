#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800111B0(s32);
extern s32 *D_8005F2A4;
extern s32 *D_8005F2B4;
extern s32 D_8005F2BC;
extern s32 *D_8006074C;
extern s32 *D_80062BAC;

s32 func_8001148C(s32 arg0) {
    s32 size;
    s32 tries;
    int new_var;
    s32 head;
    s32 next;
    s32 *p;
    s32 *q;

    tries = 2;
    size = arg0 + 3;
    size &= ~3;
    if (D_8005F2BC == 0) {
        if (!((u32)D_8005F2A4 < (u32)D_8005F2B4)) {
            return 0;
        }
        *D_8005F2A4 = -2;
        D_8006074C = D_8005F2A4;
        D_8005F2A4 = D_8005F2A4 + 1;
        if (func_800111B0(size) != 0) {
            return 0;
        }
        D_8005F2BC = 1;
        D_80062BAC = D_8006074C;
    }
    D_8006074C = D_80062BAC;
    for (;;) {
        p = D_8006074C;
        head = *p;
        if (head & 1) {
            if ((u32)head == (u32)(size | 1)) {
                break;
            }
            p = D_8006074C;
            next = *p;
            if ((u32)(size | 1) < (u32)next) {
                p[(size / 4) + 1] = next - size - 4;
                *p = size;
                return (s32)(D_8006074C + 1);
            }
            if ((u32)next < (u32)size) {
                q = &p[(u32)next >> 2];
                new_var = 1;
                head = q[new_var];
                if (head == -2) {
                    *p = head;
                    D_8005F2A4 = D_8006074C + 1;
                } else if (head & 1) {
                    *p = next + 4 + (head & ~1);
                } else {
                    D_8006074C = q + 1;
                }
            }
        } else {
            head = *D_8006074C;
            if (head != -2) {
                D_8006074C = &D_8006074C[(u32)head >> 2] + 1;
            } else {
                tries--;
                if (tries <= 0) {
                    if (func_800111B0(size) != 0) {
                        return 0;
                    }
                } else {
                    D_8006074C = D_80062BAC;
                }
            }
        }
    }
    *p = head & ~1;
    return (s32)(D_8006074C + 1);
}
