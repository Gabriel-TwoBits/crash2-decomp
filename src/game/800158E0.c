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

#ifdef NON_MATCHING
extern Unk80063458 *D_80063458[];
#define RETAIL_STATE_LOAD 2
#define RETAIL_STATE_PAL 0x28
#define CLEAR_OFFSET 0x18
#define CLEAR_BYTES 0x800
#define SHARED_BUF_BYTES 0x1770
#define ENTRY_BUF_BYTES 0x16400
#define ENTRY_BUF_BYTES_PAL 0x2C800
#define ENTRY_BUF_OFFSET 4
#define ENTRY_SHARED_OFFSET 0x208C
#define ALLOC_FAIL_EXIT_CODE -0xF
#define RETURN_VALUE -0xFF

extern void exit(s32);
extern s32 D_8005F3EC;
extern u8 *D_80063454;

s32 func_800161F8(void) {
    s32 i;
    s32 shared;
    s32 buf;
    s32 size;
    u8 *entry;

    if (D_8005F3EC == RETAIL_STATE_LOAD) {
        func_80036368();
    }
    func_8003BC5C((u8 *)D_80063458[0] + CLEAR_OFFSET, CLEAR_BYTES);
    func_8003BC5C(D_80063454 + CLEAR_OFFSET, CLEAR_BYTES);
    i = 0;
    shared = func_8001148C(SHARED_BUF_BYTES);
    if (D_8006344C.count > 0) {
        entry = (u8 *)D_8006344C.e;
        do {
            size = ENTRY_BUF_BYTES;
            *(s32 *)(entry + ENTRY_SHARED_OFFSET) = shared;
            if (D_8005F3EC == RETAIL_STATE_PAL) {
                size = ENTRY_BUF_BYTES_PAL;
            }
            buf = func_8001148C(size);
            *(s32 *)(entry + ENTRY_BUF_OFFSET) = buf;
            if (buf == 0) {
                exit(ALLOC_FAIL_EXIT_CODE);
            }
            func_800163C4((s32 *)entry);
            i++;
            entry += sizeof(*D_8006344C.e);
        } while (i < D_8006344C.count);
    }
    return RETURN_VALUE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800161F8);
#endif

#ifdef NON_MATCHING
#define STATE_FLUSH 2
#define RESULT_DONE -0xFF

extern s32 D_800654E8;
extern s32 DrawSync(s32);

s32 func_800162FC(void) {
    s32 i;

    i = 0;
    func_800163B4(D_80063458[0]);
    DrawSync(0);
    func_800116C0(D_800654E8);
    if (D_8006344C.count > 0) {
        do {
            func_800116C0(D_8006344C.e[i].pad4[0]);
            i++;
        } while (i < D_8006344C.count);
    }
    if (D_8005F3EC == STATE_FLUSH) {
        func_800363D0();
    }
    return RESULT_DONE;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800162FC);
#endif

INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800163B4);

void func_800163C4(s32 *arg0) {
    arg0[2] = arg0[1];
    func_800163B4(arg0);
}


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

#ifdef NON_MATCHING
extern u8 D_8005B500[];
extern s32 D_8006CB94;
s32 GetTPage(s32, s32, s32, s32);
void SetDrawMode(void *, s32, s32, s32, s32 *);
void SetPolyF4(void *);
void SetSemiTrans(void *, s32);

