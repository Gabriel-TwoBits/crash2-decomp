#include "common.h"
#include "sdk.h"
#include "game.h"

void Input_StartPlayback(void);
extern s32 g_DemoBlock;
extern s32 D_800675AC;
extern s32 D_8006CB20;

void Input_LoadDemo(s32 *arg0) {
    int new_var2;
    s32 buf[2];
    s32 *new_var;
    s32 *temp_v0;

    D_800675AC = 0;
    temp_v0 = Nsf_OpenEntry(arg0, 1, 1);
    do { new_var2 = 0x6396347F; new_var = arg0; } while (0);
    *new_var = new_var2;
    g_DemoBlock = temp_v0[4];
    buf[1] = 0x6400;
    buf[0] = 0;
    Obj_Spawn(&D_8006CB20, 4, 8, 2, (s32)buf, 1);
    Input_StartPlayback();
}
