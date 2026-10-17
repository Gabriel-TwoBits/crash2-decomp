#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F1FC[] = "DirnD";

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80020A24);

#ifdef NON_MATCHING
#define RES_TYPE_A 0x208
#define RES_TYPE_B 0x209
#define SIDE_SELECT 2
#define OFFSET_DELTA 0x100
#define FLAGS_FIRST 0x14
#define FLAGS_SECOND 0xC

void func_800217F8(s32 arg0, s32 arg1, s32 arg2) {
    s32 count;
    s32 type;
    s32 offset;
    s32 p;

    if (arg2 == SIDE_SELECT) {
        type = RES_TYPE_B;
        offset = arg1 - OFFSET_DELTA;
    } else {
        type = RES_TYPE_A;
        offset = arg1 + OFFSET_DELTA;
    }
    p = func_80031AE8(arg0, type, offset, 0, &count, 1, 0, 0, 0);
    if ((p != 0) && (count != 0)) {
        func_80021900(count, (u32 *)p, FLAGS_FIRST, 1);
    }

    if (arg2 == SIDE_SELECT) {
        type = RES_TYPE_A;
    } else {
        type = RES_TYPE_B;
    }
    p = func_80031AE8(arg0, type, arg1, 0, &count, 1, 0, 0, 0);
    if ((p != 0) && (count != 0)) {
        func_80021900(count, (u32 *)p, FLAGS_SECOND, 1);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800217F8);
#endif

extern s32 D_80067590;

void func_80021900(s32 count, u32 *p, s32 flags, s32 arg3) {
    u32 *cur;
    u32 v;
    u32 *e;
    s32 neg;

    cur = 0;
    do {
        v = *p;
        if (v & 1) {
            e = *(u32 **)(((v >> 13) & 0x3FC) + D_80067590);
            if (cur == 0 || cur < e) {
                cur = e;
            }
            if (cur[1] != v) {
                while (cur[1] != v) {
                    cur += 2;
                }
                cur -= 2;
                cur += 2;
            }
        } else {
            cur = (u32 *)v;
        }
        if (flags & 8) {
            func_80014260(cur, flags | 2, arg3);
        } else {
            neg = arg3;
            if (flags & 4) {
                neg = -neg;
            }
            func_8001434C((u32)cur, neg);
        }
        count--;
        p++;
    } while (count != 0);
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021A08);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80021F4C);

extern s8 D_8005B99B;
extern s32 D_8005B9C4;
extern s32 D_800608F8;

void func_800221DC(void) {
    D_800608F8 = 0;
    if (D_8005B99B != 0) {
        D_8005B9C4 = 0;
        return;
    }
    func_80023154();
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022224);

#ifdef NON_MATCHING
extern s32 D_800608CC[];
extern s32 *D_8006759C;

void func_80022314(s32 *arg0, s32 arg1) {
    s32 a[3];
    s32 b[8];
    s32 out[4];
    s32 sp58;
    s32 sp5C;
    s32 sp60;
    s32 r;
    s32 a3;
    s32 x, y, z;

    a[0] = arg0[0];
    a[1] = arg0[1];
    a[2] = arg0[2];
    if (D_8006759C[2] == 3) {
        a[0] = (a[0] * 5) >> 3;
    }
    b[0] = D_800608CC[0];
    b[1] = D_800608CC[2];
    b[2] = D_800608CC[5];
    r = func_8002433C(b, a, &sp58, &sp5C, &sp60);
    if (r > 0) {
        r = func_80024658(b, a, sp5C, 0, sp60, 1);
        func_80023E60(b, r, out, arg1 ? 0x100 : -1);
        x = out[0];
        y = out[1];
        z = out[2];
    } else if (r < 0) {
        s32 r2 = func_80024658(b, a, sp58, 1, sp60, 1);
        s32 a3b = -1;
        if (arg1 != 0) {
            a3b = 0x100;
        }
        func_80023E60(b, r2, out, a3b);
        x = out[0];
        y = out[1];
        z = out[2];
    } else {
        x = D_800608CC[0];
        y = D_800608CC[2];
        z = D_800608CC[5];
    }
    func_800200A8(x, y, z, 0);
    func_800224A0(a);
    func_800231B0(a);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022314);
