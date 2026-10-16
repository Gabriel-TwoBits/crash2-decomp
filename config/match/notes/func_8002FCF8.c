#include "common.h"
#include "sdk.h"
#include "game.h"
#include "m2c_macros.h"

M2C_UNK func_8002F6C8(void *, s32, s32, s32);       /* extern */
M2C_UNK func_8003156C(s32);                         /* extern */
extern void *D_8005F374;
extern void *D_8005F38C;
extern void *D_8005F3D8;
extern s32 D_80060944;
extern void *D_80063454;
extern s32 D_8006CBF0;
extern s32 D_8006CC00;
extern s32 D_8006CC84;
extern s32 D_8006CD14;

void func_8002FCF8(void) {
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a3;
    void *temp_v0;

    D_8006CD14 = 2;
    D_8005F374 = D_8005F3D8 + 0x1090;
    func_8003156C(M2C_FIELD(D_8005F3D8, s32 *, 4));
    D_80060944 = M2C_FIELD(D_8005F3D8, s32 *, 0xC);
    M2C_FIELD(D_80063454, s32 *, 0xC) = (s32) M2C_FIELD(D_8005F3D8, s32 *, 0x1070);
    temp_v0 = M2C_FIELD(D_8005F38C, void **, 8);
    temp_a2 = M2C_FIELD(D_8005F3D8, s32 *, 0x107C);
    temp_a3 = M2C_FIELD(D_8005F3D8, s32 *, 0x1080);
    M2C_FIELD(temp_v0, s32 *, 0x30) = (s32) M2C_FIELD(D_8005F3D8, s32 *, 0x1074);
    M2C_FIELD(temp_v0, s32 *, 0x34) = (s32) M2C_FIELD(D_8005F3D8, s32 *, 0x1078);
    M2C_FIELD(temp_v0, s32 *, 0x38) = temp_a2;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = temp_a3;
    temp_a1 = M2C_FIELD(D_8005F3D8, s32 *, 0x1088);
    M2C_FIELD(temp_v0, s32 *, 0x40) = (s32) M2C_FIELD(D_8005F3D8, s32 *, 0x1084);
    M2C_FIELD(temp_v0, s32 *, 0x44) = temp_a1;
    D_8006CC84 = -1;
    func_8002F6C8(D_8005F3D8 + 0x10, temp_a1, temp_a2, temp_a3);
    D_8006CC00 = 0;
    D_8006CBF0 = 0;
}
