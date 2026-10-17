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

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80023E60);

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

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_8002433C);

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
    *arg6 = sp20;
    *(s32 *)((char *)arg0 + 0xC) = var_s2;
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

INCLUDE_ASM("asm/nonmatchings/game/80023B04", func_80025D18);

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
