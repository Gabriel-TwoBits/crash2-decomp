#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 DrawSync(s32);
extern s32 LoadImage(s16 *, void *);

void func_80026EF4(s32 *arg0) {
    s16 rect[4];
    s32 y, w, h, rows, n, area;
    u16 *lut;
    u32 *src;
    u32 *dst;
    u32 p;

    func_80013A54(8, 1);
    func_80013A54(9, 1);
    func_80013A54(0xC, 1);
    func_80013A54(0xD, 1);
    w = arg0[6];
    h = arg0[7];
    area = w * h;
    rect[0] = 0;
    rect[1] = 0x100;
    rect[2] = w;
    rect[3] = h;
    rows = arg0[1];
    lut = (u16 *)arg0[0xE];
    for (y = 0; y < rows; y += 0x100) {
        func_800270C4(arg0, y);
        src = (u32 *)arg0[0xD];
        dst = (u32 *)arg0[0x12];
        for (n = area >> 2; n > 0; n--) {
            p = *src++;
            *dst = (lut[(p >> 8) & 0xFF] << 16) | lut[p & 0xFF];
            dst++;
            *dst = (lut[p >> 24] << 16) | lut[(p >> 16) & 0xFF];
            do { } while (0);
            dst++;
        }
        LoadImage(rect, (void *)arg0[0x12]);
        DrawSync(0);
        rect[1] += h;
    }
}
