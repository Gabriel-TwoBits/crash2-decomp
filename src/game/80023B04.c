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

extern s32 D_800608D4[];
extern s32 D_800608F0;
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
    s32 sp28;
    s32 key;
    s32 *tbl;
    s32 idx;
    s16 *p;
    s32 x, y, w, x1, y1;
    u8 *obj;
    s32 arg;
    s32 unk8;

    idx = 0;
    if (arg0->unk4 == D_800608D4[0]) {
        key = D_800608F0;
        arg = D_800608DC;
    } else {
        key = func_800321D4(arg0->unk4, KEY_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
        idx = func_800321D4(arg0->unk4, IDX_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
        obj = (u8 *)arg0->unk0;
        arg = *(s32 *)(obj + (idx * ENTRY_WORDS_PER_IDX + D_800608FC + *(s32 *)(*(s32 *)(obj + OBJ_CTX_OFFSET) + CTX_BASE_OFFSET)) * 4 + ENTRY_PRIMARY_OFFSET);
    }
    tbl = D_8005BA28;
    unk8 = arg0->unk8;
    if (key == 0) {
        tbl = D_8005BA28 + ALT_TABLE_WORDS;
    }
    p = (s16 *)func_80031AE8(arg, LOOKUP_ID, unk8, 0, 0, LOOKUP_FLAGS, &sp28, 0, 0);
    if (p == 0) {
        if (arg0->unk4 == D_800608D4[0]) {
            arg = D_800608D8;
        } else {
            obj = (u8 *)arg0->unk0;
            arg = *(s32 *)(obj + (idx * ENTRY_WORDS_PER_IDX + *(s32 *)(*(s32 *)(obj + OBJ_CTX_OFFSET) + CTX_BASE_OFFSET)) * 4 + ENTRY_FALLBACK_OFFSET);
        }
        p = (s16 *)func_80031AE8(arg, LOOKUP_ID, unk8, 0, 0, LOOKUP_FLAGS, &sp28, 0, 0);
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

extern s32 D_800608D4[];
extern s32 D_800608E0;

void func_80023D7C(void) {
    func_800321D4(*D_800608D4, UNK_ARG1, D_800608E0, 0, 0, 3, UNK_ARG6);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023D7C);
#endif

#ifdef NON_MATCHING
extern s32 D_8005BA4C;
extern s32 D_800608DC;
extern s32 D_800608E0;

s32 func_80023DC4(void) {
    s32 sp28;
    s32 v;
    s32 a0;
    s32 a1;

    v = func_80031AE8(D_800608DC, 0x16E, D_800608E0, 0, 0, 3, &sp28, 0, 0);
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
            a = ((func_800322C0(arg0->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - a;
        }
        dist = a;
        a = last->unk8;
        if (last->unkC == DIR_FORWARD) {
            a = ((func_800322C0(last->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - a;
        }
        dist += a;
        i = arg1 - 2;
        if (i > 0) {
            p = arg0 + 1;
            do {
                dist = dist + ((func_800322C0(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT) - 2;
                p++;
                i--;
            } while (i != 0);
        }
    }
    if (arg3 < 0) {
        speed = func_800321D4(arg0->unk4, SPEED_QUERY, 0, 0, 0, 2, 0);
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
        end = (func_800322C0(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT;
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
        a = (func_800322C0(p->unk4, SEGMENT_LENGTH_QUERY, 0, 2) - 1) << POSITION_SHIFT;
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

s32 func_80024188(void) {
    func_800241A8();
    return -0xFF;
}

extern s8 D_8005B994;
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
extern s32 D_800608F0;
extern s32 D_800608F8;
extern s32 D_80060918;
extern s32 D_8006CD98;

void func_800241A8(void) {
    D_8005B994 = 5;
    D_80060918 = 0x1001;
    D_8005B9AC = 0x1001;
    D_800608F0 = 5;
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
            lim = (func_800322C0(p[1], 0x4B, 0, 2) - 1) << 8;
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

extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
extern s32 D_80060828;
extern Unk8002433CMode *D_8006759C;
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
    mode = func_800242F8(func_800321D4(obj, KEY_QUERY, 0, 0, 0, QUERY_FLAGS, 0));
    if (obj == D_800608D4[0]) {
        dx = a1->x - D_800607F4;
        dy = a1->y - D_800607F8;
        ang2 = D_80060828;
        dz = a1->z - D_800607FC;
    } else {
        ang >>= FIXED_SHIFT;
        e = (s16 *)func_80031AE8(obj, KEY_ENTRY, 0, ang, 0, QUERY_FLAGS, 0, 0, 0);
        o = ((Unk8002433CObj *)obj)->info->pos;
        dx = a1->x - ((o->x + e[0]) << FIXED_SHIFT);
        dy = a1->y - ((o->y + e[1]) << FIXED_SHIFT);
        dz = a1->z - ((o->z + e[2]) << FIXED_SHIFT);
        if (mode != MODE_NO_ROTATE) {
            Unk8002433CRoot *rt = (Unk8002433CRoot *)a0->unk0;
            s32 i = func_800321D4(obj, IDX_QUERY, 0, 0, 0, QUERY_FLAGS, 0);
            e = (s16 *)func_80031AE8(*(s32 *)((s32)rt + (i * ENTRY_STRIDE_WORDS + rt->tab->base) * 4 + 0x14), KEY_ENTRY, 0, ang * 2, 0, QUERY_FLAGS, 0, 0, 0);
            ang2 = e[1];
        } else {
            ang2 = 0;
        }
    }
    if (D_8006759C->mode == MODE_FROZEN) {
        ang2 = 0;
    }
    if (mode == MODE_NO_ROTATE) {
        r = -dy;
    } else {
        ang2 = -ang2;
        if (a4 != 0) {
            *a4 = ang2;
        }
        s = func_8003D360(ang2);
        c = func_8003D2EC(ang2);
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

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024658);

#ifdef NON_MATCHING
extern char D_80010380[];

s32 func_80024A48(void *arg0, s32 arg1, s32 *arg2, s32 arg3, void *arg4, s32 arg5, s32 *arg6) {
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
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024A48);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80024B6C);

#ifdef NON_MATCHING
extern char D_800103C0[];

s32 func_800253B0(void *arg0, s32 arg1, s32 *arg2, s32 arg3, s32 arg4, s32 arg5, s32 *arg6) {
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
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800253B0);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800254D4);

#ifdef NON_MATCHING
extern s32 D_800607F4;
extern s32 D_800607F8;
extern s32 D_800607FC;
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

    yRange = (Unk80025D18Range *)func_80031AE8(D_800608D8, ENTRY_TYPE_Y_RANGE, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (yRange != 0) {
        xMin = yRange->min;
        xMax = yRange->max;
        if (xMin <= xMax) {
            dy = arg0->y - D_800607F8;
            if (dy < (xMin << FIXED_SHIFT) || (xMax << FIXED_SHIFT) < dy) {
                sp28 = yRange->value << FIXED_SHIFT;
                func_80025FDC(0, &sp28);
            }
        }
    }
    xRange = (Unk80025D18Range *)func_80031AE8(D_800608D8, ENTRY_TYPE_X_RANGE, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (xRange != 0) {
        xMin = xRange->min;
        xMax = xRange->max;
        if (xMax < xMin) {
            xRange = 0;
        }
    }
    zRange = (Unk80025D18Range *)func_80031AE8(D_800608D8, ENTRY_TYPE_Z_RANGE, D_800608E0, 0, 0, 0, 0, 0, 0);
    if (zRange != 0) {
        zMin = zRange->min;
        zMax = zRange->max;
        if (zMax < zMin) {
            zRange = 0;
        }
    }
    if (xRange != 0 || zRange != 0) {
        dx = arg0->x - D_800607F4;
        dz = arg0->z - D_800607FC;
        negAngle = -D_80060828;
        sin = func_8003D360(negAngle);
        cos = func_8003D2EC(negAngle);
        rotX = (((dz >> FIXED_SHIFT) * cos) >> TRIG_SHIFT) + (((dx >> FIXED_SHIFT) * sin) >> TRIG_SHIFT);
        rotZ = (((dx >> FIXED_SHIFT) * cos) >> TRIG_SHIFT) - (((dz >> FIXED_SHIFT) * sin) >> TRIG_SHIFT);
        if (xRange != 0 && (rotX < (xMin << FIXED_SHIFT) || (xMax << FIXED_SHIFT) < rotX)) {
            sp28 = xRange->value << FIXED_SHIFT;
            func_80025FDC(0, &sp28);
        }
        if (zRange != 0 && (rotZ < (zMin << FIXED_SHIFT) || (zMax << FIXED_SHIFT) < rotZ)) {
            sp28 = zRange->value << FIXED_SHIFT;
            func_80025FDC(0, &sp28);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80025D18);
#endif

void func_80025FDC(s32 arg0, s32 *arg1) {
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

typedef struct Unk8005BA58 {
    u8 pad[0x10];
} Unk8005BA58;

void func_800260F4(Unk8005BA58 *);
extern Unk8005BA58 D_8005BA58[];

void func_8002609C(void) {
    Unk8005BA58 *p;
    s32 n;

    n = D_8005B9E0.count;
    p = D_8005BA58;
    while (n > 0) {
        func_800260F4(p);
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

void func_800260F4(Unk8005BA58 *arg0) {
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
INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_800260F4);
#endif

typedef struct Unk80026334 {
    u8 pad0[3];
    s8 size;
    s32 key;
} Unk80026334;

extern s32 D_800608D4[];
void func_8002648C(Unk80026334 *, s32);

void func_80026334(s32 arg0) {
    s32 key;
    Unk80026334 *p;

    key = arg0 & ~D_8005B9E4;
    if (key != 0) {
        p = (Unk80026334 *)func_80031AE8(D_800608D4[0], 0x1A8, (func_800322C0(D_800608D4[0], 0x4B, 0, 2) - 1) << 8, 0, 0, 1, 0, 0, 0);
        if (p != 0 && p->size != 0) {
            do {
                if (p->key == key) {
                    func_8002648C(p, 2);
                }
                p = (Unk80026334 *)((u8 *)p + p->size);
            } while (p->size != 0);
        }
        p = (Unk80026334 *)func_80031AE8(D_800608D4[0], 0x1A8, 0, 0, 0, 1, 0, 0, 0);
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
