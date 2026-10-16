#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_80027CD0(u8 *arg0, u8 *arg1, s32 arg2) {
    u8 *ring = (u8 *)arg2;
    u8 *src = arg0;
    u8 *start = arg1;
    u8 *data;
    s32 pos = 1;
    s32 mask;
    s32 flags;
    s32 off;
    s32 len;
    u32 size;
    u8 b;

    size = *src++ << 24;
    size |= *src++ << 16;
    size |= *src++ << 8;
    size |= *src++;
    data = src + size;
    while (1) {
        flags = *src++;
        for (mask = 0x80; mask & 0xFF; mask >>= 1) {
            if (flags & mask) {
                b = *data++;
                *arg1++ = b;
                ring[pos] = b;
                pos = (pos + 1) & 0xFFF;
            } else {
                len = (*data >> 4) + 2;
                off = data[1] | ((*data & 0xF) << 8);
                data += 2;
                if (off == 0) {
                    return arg1 - start;
                }
                while (len-- & 0xFFFF) {
                    b = ring[off];
                    off = (off + 1) & 0xFFF;
                    ring[pos] = b;
                    pos = (pos + 1) & 0xFFF;
                    *arg1++ = b;
                }
            }
        }
    }
}