#endif

extern s32 D_800607FC;
extern s32 D_800608D8[];
extern s32 D_800608E0;
extern s32 D_8006CD98;

void func_800224A0(s32 *arg0) {
    s32 sp28;
    s16 *p;
    s32 new_var;
    s32 lo;
    s32 hi;
    s32 scale;
    s32 d;

    if (func_80031AE8(D_800608D8[0], 0x252, D_800608E0, 0, 0, 0, 0, 0, 0) != 0) {
        p = (s16 *)func_80031AE8(D_800608D8[0], 0x254, D_800608E0, 0, 0, 3, &sp28, 0, 0);
        if (p != 0) {
            lo = p[0] << 8;
            hi = p[1];
            hi = hi << 8;
            new_var = hi;
            scale = p[2];
            if (D_8006CD98 != 0x100) {
                scale = (scale * D_8006CD98) >> 8;
            }
            d = D_800607FC - arg0[2];
            if (new_var < d) {
                d = new_var;
            }
            d -= lo;
            if (d < 0) {
                d = 0;
            }
            D_800607FC -= (d * scale) >> 8;
        }
    }
}

#ifdef NON_MATCHING
void func_800225C0(s32 *arg0, s32 arg1) {
    s32 sp18[4];
    s32 sp28[4];
    s32 pad[4];
    s32 sp48[4];
    s32 p[4];
    s32 r;

    sp18[0] = arg0[0];
    do {
        sp18[1] = arg0[1];
        sp18[2] = arg0[2];
        sp28[0] = D_800608CC[0];
        sp28[1] = D_800608CC[2];
        sp28[2] = D_800608CC[5];
        r = func_8002433C(sp28, sp18, &p[0], &p[1], &p[2]);
        if (r > 0) {
            r = func_80024658(sp28, sp18, p[1], 0, p[2], 1);
            func_80023E60(sp28, r, sp48, arg1 != 0 ? 0x100 : -1);
            func_800200A8(sp48[0], sp48[1], sp48[2], 0);
        } else if (r < 0) {
            r = func_80024658(sp28, sp18, p[0], 1, p[2], 1) + 0;
            func_80023E60(sp28, r, sp48, arg1 != 0 ? 0x100 : -1);
            func_800200A8(sp48[0], sp48[1], sp48[2], 0);
        } else {
            func_800200A8(D_800608CC[0], D_800608CC[2], D_800608CC[5], 0);
        }
    } while (0);
    func_800231B0(sp18);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800225C0);
#endif

#ifdef NON_MATCHING
void func_8002271C(s32 *arg0, s32 arg1) {
    s32 v18[3];
    s32 v28[8];
    s32 v48[3];
    s32 v58, v5C, v60;
    s32 r;
    s32 t;
    s32 t2;

    v18[0] = arg0[0];
    v18[1] = arg0[1];
    t2 = arg0[2];
    v28[0] = D_800608CC[0];
    v28[1] = D_800608CC[2];
    v28[2] = D_800608CC[5];
    v18[2] = t2;
    r = func_8002433C(v28, v18, &v58, &v5C, &v60);
    if (r > 0) {
        t = func_80024658(v28, v18, v5C, 0, v60, 1);
        func_80023E60(v28, t, v48, arg1 != 0 ? 0x100 : -1);
        func_800200A8(v48[0], v48[1], v48[2], 0);
    } else if (r < 0) {
        t = func_80024658(v28, v18, v58, 1, v60, 1);
        func_80023E60(v28, t, v48, arg1 != 0 ? 0x100 : -1);
        func_800200A8(v48[0], v48[1], v48[2], 0);
    } else {
        func_800200A8(D_800608CC[0], D_800608CC[2], D_800608CC[5], 0);
    }
    func_800231B0(v18);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_8002271C);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022878);

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022A5C);

