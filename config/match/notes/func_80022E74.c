#include "common.h"
#include "sdk.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

extern char *func_80014260(s8 *, s32, s32);
extern void func_800200A8(void *, s32, s32, s32);
extern void func_800231B0(Vec3 *);
extern s8 D_8005B994;
extern s8 D_8005B99B[];
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern char *D_8005F38C;
extern void *D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608E0;

void func_80022E74(Vec3 *arg0) {
    s8 *f;
    s32 n;
    char *p;
    Vec3 v;
    s32 off;

    v.x = arg0->x;
    v.y = arg0->y;
    v.z = arg0->z;
    f = D_8005B99B;
    if (*f != 0) {
        f += 0x13D;
        p = func_80014260(f, 1, 0);
        off = 0x184;
        do { n = D_8005BADC; } while (0);
        func_800200A8(p, *(s32 *)(p + (2 * (2 * (n * 3 + *(s32 *)(*(char **)(p + 0x10) + off)))) + 0x10), D_8005BAE0, 0);
        D_8005B994 = 5;
        *(void **)(D_8005F38C + 0x18) = D_800608CC;
        return;
    }
    func_800200A8(D_800608CC, D_800608D4, D_800608E0, 0);
    func_800231B0(&v);
}
