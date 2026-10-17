#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F204 = 0xF424;

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029884);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029D0C);

#ifdef NON_MATCHING
extern s32 D_80062AD8;

typedef struct Unk8002A1B8 {
    u8 pad0[0xA8];
    u32 flags;
    u8 padAC[0x100 - 0xAC];
    u32 f100;
    u8 pad104[0x11C - 0x104];
    s32 f11C;
} Unk8002A1B8;

s32 func_8002A1B8(Unk8002A1B8 *p, s32 arg1) {
    s32 a3;
    u32 t;
    s32 r;

    if (arg1 != 0) {
        a3 = ((u32)(arg1 & 0xE) >> 1) + 1;
    } else {
        a3 = 0;
    }
    switch ((u32)(arg1 & 0x3F0) >> 4) {
    case 1: p->f11C = 0x700; p->flags |= 0x400; break;
    case 2: p->f11C = 0xC00; p->flags |= 0x400; break;
    case 5: p->f11C = 0x1E00; p->flags |= 0x400; break;
    case 4:
        p->f11C = 0x1F00;
        p->flags |= 0x400;
        if ((p->f100 & 0x40000000) && a3 == 1) {
            p->f11C = 0x900;
            return 1;
        }
        break;
    case 3: p->f11C = 0x2100; p->flags |= 0x400; break;
    case 12: p->f11C = 0x2300; p->flags |= 0x400; break;
    case 11:
        p->f11C = 0x900;
        p->flags |= 0x400;
        return 1;
    case 6: p->f11C = 0xD00; p->flags |= 0x400; break;
    case 13: p->f11C = 0x3300; p->flags |= 0x400; break;
    case 18: p->f11C = 0x4200; p->flags |= 0x400; break;
    case 14: p->f11C = 0x3500; p->flags |= 0x02000400; break;
    case 15: p->flags |= 0x04000000; break;
    case 7: case 8: case 9: case 10:
        p->f11C = 0x1200; p->flags |= 0x2000; break;
    case 27: p->f11C = 0x3000; p->flags |= 0x400; break;
    case 28: p->f11C = 0x3100; p->flags |= 0x400; break;
    case 29: case 30: case 31: case 32: case 33:
    case 34: case 35: case 36: case 37: case 38:
        p->f11C = ((u32)(arg1 & 0x3F0) >> 4) + 0x40000; p->flags |= 0x400; break;
    default:
        break;
    }
    if (a3 == 4) {
        p->f11C = 0x900;
        p->flags |= 0x400;
        return 1;
    } else if (a3 == 6) {
        p->f11C = 0x4000;
        p->flags |= 0x400;
        return 0;
    } else if (a3 == 5) {
        return 1;
    } else if (a3 == 3) {
        if (!(D_80062AD8 & 0x200)) {
            return 1;
        }
        r = 1;
        if ((p->f100 & 0x24000000) == 0x24000000) {
            r = 0;
        }
        return r;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A1B8);
#endif

#ifdef NON_MATCHING
#define UNK_FLAGS_OFFSET 0x100
#define UNK_FLAG_FIXED_HEIGHT 0x40000000
#define UNK_PTR_OFFSET 8
#define UNK_PTR_HEIGHT_OFFSET 0x40
#define UNK_FIXED_HEIGHT 0x29900
#define UNK_BOX_OFFSET 0x1020
#define UNK_OUT_OFFSET 0x1008
#define UNK_BOX_NEAR_NUMERATOR (-0x9600)
#define UNK_BOX_FAR_NUMERATOR 0x9600
#define UNK_BOX_MARGIN 0xC80
#define UNK_BOX_DROP 0x4B00
#define UNK_CALL_KEY 0xC4653601

void func_8002A3B0(void *arg0, void *arg1, void *arg2, void *arg3, s32 arg4, s32 arg5) {
    s32 *src = (s32 *)arg2;
    s32 *box = (s32 *)((char *)arg3 + UNK_BOX_OFFSET);
    s32 base;
    s32 diff;
    s32 near;
    s32 far;
    s32 y;

    base = src[1] + D_8005F204;
    diff = ((s32 *)arg1)[1] - src[1];
    if (diff > 0) {
        base += diff;
    }
    near = UNK_BOX_NEAR_NUMERATOR / arg4;
    far = UNK_BOX_FAR_NUMERATOR / arg4;
    box[0] = src[0] + near + UNK_BOX_MARGIN;
    box[1] = src[1] - UNK_BOX_DROP;
    box[2] = src[2] + near + UNK_BOX_MARGIN;
    box[3] = src[0] + far - UNK_BOX_MARGIN;
    if (!(*(s32 *)((char *)arg0 + UNK_FLAGS_OFFSET) & UNK_FLAG_FIXED_HEIGHT)) {
        y = src[1] + *(s32 *)(*(char **)((char *)arg0 + UNK_PTR_OFFSET) + UNK_PTR_HEIGHT_OFFSET);
    } else {
        y = src[1] + UNK_FIXED_HEIGHT;
    }
    box[4] = y;
    box[5] = src[2] + far - UNK_BOX_MARGIN;
    func_8003CD0C(arg0, arg3, (char *)arg3 + UNK_OUT_OFFSET, box, base, arg5, UNK_CALL_KEY, &func_8002A1B8);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A3B0);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A554);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A8D0);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B164);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B57C);

