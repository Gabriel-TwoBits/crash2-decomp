#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 x, y, z;
} Vec3i80028750;

extern char *g_CurrentZone;
typedef struct {
    char pad[0x190];
    s32 count;
    s32 ent[8];
    s32 flags[8];
} Unk80027DEC;

s32 func_80027F3C(void *, u16, s32 *, void *, void *, s32);

s32 func_80028750(Vec3i80028750 *arg0, Vec3i80028750 *arg1) {
    Unk80027DEC *p = *(Unk80027DEC **)(g_CurrentZone + 0x10);
    Vec3i80028750 pos;
    s32 box[6];
    s32 r;
    s32 i;
    s32 *e;
    s32 *q;

    if (arg1->x == 0 && arg1->y == 0 && arg1->z == 0) {
        return 0;
    }
    do {
        pos = *arg0;
        for (i = 0; i < p->count; i++) {
            if (!(p->flags[i] & 4)) {
                continue;
            }
            e = Nsf_OpenEntry(&p->ent[i], 1, 0);
            if (func_80030D74(e, arg0) == 0) {
                continue;
            }
            q = *(s32 **)((char *)e + 0x14);
            box[0] = q[0] << 8;
            box[1] = q[1] << 8;
            box[2] = q[2] << 8;
            box[3] = q[3] << 8;
            box[4] = q[4] << 8;
            box[5] = q[5] << 8;
            r = func_80027F3C(q, *(u16 *)((char *)q + 0x1C), box, arg0, arg1, 0);
            break;
        }
        if (i == p->count) {
            return 1;
        }
    } while (r == 0);
    return r;
}
