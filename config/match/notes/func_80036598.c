#include "common.h"
#include "sdk.h"
#include "game.h"

s32 *D_8005F264 = 0;
extern char D_8005C500[];

s32 strcmp(char *, char *);

inline char *func_80036598_name(char *entry) {
    return entry + 0x700;
}

s32 func_80036598(s32 arg0) {
    return strcmp(D_8005C500, func_80036598_name((char *)D_8005F264 + arg0 * 0x2A4)) == 0;
}
