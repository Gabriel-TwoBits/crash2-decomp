#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F1A4 = 1;
extern int D_80067FB4[];
extern int D_80069034[];

void func_80012E4C(s32 arg0) {
    char sp10[8];
    int v;

    if (D_8005F1A4 != 0 && D_80067FB4[0] == 0 && CdSync(1, 0) == 0) {
        v = D_80069034[v = arg0 * 3];
        v = 0x7FFFFF & v;
        CdIntToPos(v, sp10);
        CdControlF(0x15, sp10);
    }
}
