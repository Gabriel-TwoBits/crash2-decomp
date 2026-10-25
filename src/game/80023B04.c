#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/80023B04", D_80010380);
INCLUDE_RODATA("asm/nonmatchings/game/80023B04", D_800103C0);
INCLUDE_RODATA("asm/nonmatchings/game/80023B04", D_80010400);

#ifdef NON_MATCHING
typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Arg80023B04;

extern s32 g_CameraPath[];
extern s32 g_CameraPathKind;
extern s32 D_800608D8;
extern s32 D_800608DC;
extern s32 D_800608FC;
extern s32 D_8005BA28[];

#define KEY_QUERY 0x29
#define IDX_QUERY 0x173
#define LOOKUP_ID 0x142
#define QUERY_FLAGS 2
#define LOOKUP_FLAGS 3
#define KEY_COUNT 11
#define ALT_TABLE_WORDS 6
#define BLEND_WORD 3
#define BLEND_FULL 0x400
#define BLEND_SHIFT 10
#define FIXED_SHIFT 8
#define ENTRY_WORDS_PER_IDX 3
#define OBJ_CTX_OFFSET 0x10
#define CTX_BASE_OFFSET 0x184
#define ENTRY_PRIMARY_OFFSET 0x10
#define ENTRY_FALLBACK_OFFSET 0x14

