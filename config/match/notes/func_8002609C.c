#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk8005BA58 {
    u8 pad[0x10];
} Unk8005BA58;

void func_800260F4(Unk8005BA58 *);
extern s32 D_8005B9E0;
extern Unk8005BA58 D_8005BA58[];

void func_8002609C(void) {
    Unk8005BA58 *p;
    s32 n;

    n = D_8005B9E0;
    p = D_8005BA58;
    while (n > 0) {
        func_800260F4(p);
        n--;
        p++;
    }
    D_8005B9E0 = 0;
}
