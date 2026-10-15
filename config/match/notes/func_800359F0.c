#include "common.h"
#include "sdk.h"
#include "game.h"

extern volatile u32 D_8003BEA4;
extern char D_8005F274[];
s32 format(char *);

void func_800359F0(void) {
    s32 r;
    D_8003BEA4;
    r = format(D_8005F274);
    D_8003BEA4;
    if (r == 1) {
        func_800360AC(2);
        func_80036084(4);
        func_800360CC(0);
    } else if (func_80035A94() != 0) {
        func_80036084(2);
        func_800360CC(0);
    }
}
