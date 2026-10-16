#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F204 = 0xF424;

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029884);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_80029D0C);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A1B8);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A3B0);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A554);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002A8D0);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B164);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002B57C);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002BC38);

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

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DE08);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DF68);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002DFA8);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E280);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E628);

INCLUDE_ASM("asm/nonmatchings/game/80029884", func_8002E6E0);

void func_8002E854(s32 *arg0, s32 *arg1, s32 *arg2) {
    s32 b = arg2[0];
    s32 a = *arg1;

    arg0[0] = a - (b - a);
    arg0[1] = arg2[1];
    arg0[2] = arg2[2];
}
