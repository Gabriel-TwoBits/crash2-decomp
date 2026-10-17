#include "common.h"
#include "sdk.h"
#include "game.h"

#define TEXT_OFFSET 0xC
#define STRING_INDEX_SHIFT 8
#define ASCII_SPACE 0x20

void func_800175D8(u8 *arg0, s32 arg1, u8 *arg2, s32 arg3, s32 arg4) {
    s32 skip;
    s32 i;
    s32 pos;
    u8 c;

    pos = 0;
    skip = arg1 >> STRING_INDEX_SHIFT;
    for (i = 0; i < skip; i++) {
        /* skip one NUL-terminated string */
        do {
            c = arg0[pos + TEXT_OFFSET];
            pos++;
        } while (c != 0);
    }

    i = 0;
    if (arg3 == 0) {
        return;
    }
    do {
        c = *arg2;
        if (c == 0) {
            break;
        }
        if (c < ASCII_SPACE) {
            c = ASCII_SPACE;
        }
        if (c == 0x7B) {
            c = 0x28;
        }
        if (c == 0x7D) {
            c = 0x29;
        }
        if (c == 0x7E) {
            c = 0x7B;
        }
        if (c == 0x25) {
            c = 0x7D;
        }
        if (c == 0x26) {
            c = 0x5F;
        }
        arg0[pos + i + TEXT_OFFSET] = c;
        arg2++;
        arg3--;
        i++;
    } while (arg3 != 0);

    while (arg3 != 0) {
        arg0[pos + i + TEXT_OFFSET] = ASCII_SPACE;
        arg3--;
        i++;
    }
}
