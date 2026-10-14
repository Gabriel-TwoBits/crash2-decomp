#include "common.h"
#include "sdk.h"

void func_80013F28(void);
extern s32 D_80067FB0[];

void func_80013E10(void *arg0) {
    s8 temp_v0;
    s8 temp_v0_2;

    temp_v0 = *(s8 *)((char *)arg0 + 0xF);
    if (temp_v0 > 0) {
        temp_v0_2 = temp_v0 - 1;
        *(s8 *)((char *)arg0 + 0xF) = temp_v0_2;
        if (((temp_v0_2 << 0x18) <= 0) && (*(s16 *)((char *)arg0 + 6) == 0)) {
            func_80013F28();
            D_80067FB0[0] -= 1;
        }
    }
}