#ifdef NON_MATCHING
#define UNK_80014260_OFFSET 0x13D
#define UNK_80014260_FLAGS 1
#define UNK_FIELD_OFFSET 0x10
#define UNK_INDEX_OFFSET 0x184
#define UNK_TABLE_STRIDE 3
#define UNK_STATE_5 5
#define UNK_D_8005F38C_FIELD 0x18

typedef struct Unk80022E74Inner {
    char pad0[UNK_INDEX_OFFSET];
    s32 unk_184;
} Unk80022E74Inner;

typedef struct Unk80022E74 {
    char pad0[UNK_FIELD_OFFSET];
    Unk80022E74Inner *unk_10;
    s32 unk_14[1];
} Unk80022E74;

extern s8 D_8005B994;
extern s32 D_8005F38C;
extern s32 D_8005BADC;
extern s32 D_8005BAE0;
extern s32 D_800608D4;

void func_80022E74(s32 *arg0) {
    s32 args[3];
    Unk80022E74 *p;

    args[0] = arg0[0];
    args[1] = arg0[1];
    args[2] = arg0[2];
    if (D_8005B99B != 0) {
        p = (Unk80022E74 *)func_80014260(&D_8005B99B + UNK_80014260_OFFSET, UNK_80014260_FLAGS, 0);
        func_800200A8((s32)p,
                      ((s32 *)p)[(UNK_FIELD_OFFSET / 4) + D_8005BADC * UNK_TABLE_STRIDE + p->unk_10->unk_184],
                      D_8005BAE0, 0);
        D_8005B994 = UNK_STATE_5;
        ((s32 *)D_8005F38C)[UNK_D_8005F38C_FIELD / 4] = D_800608CC[0];
        return;
    } else {
        func_800200A8(D_800608CC[0], D_800608D4, D_800608E0, 0);
        func_800231B0(args);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022E74);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_80022F58);

extern s32 D_8005F38C;

void func_80023084(s32 arg0, s32 arg1) {
    s32 *p;
    s32 n;
    s32 len;
    s32 kind;
    s32 step;

    p = (s32 *)func_80031AE8(arg0, 0x198, arg1, 0, 0, 1, 0, 0, 0);
    if (p != 0) {
        n = func_800322C0(arg0, 0x198, arg1, 1);
        if (n != 0) {
            do {
                kind = p[1];
                len = p[0];
                if (kind >= 0) {
                    func_8001CC10(0, D_8005F38C, kind, len - 2, p + 2);
                    step = len * 4;
                } else {
                    func_80012DC0();
                    step = len * 4;
                }
                n -= len;
                p = (s32 *)((char *)p + step);
            } while (n != 0);
        }
    }
}

typedef struct Unk80060918 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Unk80060918;

typedef struct Unk8005B9C4 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    Unk80060918 unkC;
} Unk8005B9C4;

extern s32 D_8003BEA4;
extern Unk80060918 D_80060918;

void func_80023154(void) {
    Unk8005B9C4 *state = (Unk8005B9C4 *)&D_8005B9C4;

    state->unk0 = 1;
    state->unk8 = 0x155;
    state->unk4 = D_8003BEA4;
    state->unkC = D_80060918;
}

INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800231B0);

