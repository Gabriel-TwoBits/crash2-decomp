#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F1D4 = 0;
s32 D_8005F1D8 = 0;
s32 D_8005F1DC = 0;
s32 D_8005F1E0 = 0;
s32 D_8005F1E4 = 0x808080;
s32 D_8005F1E8 = 0x8000;

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800158E0);

void SetDefDrawEnv(void *env, s32 x, s32 y, s32 w, s32 h);
void PutDrawEnv(void *env);
void TermPrim(void *p);
void DrawPrim(void *p);

typedef struct {
    u8 pad0[0x3];
    u8 len;
    u8 r0, g0, b0;
    u8 code;
    s16 x0, y0;
    s16 w, h;
} Fill;

void func_8001601C(void) {
    u8 env[0x60];
    Fill f;

    SetDefDrawEnv(env, 0, 0, 0x400, 0x200);
    env[0x17] = 1;
    PutDrawEnv(env);
    TermPrim(&f);
    f.len = 3;
    f.code = 2;
    f.w = 0x3FF;
    f.h = 0x1FF;
    f.r0 = 0;
    f.g0 = 0;
    f.b0 = 0;
    f.x0 = 0;
    f.y0 = 0;
    DrawPrim(&f);
}

typedef struct {
    s32 idx;
    s32 pad4[2];
    s32 f0C;
    s32 pad10;
    s32 f14;
    u8 pad18[0x2018 - 0x18];
    u8 disp[0x14];
    u8 draw[0x2094 - 0x202C];
} Unk800160B0Entry;

typedef struct {
    s32 count;
    Unk800160B0Entry *cur;
    Unk800160B0Entry *other;
    Unk800160B0Entry *first;
    Unk800160B0Entry e[1];
} Unk800160B0Gfx;

extern Unk800160B0Gfx D_8006344C;
extern s32 D_8003BEA4;
extern s32 D_8006CCDC;
extern s32 D_80060944;
void PutDispEnv(void *env);
void SetDefDispEnv(void *env, s32 x, s32 y, s32 w, s32 h);
void SetDispMask(s32 mask);
void SetGraphDebug(s32 level);

void func_800160B0(void) {
    s32 i;
    Unk800160B0Entry *e;
    D_80060944 = 0;
    ResetGraph(0);
    SetGraphDebug(0);
    func_8001601C();
    D_8006344C.count = 2;
    for (i = 0; i < D_8006344C.count; i++) {
        e = &D_8006344C.e[i];
        e->idx = i;
        e->f0C = 0;
        SetDefDispEnv(e->disp, i << 9, 0, 0x200, 0xF0);
        D_8006CCDC = 0;
        *(s16 *)(e->disp + 8) = 0;
        SetDefDrawEnv(e->draw, i << 9, 0xC, 0x200, 0xD8);
        *(s16 *)(e->draw + 8) = i << 9;
        *(s16 *)(e->draw + 10) = 0xC;
        e->draw[0x16] = 1;
        e->draw[0x17] = 0;
        if (i == 0) {
            D_8006344C.first = e;
            D_8006344C.cur = e;
        }
        if (i == 1) {
            D_8006344C.other = e;
        }
        e->f14 = 0x22;
        e->f0C = D_8003BEA4;
    }
    PutDispEnv(D_8006344C.cur->disp);
    PutDrawEnv(D_8006344C.other->draw);
    SetDispMask(1);
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800161F8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800162FC);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800163B4);

void func_800163C4(s32 *arg0) {
    arg0[2] = arg0[1];
    func_800163B4(arg0);
}

typedef struct {
    u8 pad0[0x8];
    s32 unk8;
    u8 padC[0x202C - 0xC];
    u16 unk202C;
    u16 unk202E;
    u8 pad2030[0x2088 - 0x2030];
    void *unk2088;
    u8 pad208C[0x2090 - 0x208C];
    char *unk2090;
} Unk80063458;

extern Unk80063458 *D_80063458[];

