#include "common.h"
#include "sdk.h"
#include "game.h"

extern void func_80029884(s32, s32, s32);
extern void func_8002FB84(s32);
extern s32 *D_8006759C;
extern s32 D_80068FE4;
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CBB4;
extern s32 D_8006CC00;
extern s32 D_8006CC68;
extern s32 D_8006CC74;
extern s32 D_8006CC84;
extern s32 D_8006CC94;
extern s32 D_8006CC98;
extern s32 D_8006CCA0;
extern s32 D_8006CD14;
extern s32 D_8006CD18;
extern s32 D_8006CD1C;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CD68;
extern s32 D_8006CD90;
extern s32 D_8006CD9C;
extern s32 D_8006CDA0;
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;

void func_8002F228(s32 arg0) {
    int new_var;
    s32 *new_var2;

    D_8006CC94 = 0;
    D_8006CC68 = 0;
    D_8006CC74 = 0;
    if (D_8006CD14 != 2) {
        D_8006CCA0 = 0;
    }
    D_8006CC98 = 1;
    D_8006CD68 = 0;
    D_8006CD90 = 0;
    if (arg0 != 0) {
        new_var2 = &D_8006759C[2];
        D_8006CD9C = 0;
        if (1) {
            D_8006CDA0 = 0;
            D_8006CB84 = 0;
            D_8006CC00 = 0;
        }
        D_8006CD40 = 0;
        D_8006CD20 = 0;
        D_8006CC84 = -1;
        new_var = (*new_var2) * 2;
        D_8006CDB4 = 0;
        D_8006CDB8 = (D_80069718[new_var] & 0xF) << 8;
    }
    func_80029884(0, 0, 0);
    D_80068FE4 = 0;
    func_8002FB84(1);
    D_8006CD1C = 0x20;
    if (D_8006CBB4 != 0x600) {
        D_8006CD18 = 0x120;
    }
}
