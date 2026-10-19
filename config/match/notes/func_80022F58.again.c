#include "common.h"
#include "sdk.h"
#include "game.h"

s32 func_800267C4(void *out, s32 argAddr);
extern s8 g_CameraMode;
extern s32 g_CurrentZone[];

void func_80022F58(s32 *arg0) {
    s32 sp20[24];
    s32 sp80[4];

    do {
        sp80[0] = g_CurrentZone[0];
        sp80[1] = g_CurrentZone[2];
        sp80[2] = g_CurrentZone[5];
        sp80[3] = g_CurrentZone[11];
        if (func_800267C4(sp20, (s32)sp80) == 1) {
            Cam_MoveAlongPath(sp20[0], sp20[1], sp20[2], 0);
            if (Ent_GetPropertyValue(sp20[1], 0x29, 0, 0, 0, 2, 0) != g_CurrentZone[9]) {
                g_CameraMode = 5;
            }
        } else {
            Cam_MoveAlongPath(g_CurrentZone[0], g_CurrentZone[2], (Ent_GetPropertyRowLength(g_CurrentZone[2], 0x4B, 0, 2) - 1) << 8, 0);
            g_CameraMode = 5;
        }
        func_800231B0(arg0);
    } while (g_CameraMode != 5);
}
