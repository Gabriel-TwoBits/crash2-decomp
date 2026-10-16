#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Unk8001E8E4Mtx;

typedef struct {
    s32 x, y, z;
} Unk8001E8E4Vec;

void func_80018194(Unk8001E8E4Vec *, Unk8001E8E4Mtx *, Unk8001E8E4Vec *);
void func_8001E780(Unk8001E8E4Mtx *, s32, s32);

void func_8001E8E4(Unk8001E8E4Vec *arg0, Unk8001E8E4Vec *arg1, s32 arg2, s32 arg3, Unk8001E8E4Vec *arg4) {
    Unk8001E8E4Mtx mtx;
    Unk8001E8E4Vec out;
    Unk8001E8E4Vec in;

    func_8001E780(&mtx, arg2, arg3);
    in.x = arg0->x / 16;
    in.y = arg0->y / 16;
    in.z = arg0->z / 16;
    func_80018194(&in, &mtx, &out);
    out.x *= 16;
    out.y *= 16;
    out.z *= 16;
    arg4->x = out.x + arg1->x;
    arg4->y = out.y + arg1->y;
    arg4->z = out.z + arg1->z;
}
