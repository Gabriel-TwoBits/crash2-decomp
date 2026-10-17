#include "common.h"
#include "sdk.h"
#include "game.h"

#define SLOT_SIZE 0x2C
#define ERR_NO_SLOT (-0xC)
#define KEEP_FIELD 0x6396347F
#define FLAG_LAST 2
#define FLAG_MID 0xC

s32 func_800136C8(s32 *, s32);
extern char D_80067588[];

void *func_80013C98(void *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 arg4) {
    s32 idx;
    s32 count;
    s32 i;
    s32 base;
    char *first;
    char *dst;
    char *src;

    idx = func_800136C8(arg3, arg4);
    if (idx == -1) {
        return (void *)ERR_NO_SLOT;
    }
    count = *arg3;
    dst = D_80067588 + (idx + count) * SLOT_SIZE;
    if (arg0 == 0) {
        return dst;
    }
    first = dst;
    i = count - 1;
    if (i >= 0) {
        base = *(s32 *)((char *)arg0 + 0x14) >> 1;
        do {
            src = (char *)D_80067FC4[base + i];
            *(s32 *)(dst + 0x14) = *(s32 *)(src + 0x14);
            if (arg2 != KEEP_FIELD) {
                *(s32 *)(dst + 0x18) = arg2;
            } else {
                *(s32 *)(dst + 0x18) = *(s32 *)(src + 0x18);
            }
            *(s16 *)(src + 4) = 1;
            *(u8 *)(dst + 0xF) = *(u8 *)(src + 0xF);
            *(u8 *)(src + 0xF) = 0;
            *(u8 *)(dst + 0xE) = *(u8 *)(src + 0xE);
            *(s32 *)(dst + 0x28) = *(s32 *)(src + 0x28);
            *(char **)(dst + 0x24) = first;
            *(s32 *)(dst + 0x1C) = *arg3;
            if (i == *arg3 - 1) {
                *(s16 *)(dst + 4) = FLAG_LAST;
            } else {
                *(s16 *)(dst + 4) = FLAG_MID;
            }
            D_80067FC4[*(s32 *)(dst + 0x14) >> 1] = (s32)dst;
            dst -= SLOT_SIZE;
            i--;
        } while (i >= 0);
    }
    return first;
}