void func_80023B04(Arg80023B04 *arg0, s32 *arg1, s32 *arg2) {
    s32 sp28[ENT_ROW_BUFFER_WORDS];
    s32 key;
    s32 *tbl;
    s32 idx;
    s16 *p;
    s32 x, y, w, x1, y1;
    u8 *obj;
    s32 arg;
    s32 unk8;

    idx = 0;
    if (arg0->unk4 == g_CameraPath[0]) {
        key = g_CameraPathKind;
        arg = D_800608DC;
    } else {
        key = Ent_GetPropertyValue(arg0->unk4, KEY_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
        idx = Ent_GetPropertyValue(arg0->unk4, IDX_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
        obj = (u8 *)arg0->unk0;
        arg = *(s32 *)(obj + (idx * ENTRY_WORDS_PER_IDX + D_800608FC + *(s32 *)(*(s32 *)(obj + OBJ_CTX_OFFSET) + CTX_BASE_OFFSET)) * 4 + ENTRY_PRIMARY_OFFSET);
    }
    tbl = D_8005BA28;
    unk8 = arg0->unk8;
    if (key == 0) {
        tbl = D_8005BA28 + ALT_TABLE_WORDS;
    }
    p = (s16 *)Ent_GetProperty(arg, LOOKUP_ID, unk8, 0, 0, LOOKUP_FLAGS, sp28, 0, 0);
    if (p == 0) {
        if (arg0->unk4 == g_CameraPath[0]) {
            arg = D_800608D8;
        } else {
            obj = (u8 *)arg0->unk0;
            arg = *(s32 *)(obj + (idx * ENTRY_WORDS_PER_IDX + *(s32 *)(*(s32 *)(obj + OBJ_CTX_OFFSET) + CTX_BASE_OFFSET)) * 4 + ENTRY_FALLBACK_OFFSET);
        }
        p = (s16 *)Ent_GetProperty(arg, LOOKUP_ID, unk8, 0, 0, LOOKUP_FLAGS, sp28, 0, 0);
    }
    switch (key) {
    case 0:
    case 3:
    case 4:
    case 10:
        w = tbl[BLEND_WORD];
        x = p[0] << FIXED_SHIFT;
        y = p[1] << FIXED_SHIFT;
        if (w != 0) {
            x1 = p[2] << FIXED_SHIFT;
            y1 = p[3] << FIXED_SHIFT;
            if (w == BLEND_FULL) {
                x = x1;
                y = y1;
            } else {
                x += ((x1 - x) * w) >> BLEND_SHIFT;
                y += ((y1 - y) * w) >> BLEND_SHIFT;
            }
        }
        break;
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
        x = p[0] << FIXED_SHIFT;
        y = p[1] << FIXED_SHIFT;
        break;
    }
    *arg1 = x;
    *arg2 = y;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023B04);
#endif

#ifdef NON_MATCHING
#define UNK_ARG1 0x130
#define UNK_ARG6 0x120

extern s32 g_CameraPath[];
extern s32 g_CameraProgress;

/* The camera's projection distance at its path's progress: the retail code returns Ent_GetPropertyValue's value, which
 * its callers read */
s32 func_80023D7C(void) {
    return Ent_GetPropertyValue(*g_CameraPath, UNK_ARG1, g_CameraProgress, 0, 0, 3, UNK_ARG6);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023D7C);
#endif

#ifdef NON_MATCHING
extern s32 D_8005BA4C;
extern s32 D_800608DC;
extern s32 g_CameraProgress;

s32 func_80023DC4(void) {
    s32 sp28[ENT_ROW_BUFFER_WORDS];
    s32 v;
    s32 a0;
    s32 a1;

    v = Ent_GetProperty(D_800608DC, 0x16E, g_CameraProgress, 0, 0, ENT_ROW_INTERPOLATED, sp28, 0, 0);
    if (v == 0) {
        return 0;
    }
    a0 = *(s16 *)v;
    if (D_8005BA4C != 0) {
        a1 = *(s16 *)(v + 2);
        if (D_8005BA4C == 0x400) {
            a0 = a1;
        } else {
            a0 = func_800312CC(a0, a1, D_8005BA4C >> 2);
        }
    }
    return a0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023DC4);
#endif

#ifdef NON_MATCHING
#define SEGMENT_LENGTH_QUERY 0x4B
#define SPEED_QUERY 0xC9
#define POSITION_SHIFT 8
#define DIR_FORWARD 1
#define DIR_BACKWARD 2
#define SPEED_MIN_DISTANCE 9
#define SPEED_MAX_DISTANCE 0x7D000
#define SPEED_DIVISOR 512000
#define SPEED_NUMERATOR 300
#define SPEED_BASE 100
#define SPEED_FRAC_SHIFT 16

typedef struct Unk80023E60 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
} Unk80023E60;

void func_80023E60(s32 *path, s32 arg1, s32 *out, s32 arg3) {
    Unk80023E60 *arg0 = (Unk80023E60 *)path;
    Unk80023E60 *arg2 = (Unk80023E60 *)out;
    Unk80023E60 *last;
    Unk80023E60 *p;
    s32 dist;
    s32 a;
    s32 i;
    s32 speed;
    s32 end;
    s32 pos;

    last = &arg0[arg1 - 1];
    if (arg0->unk4 == last->unk4) {
        dist = last->unk8 - arg0->unk8;
        if (dist < 0) {
            dist = -dist;
        }
    } else {
        a = arg0->unk8;
        if (arg0->unkC == DIR_BACKWARD) {
            a = ((Ent_GetPropertyRowLength(arg0->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - a;
        }
        dist = a;
        a = last->unk8;
        if (last->unkC == DIR_FORWARD) {
            a = ((Ent_GetPropertyRowLength(last->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - a;
        }
        dist += a;
        i = arg1 - 2;
        if (i > 0) {
            p = arg0 + 1;
            do {
                dist = dist + ((Ent_GetPropertyRowLength(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - 2;
                p++;
                i--;
            } while (i != 0);
        }
    }
    if (arg3 < 0) {
        speed = Ent_GetPropertyValue(arg0->unk4, SPEED_QUERY, 0, 0, 0, 2, 0);
        end = (dist * speed) >> POSITION_SHIFT;
        a = end;
        if (end < SPEED_MIN_DISTANCE) {
            *arg2 = *last;
            return;
        }
        if (a > SPEED_MAX_DISTANCE) {
            a = SPEED_MAX_DISTANCE;
        }
        a = (((a * SPEED_NUMERATOR) / SPEED_DIVISOR + SPEED_BASE) << SPEED_FRAC_SHIFT) / speed;
        p = arg0;
        if (a < dist) {
            dist = a;
        }
    } else {
        dist = (dist * arg3) >> POSITION_SHIFT;
        p = arg0;
    }
    if (p->unkC == DIR_BACKWARD) {
        end = (Ent_GetPropertyRowLength(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT;
    } else {
        end = 0;
    }
    pos = p->unk8;
    while (1) {
        if (p->unkC == DIR_BACKWARD) {
            a = end - pos;
            if (a >= dist) {
                pos += dist;
                break;
            }
            dist -= a;
        } else {
            if (pos >= dist) {
                pos -= dist;
                break;
            }
            dist -= pos;
        }
        p++;
        a = (Ent_GetPropertyRowLength(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT;
        if (p->unkC == DIR_BACKWARD) {
            end = a;
            pos = 1;
        } else {
            end = 0;
            pos = a - 1;
        }
    }
    arg2->unk0 = p->unk0;
    arg2->unk8 = pos;
    arg2->unk4 = p->unk4;
    arg2->unkC = p->unkC;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023E60);
#endif

s32 Cam_OpenLevel(void) {
    Cam_Reset();
    return -0xFF;
}

extern s8 g_CameraMode;
extern s8 D_8005B99A;
typedef struct Unk8005B9E0 {
    s32 count;
    u8 pad[0x74];
    s32 e[8][4];
} Unk8005B9E0;

extern s32 D_8005B9AC;
extern s32 D_8005B9BC;
extern s32 D_8005B9C0;
extern s32 D_8005B9C4;
extern Unk8005B9E0 D_8005B9E0;
extern s32 D_8005B9E4;
extern s32 D_8005B9F4;
extern s32 D_8005B9F8;
extern s32 D_8005B9FC;
extern s32 D_8005BA08;
extern s32 D_8005BA0C;
extern s32 D_8005BA10;
extern s32 g_CameraPathKind;
extern s32 D_800608F8;
extern s32 D_80060918;
extern s32 D_8006CD98;

void Cam_Reset(void) {
    g_CameraMode = 5;
    D_80060918 = 0x1001;
    D_8005B9AC = 0x1001;
    g_CameraPathKind = 5;
    D_800608F8 = 0;
    D_8005B9E0.count = 0;
    D_8005B9E4 = 0;
    D_8005B9C4 = 0;
    D_8005B99A = 0;
    D_8005B9BC = 0;
    D_8005B9C0 = 0;
    D_8005B9FC = 0;
    D_8005B9F8 = 0;
    D_8005B9F4 = 0;
    D_8005BA10 = 0;
    D_8005BA0C = 0;
    D_8005BA08 = 0;
    D_8006CD98 = 0x100;
    func_80022224(1);
}

s32 func_80024268(s32 *p, s32 d, s32 *out) {
    s32 v;
    s32 lim;
    s32 over;
    s32 k;
    k = p[3];
    do {
        v = p[2];
        if (k == 2) {
            v += d;
            lim = (Ent_GetPropertyRowLength(p[1], 0x4B, 0, 2) - 1) << 8;
            over = v - lim;
            if (over > 0) {
                *out = lim;
                return over;
            }
        } else {
            v -= d;
            if (v < 0) {
                *out = 0;
                return -v;
            }
        }
    } while (0);
    *out = v;
    return 0;
}

s32 func_800242F8(s32 arg0) {
    switch (arg0) {
    case 0:
        return 4;
    case 3:
        return 1;
    case 8:
        return 2;
    default:
        return 0;
    }
}

#ifdef NON_MATCHING
#define KEY_QUERY 0x29
#define IDX_QUERY 0x173
#define KEY_ENTRY 0x4B
#define QUERY_FLAGS 2
#define MODE_ROTATE_ADD 1
#define MODE_NO_ROTATE 2
#define MODE_FROZEN 3
#define FIXED_SHIFT 8
#define TRIG_SHIFT 4
#define ENTRY_STRIDE_WORDS 3


typedef struct {
    s32 x, y, z;
} Unk8002433CVec;
typedef struct {
    s32 pad0[5];
    Unk8002433CVec *pos;
} Unk8002433CInfo;
typedef struct {
    s32 pad0[1];
    Unk8002433CInfo *info;
} Unk8002433CObj;
typedef struct {
    s32 pad[0x61];
    s32 base;
} Unk8002433CTab;
typedef struct {
    s32 pad[4];
    Unk8002433CTab *tab;
} Unk8002433CRoot;
typedef struct {
    s32 pad[2];
    s32 mode;
} Unk8002433CMode;

extern s32 g_CameraX;
extern s32 g_CameraY;
extern s32 g_CameraZ;
extern s32 D_80060828;
extern Unk8002433CMode *g_LevelHeader;
void func_80023B04(Arg80023B04 *arg0, s32 *arg1, s32 *arg2);

s32 func_8002433C(s32 *arg0, s32 *arg1, s32 *a2, s32 *a3, s32 *a4) {
    Arg80023B04 *a0 = (Arg80023B04 *)arg0;
    Unk8002433CVec *a1 = (Unk8002433CVec *)arg1;
    s32 dx, dy, dz, lo, hi;
    s32 obj, ang, mode, ang2, s, c, r;
    s16 *e;
    Unk8002433CVec *o;

    func_80023B04(a0, &lo, &hi);
    if (a2 != 0) {
        *a2 = lo;
    }
    if (a3 != 0) {
        *a3 = hi;
    }
    obj = a0->unk4;
    ang = a0->unk8;
    mode = func_800242F8(Ent_GetPropertyValue(obj, KEY_QUERY, 0, 0, 0, QUERY_FLAGS, 0));
    if (obj == g_CameraPath[0]) {
        dx = a1->x - g_CameraX;
        dy = a1->y - g_CameraY;
        ang2 = D_80060828;
        dz = a1->z - g_CameraZ;
    } else {
        ang >>= FIXED_SHIFT;
        e = (s16 *)Ent_GetProperty(obj, KEY_ENTRY, 0, ang, 0, QUERY_FLAGS, 0, 0, 0);
        o = ((Unk8002433CObj *)obj)->info->pos;
        dx = a1->x - ((o->x + e[0]) << FIXED_SHIFT);
        dy = a1->y - ((o->y + e[1]) << FIXED_SHIFT);
        dz = a1->z - ((o->z + e[2]) << FIXED_SHIFT);
        if (mode != MODE_NO_ROTATE) {
            Unk8002433CRoot *rt = (Unk8002433CRoot *)a0->unk0;
            s32 i = Ent_GetPropertyValue(obj, IDX_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
            e = (s16 *)Ent_GetProperty(*(s32 *)((s32)rt + (i * ENTRY_STRIDE_WORDS + rt->tab->base) * 4 + 0x14), KEY_ENTRY, 0, ang * 2, 0, QUERY_FLAGS, 0, 0, 0);
            ang2 = e[1];
        } else {
            ang2 = 0;
        }
    }
    if (g_LevelHeader->mode == MODE_FROZEN) {
        ang2 = 0;
    }
    if (mode == MODE_NO_ROTATE) {
        r = -dy;
    } else {
        ang2 = -ang2;
        if (a4 != 0) {
            *a4 = ang2;
        }
        s = Math_Cos(ang2);
        c = Math_Sin(ang2);
        if (mode == MODE_ROTATE_ADD) {
            r = ((dz >> FIXED_SHIFT) * c >> TRIG_SHIFT) + ((dx >> FIXED_SHIFT) * s >> TRIG_SHIFT);
        } else {
            r = ((dx >> FIXED_SHIFT) * c >> TRIG_SHIFT) - ((dz >> FIXED_SHIFT) * s >> TRIG_SHIFT);
        }
    }
    if (hi < r) {
        return 1;
    }
    return -(r < lo);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_8002433C);
#endif

#ifdef NON_MATCHING
#define ENTRY_WORDS 4
#define FIRST_SLOT 1
#define SECOND_SLOT 2
#define QUERY_KIND 0x29
#define LIST_WORDS 28
#define SKIP_TYPE 2

/* The candidate list func_800267C4 fills */
typedef struct {
    s32 list[LIST_WORDS];
} Unk80024658;

s32 func_800267C4(Unk80024658 *, s32);

s32 func_80024658(s32 *arg0, s32 *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    /* The candidate list, the result words and the saved arguments share one stack area; a long list runs into them. */
    struct {
        Unk80024658 list;
        s32 out;
        s32 altA;
        s32 altB;
        s32 altC;
        s32 a0;
        s32 pad0;
        s32 a2;
        s32 pad1;
        s32 a3;
        s32 pad2;
        s32 type;
        s32 pad3;
        s32 slot;
    } f;
    s32 *entry;
    s32 *cand;
    s32 kind;
    s32 score;
    s32 other;
    s32 count;
    s32 hi;
    s32 lo;

    f.a0 = (s32)arg0;
    f.a2 = arg2;
    f.a3 = arg3;
    f.slot = arg5;
    entry = arg0 + (f.slot - FIRST_SLOT) * ENTRY_WORDS;
    kind = func_800242F8(Ent_GetPropertyValue(entry[1], QUERY_KIND, 0, 0, 0, SKIP_TYPE, 0));
    f.type = kind;
    /* For SKIP_TYPE, hi and lo stay what the caller left in the registers */
    if (f.type != SKIP_TYPE) {
        hi = Math_Cos(arg4);
        lo = Math_Sin(arg4);
    }
    if (f.slot == FIRST_SLOT) {
        if (f.a3 != 0) {
            score = Cam_PushBackBothDirs(entry, (s32)arg1, (s32 *)f.a2, hi, lo, f.type, &f.out);
        } else {
            score = Cam_PullForwardBothDirs(entry, (s32)arg1, (s32 *)f.a2, hi, (void *)lo, f.type, &f.out);
        }
    } else if (f.a3 != 0) {
        score = func_800254D4(entry, (s32)arg1, (s32 *)f.a2, hi, lo, f.type, &f.out);
    } else {
        score = func_80024B6C(entry, (s32)arg1, (s32 *)f.a2, hi, (void *)lo, f.type, &f.out);
    }
    if (arg5 == FIRST_SLOT) {
        f.slot = SECOND_SLOT;
        entry += ENTRY_WORDS;
        arg0[4] = arg0[0];
        arg0[5] = arg0[1];
        arg0[7] = arg0[3];
    }
    entry[2] = f.out;
    if (score == 0) {
        return f.slot;
    } else if (arg5 != FIRST_SLOT) {
        return f.slot;
    } else {
        count = func_800267C4(&f.list, (s32)arg0);
        cand = f.list.list;
        /* The count is unsigned: any non-zero value runs that many entries */
        for (; count-- != 0; cand += ENTRY_WORDS) {
            kind = func_800242F8(Ent_GetPropertyValue(cand[1], QUERY_KIND, 0, 0, 0, SKIP_TYPE, 0));
            if (f.type == kind) {
                if (f.a3 != 0) {
                    other = func_800254D4(cand, (s32)arg1, (s32 *)f.a2, hi, lo, kind, &f.out);
                } else {
                    other = func_80024B6C(cand, (s32)arg1, (s32 *)f.a2, hi, (void *)lo, kind, &f.out);
                }
            } else if (kind == 0) {
                continue;
            } else {
                f.a3 = func_8002433C(cand, arg1, &f.altA, &f.altB, &f.altC);
                if (f.a3 == 0) {
                    arg0[4] = cand[0];
                    arg0[5] = cand[1];
                    arg0[6] = cand[2];
                    arg0[7] = cand[3];
                    return f.slot;
                }
                if (kind != SKIP_TYPE) {
                    hi = Math_Cos(f.altC);
                    lo = Math_Sin(f.altC);
                }
                if (f.a3 < 0) {
                    other = func_800254D4(cand, (s32)arg1, (s32 *)f.altB, hi, lo, kind, &f.out);
                } else {
                    other = func_80024B6C(cand, (s32)arg1, (s32 *)f.altA, hi, (void *)lo, kind, &f.out);
                }
            }
            if (score < 0 || (other >= 0 && other < score)) {
                cand[2] = f.out;
                score = other;
                arg0[4] = cand[0];
                arg0[5] = cand[1];
                arg0[6] = cand[2];
                arg0[7] = cand[3];
            }
        }
    }
    return f.slot;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024658);
#endif

#ifdef NON_MATCHING
extern char D_80010380[];

s32 Cam_PullForwardBothDirs(void *arg0, s32 arg1, s32 *arg2, s32 arg3, void *arg4, s32 arg5, s32 *arg6) {
    s32 sp20;
    s32 sp24;
    s32 temp_v0;
    s32 var_s0;
    s32 var_s2;

    var_s2 = 2;
    *(s32 *)((char *)arg0 + 0xC) = 2;
    var_s0 = func_80024B6C(arg0, arg1, arg2, arg3, arg4, arg5, &sp20);
    if (var_s0 != 0) {
        *(s32 *)((char *)arg0 + 0xC) = 1;
        temp_v0 = func_80024B6C(arg0, arg1, arg2, arg3, arg4, arg5, &sp24);
        if (temp_v0 >= 0) {
            if ((var_s0 < 0) || (temp_v0 < var_s0)) {
                var_s0 = temp_v0;
                var_s2 = 1;
                sp20 = sp24;
            }
        } else if (var_s0 < 0) {
            printf(D_80010380);
        }
    }
    /* In this order: arg6 may point at arg0's field */
    *(s32 *)((char *)arg0 + 0xC) = var_s2;
    *arg6 = sp20;
    return var_s0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", Cam_PullForwardBothDirs);
#endif

#ifdef NON_MATCHING
typedef struct Unk80024B6CPoint {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
} Unk80024B6CPoint;

typedef struct Unk80024B6CBase {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
} Unk80024B6CBase;

typedef struct Unk80024B6CPath {
    /* 0x0 */ s32 unk_0;
    /* 0x4 */ s32 unk_4;
    /* 0x8 */ s32 pos;
    /* 0xC */ s32 mode;
} Unk80024B6CPath;

#define UNK_80024B6C_PLANE_MASK 5
#define UNK_80024B6C_PLANE_FIRST 1
#define UNK_80024B6C_ENTRY_KIND 0x4B
#define UNK_80024B6C_ENTRY_SIZE 2
#define UNK_80024B6C_MODE_FORWARD 2
#define UNK_80024B6C_MAX_WORSE 3
#define UNK_80024B6C_BASE_SLOT 5

/* Distance of a point against the query: plane projection, or height when no plane flag is set. */
static inline s32 func_80024B6C_dist(s32 flags, s32 x, s32 y, s32 z, s32 qx, s32 qy, s32 qz, s32 c, s32 s) {
    s32 dx;
    s32 dz;

    if (flags & UNK_80024B6C_PLANE_MASK) {
        dx = qx - x;
        dz = qz - z;
        if (flags == UNK_80024B6C_PLANE_FIRST) {
            return ((dz * c) >> 4) + ((dx * s) >> 4);
        } else {
            return ((dx * c) >> 4) - ((dz * s) >> 4);
        }
    } else {
        return (y - qy) << 8;
    }
}

/* Same, reading only the point fields the flag needs. */
static inline s32 func_80024B6C_dist_at(s32 flags, Unk80024B6CPoint *p, Unk80024B6CBase *base, s32 qx, s32 qy, s32 qz,
                                        s32 c, s32 s) {
    if (flags & UNK_80024B6C_PLANE_MASK) {
        return func_80024B6C_dist(flags, p->x + base->x, 0, p->z + base->z, qx, qy, qz, c, s);
    } else {
        return func_80024B6C_dist(flags, 0, p->y + base->y, 0, qx, qy, qz, c, s);
    }
}

s32 func_80024B6C(void *arg0, s32 arg1, s32 *arg2, s32 arg3, void *arg4, s32 arg5, s32 *arg6) {
    Unk80024B6CPath *path;
    Unk80024B6CBase *base;
    Unk80024B6CPoint *p0;
    Unk80024B6CPoint *p1;
    Unk80024B6CPoint *pn;
    s32 *q;
    s32 obj;
    s32 qx, qy, qz;
    s32 last;
    s32 pos;
    s32 frac;
    s32 idx;
    s32 next;
    s32 x0, y0, z0, x1, y1, z1;
    s32 target;
    s32 here;
    s32 dnext;
    s32 dcur;
    s32 dprev;
    s32 dnew;
    s32 best;
    s32 fp;
    s32 worse;
    s32 c;
    s32 s;
    s32 cur;

    path = (Unk80024B6CPath *)arg0;
    s = arg3;
    c = (s32)arg4;
    target = (s32)arg2;
    obj = path->unk_4;
    pos = path->pos;
    q = (s32 *)arg1;
    qx = q[0] >> 8;
    qy = q[1] >> 8;
    qz = q[2] >> 8;
    idx = pos >> 8;
    base = (Unk80024B6CBase *)((s32 **)((s32 **)obj)[1])[UNK_80024B6C_BASE_SLOT];
    last = Ent_GetPropertyRowLength(obj, UNK_80024B6C_ENTRY_KIND, 0, UNK_80024B6C_ENTRY_SIZE) - 1;
    p0 = (Unk80024B6CPoint *)Ent_GetProperty(obj, UNK_80024B6C_ENTRY_KIND, 0, idx, 0, UNK_80024B6C_ENTRY_SIZE, 0, 0, 0);
    next = idx + 1;
    if (idx < last) {
        p1 = p0 + 1;
    } else {
        next = idx;
        p1 = p0;
    }
    frac = pos & 0xFF;

    /* Only the fields the flag needs are read: x and z for a plane, y otherwise. */
    x0 = y0 = z0 = x1 = y1 = z1 = 0;
    if (arg5 & UNK_80024B6C_PLANE_MASK) {
        x0 = p0->x + base->x;
        z0 = p0->z + base->z;
        x1 = p1->x + base->x;
        z1 = p1->z + base->z;
    } else {
        y0 = p0->y + base->y;
        y1 = p1->y + base->y;
    }

    here = func_80024B6C_dist(arg5, x0 + (((x1 - x0) * frac) >> 8), y0 + (((y1 - y0) * frac) >> 8),
                              z0 + (((z1 - z0) * frac) >> 8), qx, qy, qz, c, s);
    if (target >= here) {
        *arg6 = pos;
        return target - here;
    } else {
        /* target not reached at the current position */
    }
    best = here - target;
    fp = pos;

    if (path->mode == UNK_80024B6C_MODE_FORWARD) {
        dnext = func_80024B6C_dist(arg5, x1, y1, z1, qx, qy, qz, c, s);
        if (target >= dnext) {
            *arg6 = pos + ((((next << 8) - pos) * (((here - target) << 8) / (here - dnext))) >> 8);
            return 0;
        } else {
            /* target lies further on */
        }
        worse = 0;
        if (dnext - target < best) {
            best = dnext - target;
            fp = next << 8;
        } else {
            /* not closer */
        }
        while (1) {
            cur = next;
            if (next == last) {
                *arg6 = fp;
                return best;
            } else {
                /* more points */
            }
            next = cur + 1;
            pn = (Unk80024B6CPoint *)Ent_GetProperty(obj, UNK_80024B6C_ENTRY_KIND, 0, (next << 8) >> 8, 0,
                                                   UNK_80024B6C_ENTRY_SIZE, 0, 0, 0);
            dprev = dnext;
            dnext = func_80024B6C_dist_at(arg5, pn, base, qx, qy, qz, c, s);
            dnew = dnext - target;
            if (target >= dnext) {
                *arg6 = (cur << 8) + (((dprev - target) << 8) / (dprev - dnext));
                return 0;
            } else {
                /* still short */
            }
            worse += 1;
            if (dnew < best) {
                best = dnew;
                fp = next << 8;
                worse = 0;
            } else if (worse >= UNK_80024B6C_MAX_WORSE) {
                *arg6 = fp;
                return -1;
            } else {
                /* keep scanning */
            }
        }
    } else {
        dcur = func_80024B6C_dist(arg5, x0, y0, z0, qx, qy, qz, c, s);
        if (target >= dcur) {
            *arg6 = pos - (((pos - (idx << 8)) * (((here - target) << 8) / (here - dcur))) >> 8);
            return 0;
        } else {
            /* target lies further back */
        }
        worse = 0;
        if (dcur - target < best) {
            best = dcur - target;
            fp = idx << 8;
        } else {
            /* not closer */
        }
        while (1) {
            cur = idx;
            if (idx == 0) {
                *arg6 = fp;
                return best;
            } else {
                /* more points */
            }
            idx -= 1;
            pn = (Unk80024B6CPoint *)Ent_GetProperty(obj, UNK_80024B6C_ENTRY_KIND, 0, (idx << 8) >> 8, 0,
                                                   UNK_80024B6C_ENTRY_SIZE, 0, 0, 0);
            dprev = dcur;
            dcur = func_80024B6C_dist_at(arg5, pn, base, qx, qy, qz, c, s);
            dnew = dcur - target;
            if (target >= dcur) {
                *arg6 = (cur << 8) - (((dprev - target) << 8) / (dprev - dcur));
                return 0;
            } else {
                /* still short */
            }
            worse += 1;
            if (dnew < best) {
                best = dnew;
                fp = idx << 8;
                worse = 0;
            } else if (worse >= UNK_80024B6C_MAX_WORSE) {
                *arg6 = fp;
                return -1;
            } else {
                /* keep scanning */
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024B6C);
#endif

#ifdef NON_MATCHING
extern char D_800103C0[];

s32 Cam_PushBackBothDirs(void *arg0, s32 arg1, s32 *arg2, s32 arg3, s32 arg4, s32 arg5, s32 *arg6) {
    s32 sp20;
    s32 sp24;
    s32 temp_v0;
    s32 var_s0;
    s32 var_s2;

    var_s2 = 2;
    ((s32 *)arg0)[3] = 2;
    var_s0 = func_800254D4(arg0, arg1, arg2, arg3, arg4, arg5, &sp20);
    if (var_s0 != 0) {
        ((s32 *)arg0)[3] = 1;
        temp_v0 = func_800254D4(arg0, arg1, arg2, arg3, arg4, arg5, &sp24);
        if (temp_v0 >= 0) {
            if ((var_s0 < 0) || (temp_v0 < var_s0)) {
                var_s0 = temp_v0;
                var_s2 = 1;
                sp20 = sp24;
            }
        } else if (var_s0 < 0) {
            printf(D_800103C0);
        }
    }
    ((s32 *)arg0)[3] = var_s2;
    *arg6 = sp20;
    return var_s0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", Cam_PushBackBothDirs);
#endif

#ifdef NON_MATCHING
#define FIXED_SHIFT 8
#define FRAC_MASK 0xFF
#define PLANE_FLAGS 5
#define SCALE_SHIFT 4
#define PATH_ITEM 0x4B
#define PATH_MODE 2
#define POINT_STRIDE 6
#define MAX_WORSE_STEPS 3
#define DIR_FORWARD 2
#define ROTATE_FLAG 1

/* Signed distance of an offset from the query along the projected axis. */
#define func_800254D4_proj(dx, dz, c, s, mode) \
    ((mode) != ROTATE_FLAG ? (((dx) * (c)) >> SCALE_SHIFT) - (((dz) * (s)) >> SCALE_SHIFT) \
                           : (((dz) * (c)) >> SCALE_SHIFT) + (((dx) * (s)) >> SCALE_SHIFT))

/* Point `i` of the path of `owner`. */
#define PATH_POINT(owner, i) ((s16 *)Ent_GetProperty((owner), PATH_ITEM, 0, (i), 0, PATH_MODE, 0, 0, 0))

s32 func_800254D4(void *arg0, s32 arg1, s32 *arg2, s32 arg3, s32 arg4, s32 arg5, s32 *arg6) {
    s32 target = (s32)arg2;
    s32 owner = *(s32 *)((u8 *)arg0 + 4);
    s32 pos = *(s32 *)((u8 *)arg0 + 8);
    s32 *origin;
    s32 qx, qy, qz;
    s32 last;
    s32 idx, next;
    s32 frac;
    s16 *p, *q;
    s32 ax, ay, az, bx, by, bz;
    s32 cur, best, bestPos, worse, prev, val, t;
    s32 planar;

    qx = ((s32 *)arg1)[0] >> FIXED_SHIFT;
    qy = ((s32 *)arg1)[1] >> FIXED_SHIFT;
    qz = ((s32 *)arg1)[2] >> FIXED_SHIFT;
    origin = *(s32 **)(*(s32 *)(owner + 4) + 0x14);
    idx = pos >> FIXED_SHIFT;
    last = Ent_GetPropertyRowLength(owner, PATH_ITEM, 0, PATH_MODE) - 1;
    p = PATH_POINT(owner, idx);
    if (idx < last) {
        next = idx + 1;
        q = (s16 *)((u8 *)p + POINT_STRIDE);
    } else {
        next = idx;
        q = p;
    }
    frac = pos & FRAC_MASK;
    planar = (arg5 & PLANE_FLAGS) != 0;

    /* Only the axes in use are read; the others may point at unmapped memory. */
    if (planar) {
        ax = p[0] + origin[0];
        az = p[2] + origin[2];
        bx = q[0] + origin[0];
        bz = q[2] + origin[2];
        cur = func_800254D4_proj(qx - (ax + (((bx - ax) * frac) >> FIXED_SHIFT)),
                                 qz - (az + (((bz - az) * frac) >> FIXED_SHIFT)), arg4, arg3, arg5);
    } else {
        ay = p[1] + origin[1];
        by = q[1] + origin[1];
        cur = ((ay + (((by - ay) * frac) >> FIXED_SHIFT)) - qy) << FIXED_SHIFT;
    }
    best = target - cur;
    if (cur >= target) {
        *arg6 = pos;
        return cur - target;
    } else {
        /* the query lies beyond the current position */
    }
    bestPos = pos;

    if (*(s32 *)((u8 *)arg0 + 0xC) == DIR_FORWARD) {
        if (planar) {
            val = func_800254D4_proj(qx - bx, qz - bz, arg4, arg3, arg5);
        } else {
            val = (by - qy) << FIXED_SHIFT;
        }
        if (val >= target) {
            *arg6 = pos + ((((next << FIXED_SHIFT) - pos) * (((target - cur) << FIXED_SHIFT) / (val - cur))) >> FIXED_SHIFT);
            return 0;
        } else {
            /* the target is past the next point */
        }
        worse = 0;
        if (target - val < best) {
            best = target - val;
            bestPos = next << FIXED_SHIFT;
            worse = 0;
        } else {
            /* not closer */
        }
        for (;;) {
            if (next == last) {
                *arg6 = bestPos;
                return best;
            } else {
                /* more points */
            }
            t = next;
            next = next + 1;
            q = PATH_POINT(owner, (next << FIXED_SHIFT) >> FIXED_SHIFT);
            prev = val;
            if (planar) {
                val = func_800254D4_proj(qx - (q[0] + origin[0]), qz - (q[2] + origin[2]), arg4, arg3, arg5);
            } else {
                val = ((q[1] + origin[1]) - qy) << FIXED_SHIFT;
            }
            if (val >= target) {
                *arg6 = (t << FIXED_SHIFT) + (((target - prev) << FIXED_SHIFT) / (val - prev));
                return 0;
            } else {
                /* still short of the target */
            }
            worse++;
            if (best >= target - val) {
                best = target - val;
                bestPos = next << FIXED_SHIFT;
                worse = 0;
            } else if (worse >= MAX_WORSE_STEPS) {
                *arg6 = bestPos;
                return -1;
            } else {
                /* keep walking */
            }
        }
    } else {
        if (planar) {
            val = func_800254D4_proj(qx - ax, qz - az, arg4, arg3, arg5);
        } else {
            val = (ay - qy) << FIXED_SHIFT;
        }
        if (val >= target) {
            *arg6 = pos - (((pos - (idx << FIXED_SHIFT)) * (((target - cur) << FIXED_SHIFT) / (val - cur))) >> FIXED_SHIFT);
            return 0;
        } else {
            /* the target is before the point */
        }
        worse = 0;
        if (target - val < best) {
            best = target - val;
            bestPos = idx << FIXED_SHIFT;
            worse = 0;
        } else {
            /* not closer */
        }
        for (;;) {
            t = idx;
            if (idx == 0) {
                *arg6 = bestPos;
                return best;
            } else {
                /* more points */
            }
            idx = idx - 1;
            p = PATH_POINT(owner, (idx << FIXED_SHIFT) >> FIXED_SHIFT);
            prev = val;
            if (planar) {
                val = func_800254D4_proj(qx - (p[0] + origin[0]), qz - (p[2] + origin[2]), arg4, arg3, arg5);
            } else {
                val = ((p[1] + origin[1]) - qy) << FIXED_SHIFT;
            }
            if (val >= target) {
                *arg6 = (t << FIXED_SHIFT) - (((target - prev) << FIXED_SHIFT) / (val - prev));
                return 0;
            } else {
                /* still short of the target */
            }
            worse++;
            if (target - val < best) {
                best = target - val;
                bestPos = idx << FIXED_SHIFT;
                worse = 0;
            } else if (worse >= MAX_WORSE_STEPS) {
                *arg6 = bestPos;
                return -1;
            } else {
                /* keep walking */
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800254D4);
#endif

#ifdef NON_MATCHING
extern s32 g_CameraX;
extern s32 g_CameraY;
extern s32 g_CameraZ;
extern s32 D_80060828;

#define ENTRY_TYPE_Y_RANGE 0x1E3
#define ENTRY_TYPE_X_RANGE 0x1E2
#define ENTRY_TYPE_Z_RANGE 0x1E4
#define FIXED_SHIFT 8
#define TRIG_SHIFT 4

typedef struct Unk80025D18Range {
    /* 0x0 */ s16 min;
    /* 0x2 */ s16 max;
    /* 0x4 */ s16 value;
} Unk80025D18Range;

typedef struct Unk80025D18Pos {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
} Unk80025D18Pos;

void func_80025D18(Unk80025D18Pos *arg0) {
    s32 sp28;
    Unk80025D18Range *yRange;
    Unk80025D18Range *xRange;
    Unk80025D18Range *zRange;
    s32 xMin;
    s32 xMax;
    s32 zMin;
    s32 zMax;
    s32 dy;
    s32 dx;
    s32 dz;
    s32 sin;
    s32 cos;
    s32 negAngle;
    s32 rotX;
    s32 rotZ;

    yRange = (Unk80025D18Range *)Ent_GetProperty(D_800608D8, ENTRY_TYPE_Y_RANGE, g_CameraProgress, 0, 0, 0, 0, 0, 0);
    if (yRange != 0) {
        xMin = yRange->min;
        xMax = yRange->max;
        if (xMin <= xMax) {
            dy = arg0->y - g_CameraY;
            if (dy < (xMin << FIXED_SHIFT) || (xMax << FIXED_SHIFT) < dy) {
                sp28 = yRange->value << FIXED_SHIFT;
                Cam_QueueMessage(0, &sp28);
            }
        }
    }
    xRange = (Unk80025D18Range *)Ent_GetProperty(D_800608D8, ENTRY_TYPE_X_RANGE, g_CameraProgress, 0, 0, 0, 0, 0, 0);
    if (xRange != 0) {
        xMin = xRange->min;
        xMax = xRange->max;
        if (xMax < xMin) {
            xRange = 0;
        }
    }
    zRange = (Unk80025D18Range *)Ent_GetProperty(D_800608D8, ENTRY_TYPE_Z_RANGE, g_CameraProgress, 0, 0, 0, 0, 0, 0);
    if (zRange != 0) {
        zMin = zRange->min;
        zMax = zRange->max;
        if (zMax < zMin) {
            zRange = 0;
        }
    }
    if (xRange != 0 || zRange != 0) {
        dx = arg0->x - g_CameraX;
        dz = arg0->z - g_CameraZ;
        negAngle = -D_80060828;
        sin = Math_Cos(negAngle);
        cos = Math_Sin(negAngle);
        rotX = (((dz >> FIXED_SHIFT) * cos) >> TRIG_SHIFT) + (((dx >> FIXED_SHIFT) * sin) >> TRIG_SHIFT);
        rotZ = (((dx >> FIXED_SHIFT) * cos) >> TRIG_SHIFT) - (((dz >> FIXED_SHIFT) * sin) >> TRIG_SHIFT);
        if (xRange != 0 && (rotX < (xMin << FIXED_SHIFT) || (xMax << FIXED_SHIFT) < rotX)) {
            sp28 = xRange->value << FIXED_SHIFT;
            Cam_QueueMessage(0, &sp28);
        }
        if (zRange != 0 && (rotZ < (zMin << FIXED_SHIFT) || (zMax << FIXED_SHIFT) < rotZ)) {
            sp28 = zRange->value << FIXED_SHIFT;
            Cam_QueueMessage(0, &sp28);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80025D18);
#endif

void Cam_QueueMessage(s32 arg0, s32 *arg1) {
    s32 *dst;
    s32 n;
    s32 *e;

    if (D_8005B9E0.count < 8) {
        switch (arg0) {
        case 0x500:
        case 0x600:
            n = 3;
            break;
        case 0x800:
            n = 0;
            break;
        case 0x900:
        default:
            n = 1;
            break;
        case 0x700:
            n = 2;
            break;
        }
        n--;
        e = D_8005B9E0.e[D_8005B9E0.count];
        dst = e + 1;
        *e = arg0;
        while (n >= 0) {
            *dst++ = *arg1++;
            n--;
        }
        D_8005B9E0.count++;
    }
}

/* A queued camera message: Cam_QueueMessage writes the opcode and up to three words, Cam_ProcessMessage acts on them */
typedef struct CamMessage {
    /* 0x0 */ s32 opcode;
    /* 0x4 */ s32 args[3];
} CamMessage;

void Cam_ProcessMessage(CamMessage *);
extern CamMessage D_8005BA58[];

void Cam_ProcessMessages(void) {
    CamMessage *p;
    s32 n;

    n = D_8005B9E0.count;
    p = D_8005BA58;
    while (n > 0) {
        Cam_ProcessMessage(p);
        n--;
        p++;
    }
    D_8005B9E0.count = 0;
}

#ifdef NON_MATCHING
#define CMD_SET_LEVEL 0x0
#define CMD_CHANGE_LEVEL 0x100
#define CMD_FLAGS_SET 0x200
#define CMD_FLAGS_CLEAR 0x300
#define CMD_SET_FLAG_B99B 0x400
#define CMD_SET_PAIR 0x500
#define CMD_SET_SLOT 0x600
#define CMD_START_TIMER 0x700
#define CMD_STOP_TIMER 0x800
#define CMD_NOP 0x900
#define ARG_SHIFT 8
#define ENTRY_STRIDE_WORDS 3

extern char D_80010400[];
extern s8 D_8005B99B;
extern s32 D_8005B9E8[];
extern s32 D_8005B9F0[];
extern s32 D_8005BA00;
extern s32 D_8005BA04;
extern s32 D_800608FC;

void Cam_ProcessMessage(CamMessage *arg0) {
    s32 *cmd;
    s32 *p;
    s32 *q;
    s32 i;

    cmd = (s32 *)arg0;
    switch (cmd[0]) {
    case CMD_SET_LEVEL:
        func_80026334(cmd[1] >> ARG_SHIFT);
        break;
    case CMD_CHANGE_LEVEL:
        if (D_800608FC != (cmd[1] >> ARG_SHIFT)) {
            func_80022224(cmd[1] >> ARG_SHIFT);
            func_80023154();
        }
        break;
    case CMD_FLAGS_SET:
        D_8005B9E4 |= cmd[1] >> ARG_SHIFT;
        break;
    case CMD_FLAGS_CLEAR:
        D_8005B9E4 &= ~(cmd[1] >> ARG_SHIFT);
        break;
    case CMD_SET_FLAG_B99B:
        D_8005B99B = 1;
        break;
    case CMD_SET_PAIR:
        p = D_8005B9E8 + cmd[1] * ENTRY_STRIDE_WORDS;
        p[0] = cmd[2];
        p[1] = cmd[3];
        break;
    case CMD_SET_SLOT:
        i = cmd[1] * ENTRY_STRIDE_WORDS;
        q = (s32 *)&D_8005B9F4 + i;
        if (cmd[2] != 0) {
            q[2] = 1;
            D_8005B9F0[i] = cmd[3];
            q[1] = 0;
        } else {
            q[2] = -1;
            q[1] = 0;
        }
        break;
    case CMD_START_TIMER:
        D_8005BA00 = cmd[1];
        D_8005BA10 = 1;
        D_8005BA0C = 0;
        D_8005BA04 = cmd[2];
        break;
    case CMD_STOP_TIMER:
        D_8005BA10 = -1;
        D_8005BA0C = 0;
        break;
    case CMD_NOP:
        break;
    default:
        printf(D_80010400, cmd[0]);
        break;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", Cam_ProcessMessage);
#endif

typedef struct Unk80026334 {
    u8 pad0[3];
    s8 size;
    s32 key;
} Unk80026334;

extern s32 g_CameraPath[];
void func_8002648C(Unk80026334 *, s32);

void func_80026334(s32 arg0) {
    s32 key;
    Unk80026334 *p;

    key = arg0 & ~D_8005B9E4;
    if (key != 0) {
        p = (Unk80026334 *)Ent_GetProperty(g_CameraPath[0], 0x1A8, (Ent_GetPropertyRowLength(g_CameraPath[0], 0x4B, 0, 2) - 1) << 8, 0, 0, 1, 0, 0, 0);
        if (p != 0 && p->size != 0) {
            do {
                if (p->key == key) {
                    func_8002648C(p, 2);
                }
                p = (Unk80026334 *)((u8 *)p + p->size);
            } while (p->size != 0);
        }
        p = (Unk80026334 *)Ent_GetProperty(g_CameraPath[0], 0x1A8, 0, 0, 0, 1, 0, 0, 0);
        if (p != 0 && p->size != 0) {
            do {
                if (p->key == key) {
                    func_8002648C(p, 1);
                }
                p = (Unk80026334 *)((u8 *)p + p->size);
            } while (p->size != 0);
        }
    }
}
