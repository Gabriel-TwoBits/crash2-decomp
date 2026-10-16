#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
extern s32 D_80069718[];
extern char D_8005C500[];
extern char *D_8006CBCC;

s32 func_80036C40(s32 arg0) {
    s32 *src;
    s32 *row;
    s32 *dst;
    s32 i;
    s32 j;
    char *base;

    if (D_8005F264 == 0) {
        return 1;
    }
    base = (char *)D_8005F264 + (arg0 * 0x2A4 + 0x6F4);
    src = (s32 *)(base + 0x60);
    i = 0;
    row = D_80069718;
    src = (s32 *)(base + 0x60);
    do {
        j = 0;
        dst = row;
        do {
            *dst = *src;
            src++;
            j++;
            dst++;
        } while (j < 2);
        i++;
        row += 2;
    } while (i < 0x40);
    strcpy(D_8005C500, base + 0xC);
    D_8006CBCC = D_8005C500;
    return 0;
}
