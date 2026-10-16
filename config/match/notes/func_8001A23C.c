#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u32 key;
    s32 ptr;
    s32 type;
} Ent;

s32 D_8005F3B8;
extern Ent D_8006302C[];
extern u32 D_8006B1C4[];
extern s32 D_8006CDF0;
extern s32 D_8006CDF4;
extern s32 D_8006CDF8;
s32 func_80019098(void *, u8);
s32 func_80019FA0(u32);

void func_8001A23C(void *arg0, s32 arg1) {
    s32 i;
    s32 j;
    Ent *e;
    u32 id;
    u32 idx;
    u32 key;
    s32 *p;
    u8 *tbl;

    D_8006CDF0 = 0;
    D_8006CDF8 = 0;
    D_8006CDF4 = 0x19000;
    i = D_8005F3B8 - 1;
    for (; i >= 0; i--) {
        tbl = (u8 *)D_8006B1C4;
        e = &D_8006302C[i];
        if (arg1 == -1 || arg1 == e->type) {
            if (e->type != 0x13B) {
                if (e->type == 0x13C) {
                    key = e->key;
                    id = key & 0xFFFF00;
                    if ((*(u32 *)(tbl + (id >> 6)) & 1)
                        || (idx = key & 0xFF) >= 8
                        || ((e->ptr = func_80019098(func_80014260(*(u8 **)((u8 *)arg0 + 0x10) + (idx * 4 + 0x194), 1, 0), e->key >> 24)) < -0xFE)) {
                        if (e->ptr == 0) {
                            e->ptr = func_80019FA0(id);
                        }
                        e->type = 0;
                    } else {
                        goto remove;
                    }
                }
            } else {
                id = e->key & 0xFFFF00;
                if ((*(u32 *)(tbl + (id >> 6)) & 1) && (e->ptr != 0 || (e->ptr = func_80019FA0(id)) != 0)) {
                    p = (s32 *)e->ptr;
                    if (!(p[0xAC / 4] & 0x01000000) && !(p[0x100 / 4] & 0x40000)) {
                        func_80019BBC(p, 1);
                    }
                }
remove:
                for (j = i + 1; j < D_8005F3B8; j++) {
                    D_8006302C[j - 1] = D_8006302C[j];
                }
                D_8005F3B8--;
            }
        }
    }
}