#ifdef NON_MATCHING
#define F(p, off) (*(s32 *)((s8 *)(p) + (off)))
#define SUB(p) (*(void **)((s8 *)(p) + 8))

void func_8002BC38(void *arg0, s32 *arg1, s32 *arg2, void *arg3) {
    s32 sp18;
    s32 ret;
    s32 y;
    s32 a;
    s32 b;
    s32 flag;
    void *sub;

    flag = 0;
    if (F(arg0, 0xA8) & 0x100000) {
        sub = SUB(arg0);
        if ((F(sub, 0x4C) & 0x100000) && (F(sub, 0x50) & 0x100000) && (F(sub, 0x54) & 0x100000)
            && (F(sub, 0x58) & 0x100000) && (F(sub, 0x5C) & 0x100000) && (F(sub, 0x60) & 0x100000)
            && (F(sub, 0x64) & 0x100000) && (F(sub, 0x68) & 0x100000)) {
            a = F(arg0, 0x100) & 0x40000000;
            flag = a == 0;
        }
    }
    ret = func_8002B57C(arg0, arg2, arg3, flag);
    if (ret == 0xC4653601) {
        F(arg3, 0x103C) = 0;
    } else {
        F(arg3, 0x103C) = ret;
    }
    if (ret == 0xC4653601) {
        goto fail;
    }
    b = arg2[1];
    if (!(F(arg0, 0x100) & 0x40000000)) {
        if (ret - 1 >= b + F(SUB(arg0), 0x40)) {
            goto fail;
        }
    } else if (ret - 1 >= b + 0x29900) {
        goto fail;
    }
    if (!(F(arg0, 0xA8) & 1)) {
        y = ret - ((F(arg0, 0x100) & 0x40000000) ? 0x29900 : F(SUB(arg0), 0x40)) - 1;
        if (F(arg0, 0x100) & 0x40) {
            if (arg2[1] < y) {
                y = arg2[1];
            }
            arg2[1] = func_8001E5FC(arg1[1], y, 0x4E20);
        } else {
            b = arg2[1];
            a = arg1[1];
            if (a < b) {
                if (y < a) {
                    y = a;
                }
            } else if (y < b) {
                y = b;
            }
            arg2[1] = y;
        }
        F(arg0, 0xA8) |= 0x80;
        if (F(arg0, 0x88) > 0) {
            F(arg0, 0x88) = 0;
        }
        return;
    }
fail:
    if (F(arg0, 0x100) & 0x40) {
        if (ret == 0xC4653601) {
            arg2[1] = arg1[1];
        } else {
            sp18 = 0;
            y = ret - ((F(arg0, 0x100) & 0x40000000) ? 0x29900 : F(SUB(arg0), 0x40)) - 1;
            arg2[1] = func_8001E5FC(arg1[1], y, 0x4E20);
            func_8001CC10(0, (s32)arg0, 0x3300, 1, &sp18);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002BC38);
#endif

extern void *D_800608CC;
extern void *D_8006759C;
extern s32 D_8006CB64;
extern s32 D_8006CD48;

s32 func_8002BF58(void *arg0, void *arg1) {
    s32 sp18;
    s32 temp_v0;
    s32 temp_v1_2;
    s32 var_a0;
    s32 var_a2;
    u32 temp_v1;
    s8 *new_var;
    void *temp_a1;
    void *temp_s2;

    temp_v0 = func_80027DEC(D_800608CC);
    if (temp_v0 >= -0xFE) {
        temp_a1 = *(void **)((s8 *)arg0 + 0x18);
        temp_s2 = *(void **)((s8 *)temp_a1 + 0x14);
        if (*(s32 *)((s8 *)arg1 + 4) < ((*(s32 *)((s8 *)temp_s2 + 4) << 8) - *(s32 *)((s8 *)*(void **)((s8 *)arg0 + 8) + 0x34))) {
            var_a0 = 1;
            new_var = (s8 *)arg1;
            if ((*(s32 *)((s8 *)*(void **)((s8 *)temp_a1 + 0x10) + 0x29C) & 2) && (*(s32 *)((s8 *)arg0 + 0x108) != 2)) {
                temp_v1 = *(u32 *)((s8 *)D_8006759C + 8);
                sp18 = 0x6400;
                var_a2 = 0x900;
                if (temp_v1 != 0x19) {
                    if (temp_v1 >= 0x19U) {
                        if (temp_v1 < 0x22U) {
                            if (temp_v1 >= 0x20U) {
                                goto block_8;
                            }
                        }
                    }
                } else {
block_8:
                    if (!(*(s32 *)((s8 *)*(void **)((s8 *)D_800608CC + 0x10) + 0x29C) & 0x800)) {
                        var_a2 = 0x2100;
                        sp18 = D_8006CD48;
                    }
                }
                func_8001CC10(0, (s32)arg0, var_a2, 1, &sp18);
                var_a0 = 0;
            }
            if (var_a0 != 0) {
                *(s32 *)(new_var + 4) = (s32)((*(s32 *)((s8 *)temp_s2 + 4) << 8) - *(s32 *)((s8 *)*(void **)((s8 *)arg0 + 8) + 0x34));
                temp_v1_2 = *(s32 *)((s8 *)arg0 + 0x88);
                *(s32 *)((s8 *)(temp_s2 = arg0) + 0x88) = 0;
                *(s32 *)((s8 *)arg0 + 0x114) = temp_v1_2;
                *(s32 *)((s8 *)arg0 + 0xA8) = (s32)(*(s32 *)((s8 *)arg0 + 0xA8) | 1);
                *(s32 *)((s8 *)arg0 + 0x110) = (s32)D_8006CB64;
            }
        }
    } else {
        *(void **)((s8 *)arg0 + 0x18) = (void *)temp_v0;
    }
    return -0xFF;
}

void func_8002C0F0(void *arg0, void *arg1, s32 *arg2, s32 *arg3, s32 *arg4) {
    s32 pos[4];
    s32 delta[4];
    s32 sp40;
    s32 sp44;
    u32 tmp;
    s32 sp48;
    s32 temp_s2;
    s32 temp_s3;
    s32 x;
    s32 y;
    s32 d;
    s32 i;

    for (i = 0; i < 3; i++) {
        delta[i] = arg3[i];
    }
    for (i = 0; i < 3; i++) {
        pos[i] = arg2[i] + delta[i];
    }
    func_8002B164(arg0, arg2, pos, arg1, &sp40);
    func_8002DE08(arg2, pos, arg0, arg1);
    x = (pos[0] - arg2[0]) * 4;
    if (x < 0) {
        x += 0x1FFF;
    }
    y = (pos[2] - arg2[2]) * 4;
    temp_s2 = (x >> 13) + 0x10;
    if (y < 0) {
        y += 0x1FFF;
    }
    temp_s3 = (y >> 13) + 0x10;
    if (func_8002E280(arg2, temp_s2, temp_s3, &sp44, &sp48, arg0, 0) != 0) {
        pos[0] = ((sp44 - 0x10) << 13 >> 2) + arg2[0];
        pos[2] = ((sp48 - 0x10) << 13 >> 2) + (tmp = arg2[2]);
    } else {
        pos[0] = arg2[0];
        pos[2] = arg2[2];
    }
    *(s32 *)((u8 *)arg1 + 0x104C) += 1;
    if ((temp_s2 != 0x10 || temp_s3 != temp_s2) && temp_s2 == sp44 && temp_s3 == sp48) {
        *(s32 *)((u8 *)arg0 + 0xA8) |= 0x100;
    }
    func_8002BC38(arg0, arg2, pos, arg1);
    func_8002BF58(arg0, pos);
    for (i = 0; i < 3; i++) {
        tmp = pos[i];
        arg4[i] = tmp;
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002C324);

extern char D_8006CB68[];

typedef struct { s32 n; u8 pad[0x28C]; s32 *p[1]; } Unk8006CB6C;
extern Unk8006CB6C D_8006CB6C;

void func_8002CA1C(void) {
    char *v;
    if (D_8006CB6C.n != 0) {
        D_8006CB6C.n -= 1;
        v = *(char **)D_8006CB68;
        switch (*D_8006CB6C.p[D_8006CB6C.n] & 7) {
        case 0:
            v -= 0x20;
            break;
        case 1:
            v -= 0x44;
            break;
        }
        *(char **)D_8006CB68 = v;
    }
}

void func_8002CA98(void) {
    *(char **)D_8006CB68 = D_8006CB68 + 0x414;
    D_8006CB6C.n = 0;
}

extern s32 D_8005BC18[];
s32 D_8005F378;
s32 *D_8005F3C4;
s32 D_8005F3E8;

s32 func_8002CAB8(void) {
    s32 i;
    s32 *dst;
    s32 *src;

    D_8005F3C4 = (s32 *)0x1F800100;
    D_8005F378 = 0x1F800180;
    D_8005F3E8 = 0x1F800380;
    for (i = 0, dst = (s32 *)0x1F800380, src = D_8005BC18; i < 0x20; i++) {
        *dst++ = *src++;
    }
    return -0xFF;
}

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002CB14);

void func_8002CB1C(s32 arg0, s32 arg1) {
    if (((arg0 < 0) ? (-arg0) : (arg0)) < 0x20) {
        if (((arg1 < 0) ? (-arg1) : (arg1)) < 0x20) {
            if (arg0 || arg1) {
                func_8003CBE4();
            } else {
                func_8003CBE4();
            }
        }
    }
}

void func_8002CB68(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 *p;
    s32 x;
    s32 y;
    s32 end;
    s32 mask;

    arg2 += 0x10;
    mask = -1;
    if (arg2 < 0x20) {
        if (arg2 <= 0) {
            return;
        }
        mask <<= 0x20 - arg2;
    }
    arg0 += 0xF;
    if (arg0 >= 0x20) {
        return;
    }
    if (arg0 >= 0) {
        mask &= ~((s32)0x80000000 >> arg0);
    }
    if (mask == 0) {
        return;
    }
    x = arg1 + 0x10;
    if (x < 0) {
        x = 0;
    }
    end = arg3 + 0x10;
    if (end >= 0x21) {
        end = 0x20;
    }
    if (x >= 0x20) {
        return;
    }
    p = D_8005F3C4 + x;
    if (arg4 != 0) {
        if (x < end) {
            do {
                x++;
                *p |= mask;
                p++;
            } while (x < end);
        }
    } else {
        mask = ~mask;
        while (x < end) {
            x++;
            *p &= mask;
            p++;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002CC50);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002D6E4);

#ifdef NON_MATCHING
#define WORLD_FLAGS_OFFSET 0x29C
#define WORLD_FLAG_SKIP 0x100000
#define OBJ_FLAGS_OFFSET 0x100
#define OBJ_FLAG_SKIP 0x10000000
#define OBJ_FLAG_FIXED_BASE 0x40000000
#define OBJ_MODE_OFFSET 0x108
#define OBJ_MODE_MIN 3
#define OBJ_MODE_SPAN 2
#define OBJ_MODE_ALT 2
#define OBJ_BITS_OFFSET 0xB0
#define OBJ_BITS_MASK 2
#define OBJ_ALT_VALUE 2
#define OBJ_X_OFFSET 0x64
#define OBJ_X_END_DELTA 0x3200
#define OBJ_PARENT_OFFSET 0x8
#define PARENT_BASE_OFFSET 0x40
#define FIXED_BASE_OFFSET 0x29900
#define BUF_B_OFFSET 0x1008
#define TABLE_A_LAST 0x7C
#define TABLE_A_WORDS 32
#define TABLE_B_LAST 0x1FC
#define TABLE_B_WORDS 128

extern s32 D_8005F204;
extern s32 D_8005F378;
extern s32 *D_8005F3C4;


void func_8002DE08(s32 *arg0, s32 *arg1, void *arg2, void *arg3) {
    s32 *p;
    s32 i;
    s32 mode;
    s32 flags;
    s32 base;
    s32 x;

    if (!(*(s32 *)(*(s8 **)((s8 *)D_800608CC + 0x10) + WORLD_FLAGS_OFFSET) & WORLD_FLAG_SKIP)) {
        if (!(*(s32 *)((s8 *)arg2 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_SKIP)) {
            p = (s32 *)((s8 *)D_8005F3C4 + TABLE_A_LAST);
            for (i = TABLE_A_WORDS - 1; i >= 0; i--) {
                *p = 0;
                p--;
            }
            p = (s32 *)(D_8005F378 + TABLE_B_LAST);
            for (i = TABLE_B_WORDS - 1; i >= 0; i--) {
                *p = 0;
                p--;
            }
            mode = *(s32 *)((s8 *)arg2 + OBJ_MODE_OFFSET);
            flags = *(s32 *)((s8 *)arg2 + OBJ_BITS_OFFSET);
            if ((u32)(mode - OBJ_MODE_MIN) < OBJ_MODE_SPAN) {
                flags = OBJ_ALT_VALUE;
            } else if (mode == OBJ_MODE_ALT) {
                flags = OBJ_ALT_VALUE;
            } else {
                flags = flags & OBJ_BITS_MASK;
            }
            base = arg0[1];
            if (*(s32 *)((s8 *)arg2 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_FIXED_BASE) {
                base += FIXED_BASE_OFFSET;
            } else {
                base += *(s32 *)(*(s8 **)((s8 *)arg2 + OBJ_PARENT_OFFSET) + PARENT_BASE_OFFSET);
            }
            x = *(s32 *)((s8 *)arg2 + OBJ_X_OFFSET);
            func_8003C88C(arg3, (s8 *)arg3 + BUF_B_OFFSET, flags, x + D_8005F204,
                          x + OBJ_X_END_DELTA, base, arg0[0], arg0[2]);
        }
    }
    func_8002D6E4(arg0, arg1, arg2, arg3, 1);
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DE08);
#endif

#ifdef NON_MATCHING
extern s32 *D_8005F3C4;

#define BIT_COUNT 32
#define TOP_BIT 0x80000000U

s32 func_8002DF68(u32 arg0, u32 arg1) {
    s32 result;

    result = 0;
    if ((arg0 < BIT_COUNT) && (arg1 < BIT_COUNT)) {
        result = (D_8005F3C4[arg1] & (TOP_BIT >> arg0)) == 0;
    } else {
        /* out of range: result stays 0 */
    }
    return result;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DF68);
#endif

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DFA8);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E280);

#ifdef NON_MATCHING
typedef struct Unk8002E628 {
    s32 lo;
    s32 hi;
    s32 *data;
} Unk8002E628;

s32 *func_8002E628(s32 *arg0, s32 arg1, s32 arg2, Unk8002E628 *arg3) {
    s32 q;

    if (arg3->data != 0) {
        do {
            if (arg1 >= arg3->lo && arg3->hi >= arg1) {
                q = (arg1 - arg3->lo) / arg2;
                *arg0 = arg3->lo + q * arg2;
                return arg3->data + (q >> 8);
            }
            arg3++;
        } while (arg3->data != 0);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E628);
#endif

#ifdef NON_MATCHING
void func_8002E6E0(s32 *arg0, s32 *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 dx, dz, pz, px, len;

    dx = (arg3 - arg2) >> 8;
    dz = (arg5 - arg4) >> 8;
    pz = (arg1[2] - arg4) >> 8;
    px = (arg1[0] - arg2) >> 8;
    len = func_80041A78(dx * dx + dz * dz);
    dz = (dz << 12) / len;
    dx = (dx << 12) / len;
    pz = (pz * dz - px * dx) >> 12;
    arg0[0] = arg2 - ((pz * dx) >> 16);
    arg0[1] = arg1[1];
    arg0[2] = ((pz * dz) >> 16) + arg4;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E6E0);
#endif

void func_8002E854(s32 *arg0, s32 *arg1, s32 *arg2) {
    s32 b = arg2[0];
    s32 a = *arg1;

    arg0[0] = a - (b - a);
    arg0[1] = arg2[1];
    arg0[2] = arg2[2];
}
