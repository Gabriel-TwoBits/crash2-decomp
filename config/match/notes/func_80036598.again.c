#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *g_SaveState = 0;
extern char D_8005C500[];

s32 func_80036598(s32 arg0) {
    int o = arg0 * 0x2A4;
    return strcmp(D_8005C500, (char *)(o + (int)g_SaveState) + 0x700) == 0;
}