void func_800163E8(s32 arg0) {
    s32 sum;

    sum = D_80063458[0]->unk8 + arg0;
    /* Both branches store the same value; the branch is what puts the sum in $a0. */
    if (D_80063458[0]->unk8) {
        D_80063458[0]->unk8 = sum;
    } else {
        D_80063458[0]->unk8 = sum;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016408);

void *func_80016418(s32 arg0) {
    void **temp;

    temp = (void **)D_80063458[0]->unk2090;
    D_80063458[0]->unk2090 = (char *)temp + arg0;
    *temp = D_80063458[0]->unk2088;
    D_80063458[0]->unk2088 = temp;
    return temp;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001645C);

typedef struct {
    u8 pad0[3];
    u8 len;
    u8 r, g, b;
    u8 code;
    s16 x, y;
    s16 w, h;
} Unk80016A60;


void func_80016A60(s32 arg0, s32 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6) {
    Unk80016A60 *p;

    p = (Unk80016A60 *)D_80063458[0]->unk8;
    D_80063458[0]->unk8 = (s32)((u8 *)p + 0x10);
    p->len = 3;
    p->code = 2;
    p->w = arg2;
    p->h = arg3;
    p->r = arg4;
    p->g = arg5;
    p->b = arg6;
    p->x = D_80063458[0]->unk202C + arg0;
    p->y = D_80063458[0]->unk202E + arg1;
    AddPrim(&D_80063458[0]->padC[0xC], p);
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016B08);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016C58);

void func_80016EFC(void) {
}

s32 func_80016F04(s32 arg0) {
    if (arg0 < 0) {
        goto L22;
    }
    if (arg0 < 0x13) {
        arg0 = 0x11;
    } else if (arg0 < 0x24) {
L22:
        arg0 = 0x22;
    } else if (arg0 < 0x35) {
        arg0 = 0x33;
    }
    return arg0;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016F44);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800170B4);

void func_80017198(s32 arg0) {
    func_80016F44(arg0, 1, 0x6396347F);
}

void func_800171C0(int a0) {
    func_80016F44(a0, 0, 0x6396347F);
}

s32 func_800171E8(void *arg0, s32 arg1, s32 arg2) {
    u8 *s3;
    u8 *s2;
    s32 n;
    s32 i;
    s32 idx;
    s32 e;

    idx = -1;
    do { s3 = *(u8 **)((u8 *)arg0 + 0x10); } while (0);
    s2 = *(u8 **)((u8 *)arg0 + 0x1C);
    n = *(s32 *)(s3 + 0x34);
    if (arg2 != 0x6396347F) {
        for (i = 0; i < *(s32 *)(s3 + 0x40); i++) {
            e = ((s32 *)(s3 + 0xC))[i];
            if (e & 1) {
                idx = i;
                if (arg2 == e) {
                    goto found;
                }
            } else {
                idx = i;
                if (arg2 == *(s32 *)(e + 4)) {
                    goto found;
                }
            }
        }
        return -0xFF;
    }
found:
    if (arg1 != 0) {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                func_80018474(s2, s3 + 0xC);
            }
            s2 += 0xC;
        }
    } else {
        for (i = 0; i < n; i++) {
            if (idx == -1 || idx == ((*(u16 *)(s2 + 6) >> 10) & 7)) {
                *(u16 *)(s2 + 6) &= 0xFDFF;
            }
            s2 += 0xC;
        }
    }
    return -0xFF;
}

void func_80017358(s32 a0) {
    func_800171E8((void *)a0, 1, 0x6396347F);
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017380);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800175D8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800176DC);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001798C);

typedef struct Unk80017AF8 {
    u16 a, b, c;
    s16 d, e, f;
    s16 g, h, i;
    s16 pad;
    s32 x, y, z;
} Unk80017AF8;

void func_80017AF8(Unk80017AF8 *dst, Unk80017AF8 *src) {
    dst->a = src->a;
    dst->b = src->b;
    dst->c = src->c;
    dst->d = -(src->d * 5) >> 3;
    dst->e = -(src->e * 5) >> 3;
    dst->f = -(src->f * 5) >> 3;
    dst->g = -src->g;
    dst->h = -src->h;
    dst->i = -src->i;
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80017BC4);

void func_80018168(s32 *p) {
    p[3] = 0;
    p[4] = 0;
    p[5] = 0;
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[6] = 0x1000;
    p[7] = 0x1000;
    p[8] = 0x1000;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018194);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018294);

extern s32 D_8005F390;
extern s32 D_80060944;
extern s32 D_80062A18;

void func_80041E5C(s32, Unk80063458 *, void *, s32);

