#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_8001254C(u8 *base) {
    s32 count;
    s32 i;
    s32 j;
    u32 *table;
    u32 *p;
    u32 *q;
    u8 *entry;
    u32 *data;

    table = (u32 *)(base + 0x10);
    count = *(s32 *)(base + 8);
    if (*(u16 *)(base + 2) == 1) {
        return -0xFF;
    }
    if (*(u16 *)base != 0x1234) {
        return -0x12;
    }
    i = count - 1;
    if (i >= 0) {
        p = table - (-i);
        do {
            entry = base + *p;
            *p = (u32)entry;
            j = *(s32 *)(entry + 0xC);
            data = (u32 *)(entry + 0x10);
            if (j >= 0) {
                q = data - (-j);
                do {
                    j--;
                    *q = (u32)entry + *q;
                    q--;
                } while (j >= 0);
            }
            i--;
            p--;
        } while (i >= 0);
    }
    table[count] = (u32)base + table[count];
    return -0xFF;
}
