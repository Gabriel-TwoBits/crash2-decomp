#include "common.h"
#include "sdk.h"
#include "game.h"

extern char D_8001069C[];

s32 func_80035F00(char *arg0) {
    return strncmp(D_8001069C, arg0, 12) == 0;
}
