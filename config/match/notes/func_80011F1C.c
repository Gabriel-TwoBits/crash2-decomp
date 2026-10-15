#include "common.h"
#include "sdk.h"
#include "game.h"

extern char D_8005F198[];
extern char D_8005F1A0[];
extern char D_8005F404[];

char *func_80011F1C(s32 arg0) {
    if (arg0 & 1) {
        sprintf(D_8005F404, D_8005F1A0, arg0 >> 1);
    } else {
        sprintf(D_8005F404, D_8005F198, arg0);
    }
    return D_8005F404;
}
