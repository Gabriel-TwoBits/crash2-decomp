#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 g_PermFlags[];
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CC00;
extern s32 D_8006CC84;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CBC0;
extern s32 D_8006CBEC[];
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;
extern s32 D_8006CB98;
extern s32 D_8006CBCC;
extern s32 D_8006CBD0;
extern s32 D_8006CBD4;
extern s32 D_8006CBD8;
extern s32 D_8006CBDC;
extern s32 D_8006CBE0;
extern s32 D_8006CC28;
extern s32 D_8006CC2C;
extern s32 D_8006CC30;
extern s32 D_8006CC34;
extern s32 D_8006CC50;
extern s32 D_8006CC54;
extern s32 D_8006CC6C;
extern s32 D_8006CC70;
extern s32 D_8006CC88;
extern s32 D_8006CC8C;
extern s32 D_8006CCD4;
extern s32 D_8006CCD8;
extern s32 D_8006CD7C;
extern s32 D_8006CDA4;
extern s32 D_8006CDA8;
extern s32 D_8006CDC8;

void Sys_ResetGameInfo(s32 arg0) {
    if (arg0 != 0) {
        D_8006CD7C = 0;
        D_8006CD20 = 0;
        D_8006CB84 = 0;
        D_8006CC00 = 0;
        D_8006CD40 = 0;
        D_8006CBD4 = 0;
        D_8006CBD8 = 0;
        D_8006CBDC = 0;
        D_8006CBE0 = 0;
        D_8006CC28 = 0;
        D_8006CC2C = 0;
        D_8006CDA4 = 0;
        D_8006CDA8 = 0;
        D_8006CDC8 = 0;
        D_8006CC50 = 0;
        D_8006CC54 = 0;
        D_8006CC30 = 0;
        D_8006CC34 = 0;
        D_8006CC84 = -1;
        D_8006CBC0 = 0x63;
        D_8006CCD4 = 0x63;
        D_8006CCD8 = 0;
        D_8006CB98 = 0;
        D_8006CC6C = 0;
        D_8006CC70 = 0;
        D_8006CC88 = 0;
        D_8006CC8C = 0;
        D_8006CDB4 = -1;
        D_8006CDB8 = 0;
        D_8006CBD0 = D_8006CBEC[0];
        memset(D_80069718, 0, 0x200);
        memset(g_PermFlags, 0, 0x200);
        D_8006CBCC = 0;
    }
}