void func_80016B08(u32 arg0) {
    u8 *p;
    u8 *q;
    u8 *t;
    s32 lo;
    s32 hi;

    q = (u8 *)D_80063458[0]->unk8;
    D_80063458[0]->unk8 = (s32)(q + 0xC);
    p = (u8 *)D_80063458[0]->unk8;
    D_80063458[0]->unk8 = (s32)(p + 0x18);
    if (arg0 != 0) {
        t = D_8005B500 + (arg0 >> 4);
        lo = t[0];
        hi = t[1];
        *(s16 *)(p + 8) = 0;
        *(s16 *)(p + 0x10) = 0;
        *(s16 *)(p + 0xA) = -0xC;
        *(s16 *)(p + 0xE) = -0xC;
        *(s16 *)(p + 0xC) = 0x200;
        *(s16 *)(p + 0x12) = 0xE4;
        *(s16 *)(p + 0x14) = 0x200;
        *(s16 *)(p + 0x16) = 0xE4;
        arg0 = ((u32)((hi - lo) * (arg0 & 0xF)) >> 4) + lo;
        p[4] = arg0;
        p[5] = arg0;
        p[6] = arg0;
        SetPolyF4(p);
        SetSemiTrans(p, 1);
        AddPrim(&D_80063458[0]->padC[0x2008], p);
        SetDrawMode(q, 1, 0, GetTPage(0, (D_8006CB94 & 0x800000) ? 1 : 2, 0, 0) & 0xFFFF, 0);
        AddPrim(&D_80063458[0]->padC[0x2008], q);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016B08);
#endif

typedef struct {
    s32 a;
    s32 pad;
    s16 b;
} Unk80060884;

#ifdef NON_MATCHING
typedef struct {
    u8 pad0[4];
    u8 r, g, b;
    u8 pad7;
    s16 x0, y0, x1, y1, x2, y2, x3, y3;
} PolyF4;

extern Unk80060884 D_80060884;
void SetGeomScreen(s32);

#define SCREEN_MODE_ALT 1
#define SCREEN_X_OFFSET 0x2034
#define SCREEN_Y_OFFSET 0x2036
#define PRIM_SIZE 0x18
#define OT_OFFSET 0x2008
#define SCREEN_CENTER_X 0x200
#define BAR_TOP -0xC
#define BAR_BOTTOM 0xE4

void func_80016C58(s32 arg0, s32 arg1) {
    PolyF4 *p;
    s32 w;
    u32 x;
    u32 y;
    s16 right, left, top, bot, mid;
    s32 ret;

    p = (PolyF4 *)D_80063458[0]->unk8;
    w = arg0 * 25;
    x = (u32)(arg0 * 100) >> 3;
    D_80063458[0]->unk8 += PRIM_SIZE;
    if (*(s32 *)D_80063454 == SCREEN_MODE_ALT) {
        *(s16 *)(D_80063454 + SCREEN_X_OFFSET) = SCREEN_CENTER_X - x;
    } else {
        *(s16 *)(D_80063454 + SCREEN_X_OFFSET) = -x;
    }
    *(s16 *)(D_80063454 + SCREEN_Y_OFFSET) =0xC - ((u32)(arg0 * 60) >> 3);
    if (arg0 != 0) {
        if (arg1 == 0) {
            y = 0;
            x = 0;
        } else {
            x = (u32)(arg1 * 100) >> 3;
            y = (u32)(arg1 * 60) >> 3;
        }
        right = x + SCREEN_CENTER_X;
        left = right - w;
        top = y + BAR_TOP;
        bot = y + BAR_BOTTOM;
        p->x0 = left; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = left; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[OT_OFFSET], p);
        p = (PolyF4 *)D_80063458[0]->unk8;
        D_80063458[0]->unk8 += PRIM_SIZE;
        mid = bot - arg0 * 15;
        p->x0 = x; p->y0 = mid; p->x1 = left; p->y1 = mid;
        p->x2 = x; p->y2 = bot; p->x3 = left; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[OT_OFFSET], p);
        p = (PolyF4 *)D_80063458[0]->unk8;
        w = (u32)(arg0 * 9) >> 3;
        D_80063458[0]->unk8 += PRIM_SIZE;
        right = x + w;
        p->x0 = x; p->y0 = top; p->x1 = right; p->y1 = top;
        p->x2 = x; p->y2 = bot; p->x3 = right; p->y3 = bot;
        p->r = 0; p->g = 0; p->b = 0;
        SetPolyF4(p);
        AddPrim(&D_80063458[0]->padC[OT_OFFSET], p);
    }
    ret = ((s32 (*)(void))func_80023D7C)();
    ret = ret - (ret / 24) * arg0;
    if (D_80060884.a != ret) {
        D_80060884.a = ret;
        SetGeomScreen(ret);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80016C58);
#endif

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

