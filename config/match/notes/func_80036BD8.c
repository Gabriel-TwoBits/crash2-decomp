#include "common.h"
#include "sdk.h"
#include "game.h"

extern char D_8005C500[];
void func_80036850(void);
s32 *D_8005F264 = 0;

s32 func_80036BD8(s32 arg0) {
    s32 ret;

    if (D_8005F264 != 0) {
        strcpy(arg0 * 0x2A4 + (char *)D_8005F264 + 0x700, D_8005C500);
        func_80036850();
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}
