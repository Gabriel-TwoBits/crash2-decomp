#include "common.h"
#include "sdk.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

typedef struct {
    char pad0[0x184];
    s32 idx;
} Inner;

typedef struct {
    char pad0[0x10];
    s32 items[1];
} Outer;

extern Outer *Nsf_OpenEntry(s8 *, s32, s32);
extern void Cam_MoveAlongPath(void *, s32, s32, s32);
extern void func_800231B0(Vec3 *);
extern s8 g_CameraMode;
extern s8 D_8005B99B[];
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern char *g_Player;
extern void *g_CurrentZone;
extern s32 g_CameraPath;
extern s32 g_CameraProgress;

void func_80022E74(Vec3 *arg0) {
    s8 *f;
    Outer *p;
    Vec3 v;

    v.x = arg0->x;
    v.y = arg0->y;
    v.z = arg0->z;
    f = D_8005B99B;
    if (*f != 0) {
        p = Nsf_OpenEntry(f + 0x13D, 1, 0);
        Cam_MoveAlongPath(p, p->items[D_8005BADC * 3 + ((Inner *)p->items[0])->idx], (&D_8005BADC)[1], 0);
        g_CameraMode = 5;
        *(void **)(g_Player + 0x18) = g_CurrentZone;
        return;
    }
    Cam_MoveAlongPath(g_CurrentZone, g_CameraPath, g_CameraProgress, 0);
    func_800231B0(&v);
}
