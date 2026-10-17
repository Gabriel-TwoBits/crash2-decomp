#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F208[] = "vcm?X";

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002E884);

extern s32 D_8005C514;
extern s32 D_8005F3EC;
extern u8 *D_8006759C[];
extern s32 D_8006CB20;
extern s32 D_8006CB88;
extern s32 D_8006CB8C;
extern s32 D_8006CBA0;
extern s32 D_8006CBA8;
extern s32 D_8006CBE4;

void func_8002EE48(void) {
    func_800158E0();
    D_8006CBA0 = 0;
    switch (D_8005F3EC) {
    case 0x28:
    case 0x29:
    case 0x3B:
    case 0x3C:
        break;
    default: {
        s32 *cb20 = &D_8006CB20;
        u8 **p;
        D_8006CB8C = func_80019800(cb20, 4, 0, 0, 0, 0);
        D_8006CB88 = func_80019800(cb20, 4, 1, 0, 0, 0);
        D_8006CBA8 = func_80019800(cb20, 4, 5, 0, 0, 0);
        D_8006CBE4 = func_80019800(cb20, 4, 0x16, 0, 0, 0);
        func_80014260(D_8006759C[0] + 0x10, 0, 1);
        func_80014260(D_8006759C[0] + 0x24, 0, 1);
        func_80014260(D_8006759C[0] + 0x84, 0, 1);
        func_80014260(D_8006759C[0] + 0x98, 0, 1);
        func_80014260(D_8006759C[0] + 0x1C, 0, 1);
        func_80014260(D_8006759C[0] + 0x20, 0, 1);
        func_80014260(D_8006759C[0] + 0x34, 0, 1);
        break;
    }
    }
    func_8002F228(1);
    *(s32 **)0x1F80005C = &D_8005C514;
}

extern s32 D_8006CBB4;
extern s32 D_8006CBB8;
extern s32 D_8006CBBC;
extern s32 D_8006CBC0;
extern s32 D_8006CBC4;
extern s32 D_8006CBEC;
extern s32 D_8006CBF0;
extern s32 D_8006CBF4;
extern s32 D_8006CBF8;
extern s32 D_8006CBFC;
extern s32 D_8006CC80;
extern s32 D_8006CCEC;
extern s32 D_8006CCF0;
extern s32 D_8006CD24;
extern s32 D_8006CD3C;
extern s32 D_8006CDCC;
extern s32 D_8006CDD0;
extern s32 D_8006CDD4;
extern s32 D_8006CDD8;
extern s32 D_8006CDDC;
extern s32 D_8006CDE0;
extern s32 D_8006CDE4;
extern s32 D_8006CDE8;

void func_8002EFE4(void) {
    D_8006CBC0 = 0x63;
    D_8006CBBC = -1;
    D_8006CBB8 = 7;
    D_8006CBF8 = 0x100;
    D_8006CBFC = 0x100;
    D_8006CBEC = 0x400;
    D_8006CCEC = 1;
    D_8006CDD0 = 0x80;
    D_8006CDD4 = 0x80;
    D_8006CBC4 = 0;
    D_8006CBF0 = 0;
    D_8006CBF4 = 0;
    D_8006CC80 = 0;
    D_8006CD24 = 0;
    D_8006CD3C = 0;
    D_8006CCF0 = 0;
    D_8006CDE8 = 0;
    D_8006CDCC = 0;
    D_8006CDD8 = 0;
    D_8006CDDC = 0;
    D_8006CDE0 = 0xFF;
    D_8006CDE4 = 0xFF;
    D_8006CBB4 = 0;
    func_8002F0DC(1);
}

#ifdef NON_MATCHING
extern s32 D_8006935C[];
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CC00;
extern s32 D_8006CC84;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;
extern s32 D_8006CB98;
extern s32 D_8006CBCC;
extern s32 D_8006CBD0;
extern s32 D_8006CBD4;
extern s32 D_8006CBD8;
extern s32 D_8006CBDC;
extern s32 D_8006CBE0;
extern s32 D_8006CC28;
extern s32 D_8006CC2C;
extern s32 D_8006CC30;
extern s32 D_8006CC34;
extern s32 D_8006CC50;
extern s32 D_8006CC54;
extern s32 D_8006CC6C;
extern s32 D_8006CC70;
extern s32 D_8006CC88;
extern s32 D_8006CC8C;
extern s32 D_8006CCD4;
extern s32 D_8006CCD8;
extern s32 D_8006CD7C;
extern s32 D_8006CDA4;
extern s32 D_8006CDA8;
extern s32 D_8006CDC8;

