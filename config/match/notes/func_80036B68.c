#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F264;
extern char D_8005C500[];
extern char *D_8006CBCC;

s32 func_80036B68(s32 arg0) {
    s32 ret;
    char *src;
    if (D_8005F264 != 0) {
        src = (char *)(arg0 * 0x2A4 + D_8005F264 + 0x700);
        strcpy(D_8005C500, src);
        D_8006CBCC = D_8005C500;
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}
