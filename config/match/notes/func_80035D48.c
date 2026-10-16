#include "common.h"
#include "sdk.h"
#include "game.h"

s32 strcmp(char *, char *);

typedef struct { char b[0x28]; } Unk80035D48;

void func_80035D48(Unk80035D48 *arg0, s32 arg1) {
    Unk80035D48 tmp;
    s32 i;
    s32 j;
    s32 first;
    s32 r;
    char *a;

    for (i = 0; i < arg1; i++) {
        a = arg0[i].b;
        first = func_80035F00(a);
        for (j = i + 1; j < arg1; j++) {
            r = func_80035F00(arg0[j].b);
            if (first != 0 && (r == 0 || strcmp(a, arg0[j].b) > 0)) {
                func_8003B940(&arg0[i], &tmp, 0x14);
                func_8003B940(&arg0[j], &arg0[i], 0x14);
                func_8003B940(&tmp, &arg0[j], 0x14);
                i--;
                break;
            }
        }
    }
}