s32 func_80016F44(s32 arg0, s32 arg1, s32 arg2) {
    u8 *s3;
    u8 *s2;
    s32 n;
    s32 i;
    s32 idx;
    s32 e;

    idx = -1;
    do { s3 = *(u8 **)((u8 *)arg0 + 0x10); } while (0);
    s2 = *(u8 **)((u8 *)arg0 + 0x20);
    n = *(s32 *)(s3 + 0x1C);
    if (arg2 != 0x6396347F) {
        for (i = 0; i < *(u32 *)(s3 + 0x28); i++) {
            e = ((s32 *)(s3 + 0x2C))[i];
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
                func_80018474(s2, s3 + 0x2C);
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

#ifdef NON_MATCHING
typedef struct { s32 pad0[2]; s32 kind; } Obj170B4;
typedef struct { char pad[0x404]; s32 count; } Cnt170B4;
typedef struct { char pad[0x18]; Cnt170B4 *list; } Hdr170B4;
extern Hdr170B4 D_80067588;
extern Cnt170B4 *D_800675A0;

s32 func_800170B4(s32 arg0, s32 arg1) {
    s32 pad[2];
    s32 i;
    Obj170B4 *o;
    Hdr170B4 *h = &D_80067588;
    Hdr170B4 *new_var;

    i = 0;
    if (D_800675A0->count > 0) {
        new_var = h;
        do {
            o = *(Obj170B4 **)((char *)new_var->list + i * 8 + 0x520);
            if (!((s32)o & 3)) {
                if (o->kind != 2) {
                    if (o->kind == 3) {
                        func_80016F44((s32)o, arg1, arg0);
                    }
                } else {
                    func_800171E8(o, arg1, arg0);
                }
            }
            i++;
        } while (i < new_var->list->count);
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800170B4);
#endif

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

#ifdef NON_MATCHING
#define TEXT_OFFSET 0xC
#define STRING_INDEX_SHIFT 8
#define ASCII_SPACE 0x20

void func_800175D8(u8 *arg0, s32 arg1, u8 *arg2, s32 arg3, s32 arg4) {
    s32 skip;
    s32 i;
    s32 pos;
    u8 c;

    pos = 0;
    skip = arg1 >> STRING_INDEX_SHIFT;
    for (i = 0; i < skip; i++) {
        /* skip one NUL-terminated string */
        do {
            c = arg0[pos + TEXT_OFFSET];
            pos++;
        } while (c != 0);
    }

    i = 0;
    if (arg3 == 0) {
        return;
    }
    do {
        c = *arg2;
        if (c == 0) {
            break;
        }
        if (c < ASCII_SPACE) {
            c = ASCII_SPACE;
        }
        if (c == 0x7B) {
            c = 0x28;
        }
        if (c == 0x7D) {
            c = 0x29;
        }
        if (c == 0x7E) {
            c = 0x7B;
        }
        if (c == 0x25) {
            c = 0x7D;
        }
        if (c == 0x26) {
            c = 0x5F;
        }
        arg0[pos + i + TEXT_OFFSET] = c;
        arg2++;
        arg3--;
        i++;
    } while (arg3 != 0);

    while (arg3 != 0) {
        arg0[pos + i + TEXT_OFFSET] = ASCII_SPACE;
        arg3--;
        i++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800175D8);
#endif

#ifdef NON_MATCHING
typedef struct { s16 x, y, w, h; } Rect;
s32 func_80017380(s32, s32, s32, Rect *);

s32 func_800176DC(s32 mode, s32 a1, s32 a2, u8 *i3, u8 *i4, s32 flag) {
    Rect r;
    char buf[0x110];
    u8 *tmp = (u8 *)((s32)&buf[15] & ~0xF);
    u8 *s;
    u8 *d;
    s32 n;

    if (i3 != 0) {
        func_80017380(2, a1, a2, &r);
        if (mode == 1) {
            if ((s32)i3 & 0xF) {
                flag = 0;
                s = i3; d = tmp;
                n = r.w * r.h * 2;
                if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
                LoadImage(&r, tmp);
                DrawSync(0);
            } else {
                LoadImage(&r, i3);
                if (flag == 0) DrawSync(0);
            }
        } else if (((s32)i3 & 0xF) != 0) {
            flag = 0;
            StoreImage(&r, tmp);
            DrawSync(0);
            s = tmp; d = i3;
            n = r.w * r.h * 2;
            if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
        } else {
            StoreImage(&r, i3);
            if (flag == 0) DrawSync(0);
        }
    }
    if (i4 != 0) {
        func_80017380(1, a1, a2, &r);
        if (mode == 1) {
            if ((s32)i4 & 0xF) {
                flag = 0;
                s = i4; d = tmp;
                n = r.w * r.h * 2;
                if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
                LoadImage(&r, tmp);
                DrawSync(0);
            } else {
                LoadImage(&r, i4);
                if (flag == 0) DrawSync(0);
            }
        } else if (((s32)i4 & 0xF) != 0) {
            flag = 0;
            StoreImage(&r, tmp);
            DrawSync(0);
            s = tmp; d = i4;
            n = r.w * r.h * 2;
            if (n-- != 0) { do { *d++ = *s++; } while (n-- != 0); }
        } else {
            StoreImage(&r, i4);
            if (flag == 0) DrawSync(0);
        }
    }
    return flag;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_800176DC);
#endif

void SetColorMatrix(void *);
void SetFarColor(s32, s32, s32);
void SetGeomOffset(s32, s32);
void SetGeomScreen(s32);
extern s32 D_80060144, D_80060148, D_8006014C, D_800607F8, D_800607FC, D_80060800, D_80060804, D_80060808, D_8006083C, D_80060840, D_80060844, D_80060860;
extern s16 D_80060864, D_80060866, D_80060868;
extern s32 D_80060878, D_8006087C, D_80060880;
extern Unk80060884 D_80060884;
extern s16 D_8006088E, D_80060890, D_80060892, D_80060894, D_80060896, D_80060898, D_8006089A, D_8006089C;
extern s32 D_8006CC20, D_8006CC24;

void func_8001798C(void) {
    D_8006014C = 0;
    D_80060148 = 0;
    D_80060144 = 0;
    D_80060884.a = 0x120;
    SetGeomScreen(0x120);
    SetGeomOffset(0, 0);
    D_80060868 = 0x1000;
    D_80060864 = 0;
    D_80060866 = 0;
    D_80060878 = 0;
    D_8006087C = 0;
    D_80060880 = 0;
    D_80060884.b = 0x200;
    D_80060892 = 0x200;
    D_80060898 = 0x200;
    D_8006088E = 0x200;
    D_80060894 = 0x200;
    D_8006089A = 0x200;
    D_80060890 = 0x200;
    D_80060896 = 0x200;
    D_8006089C = 0x200;
    SetFarColor(0, 0, 0);
    SetColorMatrix(&D_80060884.b);
    func_80018168((s32 *)((char *)&D_80060884 - 0x90));
    *(s32 *)((char *)&D_80060884 - 0x90) = 0;
    D_800607F8 = 0;
    D_800607FC = 0x1F400;
    D_80060800 = 0;
    D_80060804 = 0;
    D_80060808 = 0;
    D_8006083C = 0;
    D_80060840 = 0xE1000;
    D_80060844 = 0x5DC000;
    D_80060860 = 0;
    D_8006CC20 = 0;
    D_8006CC24 = 0;
}

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

#ifdef NON_MATCHING
void func_80018194(s32 *v, s16 *m, s32 *out) {
    out[0] = (m[0] * v[0] + m[1] * v[1] + m[2] * v[2]) >> 12;
    out[1] = (m[3] * v[0] + m[4] * v[1] + m[5] * v[2]) >> 12;
    out[2] = (m[6] * v[0] + m[7] * v[1] + m[8] * v[2]) >> 12;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018194);
#endif

#ifdef NON_MATCHING
#define ENTRY_STRIDE 0x30
#define FIRST_ENTRY_OFFSET 4
#define POS_SHIFT 8
#define ACTIVE_MASK 0xFF
#define MASK_QUERY_ID 0x27F
#define MASK_QUERY_MODE 2
#define SHAPE_FLAG_ZERO_OFFSET 1

extern s32 D_800607F4;
extern s32 D_800608D4;
extern s32 D_800608E8;

void func_80018294(s32 *arg0) {
    s32 mask;
    s32 i;
    s32 off;
    s32 *entry;
    s32 *src;
    s32 *shape;
    s32 dx;
    s32 dy;
    s32 dz;

    mask = func_800321D4(D_800608D4, MASK_QUERY_ID, -1, D_800608E8, 0, MASK_QUERY_MODE, 0);
    entry = arg0;
    off = FIRST_ENTRY_OFFSET;
    for (i = 0; i < *arg0; i++) {
        if (((mask & ACTIVE_MASK) >> i) & 1) {
            src = (s32 *)func_80014260((u8 *)arg0 + off, 1, 0);
            shape = (s32 *)src[4];
            if (shape[3] == SHAPE_FLAG_ZERO_OFFSET) {
                entry[2] = 0;
                entry[3] = 0;
                entry[4] = 0;
            } else {
                dx = shape[0] - (D_800607F4 >> POS_SHIFT);
                dy = shape[1] - ((u32)D_800607F8 >> POS_SHIFT);
                dz = shape[2] - (D_800607FC >> POS_SHIFT);
                entry[2] = dx;
                entry[3] = dy;
                entry[4] = dz;
            }
            entry[5] = (s32)shape;
            entry[8] = src[5];
            entry[6] = src[6];
            entry[7] = src[7];
            entry[11] = src[8];
            entry[9] = src[9];
            entry[12] = (s32)(shape + 11);
            entry[10] = src[10];
        }
        entry += ENTRY_STRIDE / 4;
        off += ENTRY_STRIDE;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018294);
#endif

extern s32 D_8005F390;
extern s32 D_80060944;
extern s32 D_80062A18;

void func_80041E5C(s32, Unk80063458 *, void *, s32);

void func_80018434(void) {
    func_80041E5C(D_8005F390, D_80063458[0], &D_80062A18, D_80060944);
}

#ifdef NON_MATCHING
typedef struct {
    u8 pad0;
    u8 b1;
    u16 h2;
    u8 pad4;
    u8 b5;
    u16 h6;
    u8 pad8;
    u8 b9;
    u8 padA;
    u8 bB;
} Unk80018474;

void func_80018474(void *a0, void *a1) {
    Unk80018474 *arg0 = a0;
    s32 **arg1 = a1;
    s32 idx;
    s32 v;
    u32 r;
    u32 flag;
    u32 hi;

    idx = arg0->h6 >> 10;
    if (!(arg0->h6 & 0x200)) {
        v = func_800143C0(arg1 + idx, 0);
        if (v != 0) {
            r = (u32)func_80014260((void *)v, 3, 0);
            hi = (r >> 4) & 0x6030;
            flag = r & 0x80;
            arg0->h6 = (arg0->h6 & 0xFFE3) | (r & 0x1C) | 0x200;
            arg0->b5 = (arg0->b5 & 0x7F) | flag;
            arg0->b9 = (arg0->b9 & 0x7F) | flag;
            arg0->bB = (arg0->bB & 0x7F) | flag;
            arg0->h2 = (arg0->h2 & 0x9FCF) | hi;
            arg0->b1 = (arg0->b1 & 0x7F) | flag;
        }
    }
    *(volatile s32 *)arg1[idx];
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018474);
#endif

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
        func_80019BBC((s32 *)arg0, 0);
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
                func_80019BBC((s32 *)arg0, 0);
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

#ifdef NON_MATCHING
#define NODE_KIND_LIST4 2



typedef struct Unk80018CAC {
    s32 kind;
    Unk80018AA8 *children4;
    u8 pad[0x44];
    Unk80018AA8 *children;
} Unk80018CAC;

s32 func_80018AA8(Unk80018AA8 *arg0, Cb80018AA8 arg1, void *arg2);

s32 func_80018CAC(Unk80018CAC *arg0, Cb80018AA8 arg1, void *arg2) {
    Unk80018AA8 *node;
    Unk80018AA8 *next;

    if (arg0->kind == NODE_KIND_LIST4) {
        node = arg0->children4;
    } else {
        node = arg0->children;
    }
    while (node != 0) {
        next = node->next;
        func_80018AA8(node, arg1, arg2);
        node = next;
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018CAC);
#endif

#ifdef NON_MATCHING
s32 func_80018B50(void *arg0, s32 (*arg1)(void *, s32), s32 arg2);

typedef struct Unk80018D28 {
    s32 kind;
    struct Unk80018D28 *first;
    char pad[0x40];
    struct Unk80018D28 *next;
    struct Unk80018D28 *head;
} Unk80018D28;

s32 func_80018D28(void *arg0, void *arg1, void *arg2) {
    int is_kind2;
    Unk80018D28 *node;
    Unk80018D28 *next;

    is_kind2 = ((Unk80018D28 *)arg0)->kind == 2;
    if (is_kind2) {
        node = ((Unk80018D28 *)arg0)->first;
    } else {
        node = ((Unk80018D28 *)arg0)->head;
    }
    while (node != 0) {
        next = node->next;
        if (func_80018B50(node, (s32 (*)(void *, s32))arg1, (s32)arg2) == -0x19) {
            func_80019BBC((s32)arg0, 0);
        }
        node = next;
    }
    return -0xFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018D28);
#endif

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

#ifdef NON_MATCHING
#define F(p, T, off) (*(T *)((u8 *)(p) + (off)))


s32 func_80018E68(void *arg0, void *arg1) {
    s32 sp10;
    s32 d;
    s32 sh;
    s32 new_var;
    s32 t;
    s32 st;
    s32 m;
    s32 s0;
    void *tbl;

    if (arg0 == F(arg1, void *, 4)) {
        return 0;
    }
    sh = F(F(F(arg0, void *, 0xC), void *, 0x10), s32, 4) >> 8;
    if (!((1 << sh) & F(arg1, u32, 0))) {
        return 0;
    }
    d = func_80030FEC((s32 *)((u8 *)arg0 + 0x60), (s32 *)((u8 *)F(arg1, void *, 4) + 0x60));
    if (d < F(arg1, s32, 0xC)) {
        t = F(arg1, s32, 0x10);
        if (t == 0xFF) {
            s0 = 0xFE;
        } else {
            tbl = F(arg0, void *, 0xC);
            s0 = *(u16 *)(F(tbl, u8 *, 0x1C) + (t >> 8) * 2);
            if (s0 == 0xFF) {
                if (t == 0x300) {
                    st = F(arg0, s32, 0x108);
                    new_var = F(arg0, s32, 0xB0);
                    if ((u32)(st - 3) >= 2 && st != 2 && !(new_var & 2)) {
                        s0 = 0xFE;
                        goto cmp;
                    }
                }
                if (F(arg1, s32, 0x10) == 0xA00 && !(F(arg0, s32, 0x100) & 0x800)) {
                    s0 = 0xFE;
                    goto cmp;
                }
                if (F(arg1, s32, 0x10) != 0x1D00) {
                    return 0;
                }
                if (F(arg0, s32, 0x100) & 0x02000000) {
                    return 0;
                }
                s0 = 0xFE;
                goto cmp;
            }
            if (!(s0 & 0x8000)) {
                m = F(arg0, s32, 0xB0);
                st = F(arg0, s32, 0x108);
                t = *(s32 *)(F(tbl, u8 *, 0x20) + s0 * 16);
                if ((u32)(st - 3) < 2 || st == 2) {
                    m |= 0x1002;
                }
                if (m & t) {
                    return 0;
                }
                if (!(s0 & 0x8000)) {
                    goto cmp;
                }
            }
            if (F(arg1, s32, 0x10) == 0xF00) {
                sp10 = 0x100;
                F(arg0, void *, 0x5C) = F(arg1, void *, 4);
                func_8001D22C(arg0, (u16)s0, 1, &sp10);
                if (F(F(arg1, void *, 4), s32, 0xD8) == 0) {
                    return 0;
                }
            }
        }
cmp:
        if (s0 != 0xFF) {
            F(arg1, void *, 8) = arg0;
            F(arg1, s32, 0xC) = d;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800158E0", func_80018E68);
#endif

typedef struct { char pad[0x100]; s32 flags; } T;

T *func_80019074(T *a, s32 m) {
    if (a->flags & m) {
        return a;
    }
    return 0;
}
