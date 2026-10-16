#include "common.h"
#include "sdk.h"

typedef struct { s16 x, y, w, h; } Rect;
s32 func_80017380(s32, s32, s32, Rect *);

s32 func_800176DC(s32 mode, s32 a1, s32 a2, u8 *i3, u8 *i4, s32 flag) {
    Rect r;
    char buf[0x110];
    u8 *tmp = (u8 *)((s32)&buf[15] & ~0xF);
    u8 *s;
    u8 *d;
    s32 n;

    if (i3 != 0) {
        func_80017380(2, a1, a2, &r);
        if (mode == 1) {
            if ((s32)i3 & 0xF) {
                flag = 0;
                s = i3; d = tmp;
                n = r.w * r.h * 2;
                if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
                LoadImage(&r, tmp);
                DrawSync(0);
            } else {
                LoadImage(&r, i3);
                if (flag == 0) DrawSync(0);
            }
        } else if (((s32)i3 & 0xF) != 0) {
            flag = 0;
            StoreImage(&r, tmp);
            DrawSync(0);
            s = tmp; d = i3;
            n = r.w * r.h * 2;
            if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
        } else {
            StoreImage(&r, i3);
            if (flag == 0) DrawSync(0);
        }
    }
    if (i4 != 0) {
        func_80017380(1, a1, a2, &r);
        if (mode == 1) {
            if ((s32)i4 & 0xF) {
                flag = 0;
                s = i4; d = tmp;
                n = r.w * r.h * 2;
                if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
                LoadImage(&r, tmp);
                DrawSync(0);
            } else {
                LoadImage(&r, i4);
                if (flag == 0) DrawSync(0);
            }
        } else if (((s32)i4 & 0xF) != 0) {
            flag = 0;
            StoreImage(&r, tmp);
            DrawSync(0);
            s = tmp; d = i4;
            n = r.w * r.h * 2;
            if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
        } else {
            StoreImage(&r, i4);
            if (flag == 0) DrawSync(0);
        }
    }
    return flag;
}
