#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { char b[0x28]; } Unk80035D48;
extern char D_8005F274[];
s32 *D_8005F264 = 0;
s32 func_80035CBC(char *, void *);
s32 _card_load(s32);

void func_80034898(void) {
    s32 r;
    s32 a0;

    if (D_8005F264[6] != 0) {
        D_8005F264[6] = 0;
        func_80035B70();
        _card_load(0);
    }
    switch (func_80035AB8()) {
    case 0:
        r = func_80035CBC(D_8005F274, (char *)D_8005F264 + 0x49C);
        func_80035D48((Unk80035D48 *)((char *)D_8005F264 + 0x49C), r);
        if (r <= 0) {
            D_8005F264[7]++;
            if (D_8005F264[7] < 11) {
                func_800360CC(1);
                return;
            }
        }
        func_800360AC(2);
        func_80036050();
        func_80036850();
        func_800360CC(5);
        func_80035A7C(0x20);
        D_8005F264[2] = 0;
        D_8005F264[8] = r;
        D_8005F264[9] = 0;
        break;
    case 2:
        if (func_80035A94() != 0) {
            goto block_15;
        }
        a0 = 1;
        goto block_16;
    case 1:
        if (func_80035A94() != 0) {
    case 3:
block_15:
            func_80036028(2);
            a0 = 0;
        } else {
            a0 = 1;
        }
block_16:
        func_800360CC(a0);
        break;
    default:
        break;
    }
}
