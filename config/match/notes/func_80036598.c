#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
extern char D_8005C500[];

s32 strcmp(char *, char *);

s32 func_80036598(s32 arg0) {
    return strcmp(D_8005C500, (char *)(arg0 * 0x2A4 + (int)D_8005F264) + 0x700) == 0;
}
