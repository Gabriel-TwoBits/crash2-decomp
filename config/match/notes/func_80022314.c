typedef signed int s32;
extern void func_800200A8(s32, s32, s32, s32);
extern void func_800224A0(s32 *);
extern void func_800231B0(s32 *);
extern void func_80023E60(s32 *, s32, s32 *, s32);
extern s32 func_8002433C(s32 *, s32 *, s32 *, s32 *, s32 *);
extern s32 func_80024658(s32 *, s32 *, s32, s32, s32, s32);
extern s32 D_800608CC[];
extern s32 *D_8006759C;

void func_80022314(s32 *arg0, s32 arg1) {
    s32 a[3];
    s32 b[3];
    s32 pad[3];
    s32 c[3];
    s32 sp58, sp5C, sp60;
    s32 r;
    s32 k;
    s32 *p;

    a[0] = arg0[0];
    a[1] = arg0[1];
    a[2] = arg0[2];
    if (D_8006759C[2] == 3) {
        a[0] = (a[0] * 5) >> 3;
    }
    p = D_800608CC;
    b[0] = p[0];
    b[1] = p[2];
    b[2] = p[5];
    r = func_8002433C(b, a, &sp58, &sp5C, &sp60);
    if (r > 0) {
        r = func_80024658(b, a, sp5C, 0, sp60, 1);
        k = -1;
        if (arg1 != 0) {
            k = 0x100;
        }
        func_80023E60(b, r, c, k);
        func_800200A8(c[0], c[1], c[2], 0);
    } else if (r < 0) {
        r = func_80024658(b, a, sp58, 1, sp60, 1);
        k = -1;
        if (arg1 != 0) {
            k = 0x100;
        }
        func_80023E60(b, r, c, k);
        func_800200A8(c[0], c[1], c[2], 0);
    } else {
        func_800200A8(p[0], p[2], p[5], 0);
    }
    func_800224A0(a);
    func_800231B0(a);
}
