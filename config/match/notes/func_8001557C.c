#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    char pad0[6];
    s16 a;
    char pad1[0x10];
    s32 b;
    char pad2[0x10];
} Unk8001557C;

extern Unk8001557C D_80062BB4[];

Unk8001557C *func_8001557C(s32 arg0) {
    s32 i;

    for (i = 0; i < 10; i++) {
        if (D_80062BB4[i].a == 5 && D_80062BB4[i].b == arg0) {
            return &D_80062BB4[i];
        }
    }
    return 0;
}