void func_80018434(void) {
    func_80041E5C(D_8005F390, D_80063458[0], &D_80062A18, D_80060944);
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018474);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_8001856C);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800186D8);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800189C4);

typedef struct Node {
    s32 unk0;
    struct Node *children4; /* the child list's head when unk0 == 2 */
    u8 pad08[0x44 - 0x08];
    struct Node *parent;
    struct Node *next;
    struct Node *children; /* the child list's head otherwise */
} Node;

/* Moves node from its parent's child list to the head of parent's. Returns -0x18 when node is missing from its old
 * parent's list. The success path has no return statement: $v0 is left holding the 2 of the last comparison. */
s32 func_800189CC(Node *node, Node *parent) {
    Node *old;
    Node *it;

    old = node->parent;
    if (old != 0) {
        it = old->unk0 == 2 ? old->children4 : old->children;
        if (it == node) {
            *(old->unk0 == 2 ? &old->children4 : &old->children) = node->next;
        } else {
            while (it != 0 ? it->next != node : 1) {
                if (it == 0) {
                    return -0x18;
                }
                it = it->next;
            }
            it->next = node->next;
        }
    }
    node->parent = parent;
    node->next = parent->unk0 == 2 ? parent->children4 : parent->children;
    *(parent->unk0 == 2 ? &parent->children4 : &parent->children) = node;
}

typedef struct Unk80018AA8 {
    s32 f0;
    u8 pad[0x44];
    struct Unk80018AA8 *next;
    struct Unk80018AA8 *child;
} Unk80018AA8;

typedef s32 (*Cb80018AA8)(Unk80018AA8 *, void *);

s32 func_80018AA8(Unk80018AA8 *arg0, Cb80018AA8 arg1, void *arg2) {
    s32 ret;
    Unk80018AA8 *p;
    Unk80018AA8 *nx;

    ret = arg1(arg0, arg2);
    if (ret == -0x19) {
        func_80019BBC((s32)arg0, 0);
        return -0x19;
    }
    p = arg0->child;
    while (p != 0 && p->f0 != 0) {
        nx = p->next;
        func_80018AA8(p, arg1, arg2);
        p = nx;
    }
    return ret;
}

s32 func_80018B50(void *arg0, s32 (*arg1)(void *, s32), s32 arg2) {
    char *child;
    char *next;

    child = *(char **)((char *)arg0 + 0x4C);
    if (child != 0) {
        do {
            next = *(char **)(child + 0x48);
            if (func_80018B50(child, arg1, arg2) == -0x19) {
                func_80019BBC((s32)arg0, 0);
            }
            child = next;
        } while (child != 0);
    }
    return arg1(arg0, arg2);
}

s32 func_80018BE4(u8 *arg0, void *arg1, s32 arg2) {
    s32 r;
    u8 *c;
    u8 *next;

    r = 0;
    if (arg0 == 0) {
        return 0;
    }
    if (*(s32 *)arg0 != 2) {
        r = ((s32 (*)(u8 *, s32))arg1)(arg0, arg2);
        if (r != 0) {
            return r;
        }
    }
    if (*(s32 *)arg0 == 2) {
        c = *(u8 **)(arg0 + 4);
    } else {
        c = *(u8 **)(arg0 + 0x4C);
    }
    while (c != 0 && r == 0) {
        next = *(u8 **)(c + 0x48);
        r = func_80018BE4(c, arg1, arg2);
        c = next;
    }
    return r;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018CAC);

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018D28);

extern u8 D_8006CB18[];

void func_80018DD0(void *arg0, void *arg1, void *arg2) {
    u8 *var_s0;
    s32 var_s1;

    var_s1 = 0;
    var_s0 = D_8006CB18;
    do {
        ((void (*)(void *, void *, void *))arg0)(var_s0, arg1, arg2);
        var_s1 += 1;
        var_s0 += 8;
    } while (var_s1 < 8);
}

void *func_80018E48(void *arg0, s32 arg1) {
    if (*(s32 *)((u8 *)arg0 + 0xB8) == arg1) {
        return arg0;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018E68);

typedef struct { char pad[0x100]; s32 flags; } T;

T *func_80019074(T *a, s32 m) {
    if (a->flags & m) {
        return a;
    }
    return 0;
}