void func_8002F0DC(s32 arg0) {
    s32 t;
    s32 *p;
    if (arg0 != 0) {
        p = D_80069718;
        t = D_8006CBEC;
        D_8006CD7C = 0;
        D_8006CD20 = 0;
        D_8006CB84 = 0;
        D_8006CC00 = 0;
        D_8006CD40 = 0;
        D_8006CBD4 = 0;
        D_8006CBD8 = 0;
        D_8006CBDC = 0;
        D_8006CBE0 = 0;
        D_8006CC28 = 0;
        D_8006CC2C = 0;
        D_8006CDA4 = 0;
        D_8006CDA8 = 0;
        D_8006CDC8 = 0;
        D_8006CC50 = 0;
        D_8006CC54 = 0;
        D_8006CC30 = 0;
        D_8006CC34 = 0;
        D_8006CC84 = -1;
        D_8006CBC0 = 0x63;
        D_8006CCD4 = 0x63;
        D_8006CCD8 = 0;
        D_8006CB98 = 0;
        D_8006CC6C = 0;
        D_8006CC70 = 0;
        D_8006CC88 = 0;
        D_8006CC8C = 0;
        D_8006CDB4 = -1;
        D_8006CDB8 = 0;
        D_8006CBD0 = t;
        memset(p, 0, 0x200);
        memset(D_8006935C, 0, 0x200);
        D_8006CBCC = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F0DC);
#endif

extern s32 D_80068FE4;
extern s32 D_80069718[];
extern s32 D_8006CB84;
extern s32 D_8006CC00;
extern s32 D_8006CC68;
extern s32 D_8006CC74;
extern s32 D_8006CC84;
extern s32 D_8006CC94;
extern s32 D_8006CC98;
extern s32 D_8006CCA0;
extern s32 D_8006CD14;
extern s32 D_8006CD18;
extern s32 D_8006CD1C;
extern s32 D_8006CD20;
extern s32 D_8006CD40;
extern s32 D_8006CD68;
extern s32 D_8006CD90;
extern s32 D_8006CD9C;
extern s32 D_8006CDA0;
extern s32 D_8006CDB4;
extern s32 D_8006CDB8;

void func_8002F228(s32 arg0) {
    int new_var;
    s32 *new_var2;

    D_8006CC94 = 0;
    D_8006CC68 = 0;
    D_8006CC74 = 0;
    if (D_8006CD14 != 2) {
        D_8006CCA0 = 0;
    }
    D_8006CC98 = 1;
    D_8006CD68 = 0;
    D_8006CD90 = 0;
    if (arg0 != 0) {
        new_var2 = (s32 *)D_8006759C[0] + 2;
        D_8006CD9C = 0;
        if (1) {
            D_8006CDA0 = 0;
            D_8006CB84 = 0;
            D_8006CC00 = 0;
        }
        D_8006CD40 = 0;
        D_8006CD20 = 0;
        D_8006CC84 = -1;
        new_var = (*new_var2) * 2;
        D_8006CDB4 = 0;
        D_8006CDB8 = (D_80069718[new_var] & 0xF) << 8;
    }
    func_80029884(0, 0, 0);
    D_80068FE4 = 0;
    func_8002FB84(1);
    D_8006CD1C = 0x20;
    if (D_8006CBB4 != 0x600) {
        D_8006CD18 = 0x120;
    }
}

#ifdef NON_MATCHING
#define CTX_FLAGS_OFFSET 0x29C
#define CTX_FLAGS_MASK 0x2000
#define MODE_INIT 0
#define MODE_MERGE 2
#define CLEAR_SIZE 0x4400
#define SRC_COPY_OFFSET 0x60
#define SRC_COPY_END_OFFSET 0x80
#define COPY_STEP_BYTES 0x10
#define KIND_OFFSET 0x120
#define KIND_FIRST 6
#define KIND_COUNT 2
#define CC00_STEP 0x100
#define ID_28 0x173
#define ID_38 0x288
#define CALL_FLAGS 2
#define NIBBLE_MASK 0xF
#define NIBBLE_BITS 4
#define NIBBLES_PER_WORD 8
#define TABLE_ROW_WORDS 2
#define BLOCK_WORDS 0x400
#define BLOCK_OFFSET 0x3C
#define BLOCK_SRC_DELTA 0x1060
#define TAIL_OFFSET 0x103C
#define CALLEE_OFFSET 0x1040
#define NO_ENTRY -1
#define VAL_SHIFT 8

extern u8 *D_8005F38C;
extern u8 *D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608E0;
extern u8 D_800619B8[];
extern s32 D_8006B1C4[];
extern s32 D_8006CD08[];

void func_8002F34C(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3) {
    s32 *src;
    s32 *dst;
    s32 *end;
    s32 i;
    s32 id;
    s32 isStatic;
    s32 *blk;

    if (*(s32 *)(*(u8 **)(D_800608CC + 0x10) + CTX_FLAGS_OFFSET) & CTX_FLAGS_MASK) {
        return;
    }
    isStatic = (arg1 == D_800619B8);
    blk = (s32 *)arg1;
    if (arg3 == MODE_INIT) {
        func_8001CC10((s32)arg0, (s32)D_8005F38C, CLEAR_SIZE, 0, (void *)0);
        if (arg2 != 0) {
            D_8006CC00 = 0;
            D_8006CC68 = 0;
        } else {
            D_8006CC00 += CC00_STEP;
        }
        *(s32 *)(arg1 + 0x34) = arg2;
        src = (s32 *)(D_8005F38C + SRC_COPY_OFFSET);
        end = (s32 *)(D_8005F38C + SRC_COPY_END_OFFSET);
        dst = (s32 *)arg1;
        do {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            src = (s32 *)((u8 *)src + COPY_STEP_BYTES);
            dst = (s32 *)((u8 *)dst + COPY_STEP_BYTES);
        } while (src != end);
        dst[0] = src[0];
        if ((u32)(arg0[KIND_OFFSET] - KIND_FIRST) >= KIND_COUNT) {
            blk[0] = *(s32 *)(arg0 + 0x60);
            blk[1] = *(s32 *)(arg0 + 0x64);
            blk[2] = *(s32 *)(arg0 + 0x68);
        }
        if (D_8006CC84 >= 0 && D_8006CC84 != 0) {
            blk[0] = D_8006CD08[0];
            blk[1] = D_8006CD08[1];
            blk[2] = D_8006CD08[2];
        }
        blk[3] = 0;
        blk[4] = 0;
        blk[5] = 0;
        *(s32 *)(arg1 + 0x24) = *(s32 *)(D_800608CC + 4);
        *(s32 *)(arg1 + 0x2C) = D_800608E0;
        *(s32 *)(arg1 + 0x30) = *(s32 *)(D_8006759C[0] + 8);
        *(s32 *)(arg1 + 0x28) = func_800321D4(D_800608D4, ID_28, 0, 0, 0, CALL_FLAGS, 0);
        if (D_8006CC84 < -1) {
            *(s32 *)(arg1 + 0x38) = NO_ENTRY;
        } else if (*(s32 *)(arg0 + 0xF0) == 0) {
            *(s32 *)(arg1 + 0x38) = NO_ENTRY;
        } else if (isStatic) {
            *(s32 *)(arg1 + 0x38) = NO_ENTRY;
        } else {
            id = func_800321D4(*(s32 *)(arg0 + 0xF0), ID_38, 0, 0, 0, CALL_FLAGS, 0);
            *(s32 *)(arg1 + 0x38) = id;
            if (id != 0) {
                D_8006CDB8 = ((D_80069718[*(s32 *)(arg1 + 0x30) * TABLE_ROW_WORDS + id / NIBBLES_PER_WORD]
                               >> ((id % NIBBLES_PER_WORD) * NIBBLE_BITS)) & NIBBLE_MASK) << VAL_SHIFT;
                D_8006CDB4 = *(s32 *)(arg1 + 0x38);
            }
        }
        func_80014638((u32 *)(arg1 + CALLEE_OFFSET));
    }
    if (arg3 == MODE_MERGE) {
        *(s32 *)(arg1 + TAIL_OFFSET) += D_8006CC68;
        for (i = 0; i < BLOCK_WORDS; i++) {
            blk[BLOCK_OFFSET / 4 + i] |= ~*(s32 *)(arg1 + BLOCK_SRC_DELTA + BLOCK_OFFSET + i * 4) & D_8006B1C4[i];
        }
    } else {
        *(s32 *)(arg1 + TAIL_OFFSET) = D_8006CC68;
    }
    if (arg3 != MODE_MERGE) {
        for (i = 0; i < BLOCK_WORDS; i++) {
            blk[BLOCK_OFFSET / 4 + i] = D_8006B1C4[i];
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F34C);
#endif

INCLUDE_ASM("asm/nonmatchings/game/8002E884", func_8002F6C8);