#ifdef NON_MATCHING
s32 func_800236DC(s32 *arg0, s32 *arg1, s32 arg2) {
    s32 cur;
    s32 goal;
    s32 diff;
    s32 step;
    s32 vel;
    s32 limit;
    s32 t;

    cur = arg0[0];
    if (arg2 == 0) {
        arg2 = arg0[2];
    }
    if (arg2 != 0) {
        goal = (~arg2 >> 31) & 0x400;
        diff = goal - cur;
        if (cur != goal) {
            step = diff;
            if (diff < 0) {
                step = -step;
            }
            vel = arg0[1];
            if (arg1[0] >= step) {
                step = step >> arg1[3];
                if (step == 0) {
                    step = 1;
                }
                if (diff < 0) {
                    t = -step;
                } else {
                    t = step;
                }
                diff = t;
            }
            limit = arg1[1];
            if (limit < step) {
                t = limit;
                if (diff < 0) {
                    t = -t;
                }
                diff = t;
            }
            if (vel < diff) {
                vel += arg1[2];
                if (diff < vel) {
                    vel = diff;
                }
            } else if (diff < vel) {
                vel -= arg1[2];
                if (vel < diff) {
                    vel = diff;
                }
            }
            cur += vel;
            if (cur < 0) {
                cur = 0;
            } else if (cur >= 0x401) {
                cur = 0x400;
            }
            arg0[1] = vel;
            arg0[0] = cur;
        } else {
            arg0[1] = 0;
        }
    }
    return cur;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80020A24", func_800236DC);
#endif

typedef struct Unk800237F8 {
    s32 cur;
    s32 want;
    s32 count;
    s32 rest[3];
} Unk800237F8;

extern s32 D_8005B9BC[];
extern s32 D_8005B9C0;
extern Unk800237F8 D_8005BA28[];
extern char D_8005BB48[];
extern s32 D_80060914;
extern s32 D_80069948;
extern s32 D_8005F38C;

void func_800237F8(void) {
    s32 notFlag;
    s32 bits;
    s32 d;
    s32 i;
    Unk800237F8 *e;
    char *q;

    notFlag = (u32)*(s32 *)(D_8005F38C + 0xAC) >> 7;
    notFlag = notFlag ^ 1;
    notFlag = notFlag & 1;
    bits = 0;
    if (notFlag == 0) {
        d = (s32)((*(s32 *)(D_8005F38C + 0x70) - D_80060914) << 20) >> 20;
        if ((u32)(d - 0x71) < 0x71E) {
            bits = 2;
        } else if ((u32)(d + 0x78E) < 0x71E) {
            bits = 8;
        }
        if ((u32)(d + 0x38D) < 0x71B) {
            bits |= 4;
        } else if ((u32)(d + 0x471) >= 0x8E3) {
            bits |= 1;
        }
        if (D_8005B9BC[0] != bits) {
            D_8005B9BC[0] = bits;
            D_8005B9C0 = 0;
            bits = 0;
        } else {
            D_8005B9C0 = D_8005B9C0 + 1;
            if (D_8005B9C0 >= 16) {
                D_8005B9C0 = 15;
            } else {
                bits = 0;
            }
        }
        if (*(s32 *)(D_8005F38C + 0x100) & 0x04000000) {
            if (D_80069948 & 0x40) {
                bits |= 1;
            } else if (D_80069948 & 0x20) {
                bits |= 4;
            }
        } else {
            if (D_80069948 & 0x1000) {
                bits |= 1;
            } else if (D_80069948 & 0x4000) {
                bits |= 4;
            }
            if (D_80069948 & 0x2000) {
                bits |= 2;
            } else if (D_80069948 & 0x8000) {
                bits |= 8;
            }
        }
    }
    e = D_8005BA28;
    q = D_8005BB48;
    for (i = 0; i < 2; i++) {
        if (notFlag == 0) {
            if (i == 1) {
                if (bits & 1) {
                    if (e->want == i) {
                        e->want = 0;
                        e->count = 0;
                    }
                } else if (bits & 4) {
                    if (e->want == 0) {
                        e->want = i;
                        e->count = 0;
                    }
                }
            } else if (bits & 2) {
                if (e->want == 1) {
                    e->want = 0;
                    e->count = 0;
                }
            } else if (bits & 8) {
                if (e->want == 0) {
                    e->want = 1;
                    e->count = 0;
                }
            }
        }
        if (e->want != e->cur) {
            e->count = e->count + 1;
            if (e->count > 0) {
                e->cur = e->want;
                e->rest[1] = 0;
            }
        }
        func_800236DC(&e->rest[0], q, e->cur == 0 ? -1 : 1);
        e++;
        q += 0x10;
    }
}
