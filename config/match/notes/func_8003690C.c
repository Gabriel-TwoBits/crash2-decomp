#include "common.h"
#include "sdk.h"
#include "game.h"

struct Unk8005F288 {
    s32 a;
    s16 b;
};
struct Unk8005F288 D_8005F288 = {0x59544D45, 0x5000};
void *memset(void *s, s32 c, s32 n);

void func_8003690C(void *p) {
    s32 *arg0 = p;
    memset(arg0, 0, 0x2A4);
    arg0[0] = 8;
    arg0[1] = 1;
    arg0[3] = D_8005F288.a;
    *(s16 *)&arg0[4] = D_8005F288.b;
    arg0[8] = 0x63;
    arg0[11] = 5;
    arg0[22] = 0xFF;
    arg0[23] = 0xFF;
}
