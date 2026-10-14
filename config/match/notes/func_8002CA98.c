#include "common.h"
#include "sdk.h"

extern char D_8006CB68[];
extern s32 D_8006CB6C;

void func_8002CA98(void) {
    *(char **)D_8006CB68 = D_8006CB68 + 0x414;
    D_8006CB6C = 0;
}
