#include "common.h"
#include "sdk.h"
#include "game.h"

#define NIBBLE_BITS 4
#define NIBBLE_MASK 0xF
#define NIBBLES_PER_WORD 8
#define WORDS_PER_ROW 2
#define COUNTER_MAX 0xF
#define COUNT_SHIFT 8
#define FLAG_CLEAR_2 (~2)
#define FLAG_SET_8 8
#define FLAG_CLEAR_1 (~1)
#define FLAG_TABLE_WORDS 0x400
#define SLOT_INDEX_SHIFT 8
#define SPECIAL_ARG_FIELD_BASE 0x3C
#define COPY_WORDS_END 0x20
#define COPY_DEST_OFFSET 0x60
#define COPY_STEP 0x10
#define INIT_ARG_FLAGS 0x0C01FFFF
#define STEP_AMOUNT 0x100
#define SCREEN_CALL_KIND_SPECIAL 0x1A
#define SCREEN_CALL_KIND_NORMAL 0x1B

extern u8 D_800619B8[];
extern u8 D_8006CB48[];
extern u8 *D_8005F38C;
extern s32 D_8005F3B4;
extern s32 D_80060884;
extern u8 *D_800608CC;
extern s32 D_800608D0;
extern s32 D_800608D4;
extern s32 D_80060944;
extern void *D_80063458;
extern s32 D_8006B1C4[];
extern s32 D_8006CB78;
extern s32 D_8006CB80;
extern s32 D_8006CC60;
void SetGeomScreen(s32);
void func_8001A014(void);
void func_8001F858(s32);
void func_80014790(u32 *);

void func_8002F6C8(u8 *arg0) {
    s32 sp18;
    s32 isSpecial;
    s32 i;
    s32 idx;
    u8 *obj;
    u8 *slot;
    u8 *t0;
    u8 *dst;
    u8 *src;
    u32 *word;
    s32 shift;

    isSpecial = arg0 == D_800619B8;
    idx = *(s32 *)(arg0 + 0x38);
    if (idx >= 0 && !isSpecial) {
        word = (u32 *)&D_80069718[*(s32 *)(arg0 + 0x30) * WORDS_PER_ROW + (idx >> 3)];
        shift = (idx % NIBBLES_PER_WORD) * NIBBLE_BITS;
        if (((*word >> shift) & NIBBLE_MASK) < COUNTER_MAX) {
            *word = *word + (1 << shift);
        }
        idx = *(s32 *)(arg0 + 0x38);
        D_8006CDB8 = ((D_80069718[*(s32 *)(arg0 + 0x30) * WORDS_PER_ROW + (idx >> 3)]
                       >> ((idx - (idx >> 3) * NIBBLES_PER_WORD) * NIBBLE_BITS)) & NIBBLE_MASK) << COUNT_SHIFT;
        D_8006CDB4 = *(s32 *)(arg0 + 0x38);
    } else {
        D_8006CDB8 = 0;
        D_8006CDB4 = 0;
    }
    D_8006CC60 = 0;
    func_8001D194(0, 0x1300, 0, 0, 0);
    if (D_800608CC != 0) {
        func_8001A014();
        func_8001A23C(D_800608CC, -1);
        obj = *(u8 **)(D_800608CC + 0x10);
        D_800608D0 = -1;
        for (i = 0; i < *(s32 *)(obj + 0x190); i++) {
            slot = obj + 0x194 + i * 4;
            if (func_800143C0(slot, 0) != 0) {
                func_8002001C(func_80014260(slot, 1, 0));
            }
        }
        func_80014790((u32 *)(arg0 + 0x1040));
        func_800163B4(D_80063458);
    }
    D_800608CC = 0;
    D_800608D4 = 0;
    SetGeomScreen(D_80060884);
    func_800241A8();
    for (i = 0; i < FLAG_TABLE_WORDS; i++) {
        D_8006B1C4[i] = *(s32 *)(arg0 + SPECIAL_ARG_FIELD_BASE + i * 4);
    }
    if (!isSpecial && D_8006CC84 > 0) {
        D_8006B1C4[(u32)D_8006CC84 >> SLOT_INDEX_SHIFT] &= FLAG_CLEAR_2;
        D_8006B1C4[(u32)D_8006CC84 >> SLOT_INDEX_SHIFT] |= FLAG_SET_8;
    }
    for (i = 0; i < FLAG_TABLE_WORDS; i++) {
        D_8006B1C4[i] &= FLAG_CLEAR_1;
    }
    func_8001F858(*(s32 *)(D_8006759C[0] + 8));
    sp18 = *(s32 *)(arg0 + 0x24);
    t0 = func_80014260(&sp18, 1, 1);
    {
        s32 a1 = *(s32 *)(t0 + (*(s32 *)(arg0 + 0x28) * 3 + *(s32 *)(*(u8 **)(t0 + 0x10) + 0x184)) * 4 + 0x10);
        s32 kind;
        if (isSpecial) {
            kind = SCREEN_CALL_KIND_SPECIAL;
        } else {
            kind = SCREEN_CALL_KIND_NORMAL;
        }
        func_800200A8((s32)t0, a1, *(s32 *)(arg0 + 0x2C), kind);
    }
    func_800143C0(&sp18, 1);
    if (D_8005F38C == 0) {
        func_80019800(D_8006CB48, 0, 0, 0, 0, 1);
    }
    src = arg0;
    dst = D_8005F38C + COPY_DEST_OFFSET;
    do {
        *(s32 *)(dst + 0) = *(s32 *)(src + 0);
        *(s32 *)(dst + 4) = *(s32 *)(src + 4);
        *(s32 *)(dst + 8) = *(s32 *)(src + 8);
        *(s32 *)(dst + 12) = *(s32 *)(src + 12);
        src += COPY_STEP;
        dst += COPY_STEP;
    } while (src != arg0 + COPY_WORDS_END);
    *(s32 *)dst = *(s32 *)src;
    {
        u8 *base = D_8005F38C;
        s32 v1 = *(s32 *)(base + 0xA8);
        s32 a0 = *(s32 *)(base + 0x70);
        u8 *a2 = *(u8 **)(base + 0x58);
        *(u8 **)(base + 0x18) = D_800608CC;
        *(s32 *)(base + 0x110) = 0;
        *(s32 *)(base + 0x84) = 0;
        *(s32 *)(base + 0x88) = 0;
        *(s32 *)(base + 0x8C) = 0;
        *(s32 *)(base + 0x104) = 0;
        *(s32 *)(base + 0xA8) = v1;
        *(s32 *)(base + 0x94) = a0;
        if (a2 != 0) {
            *(s32 *)(a2 + 0x58) = 0;
            *(s32 *)(base + 0x58) = 0;
        }
    }
    D_80060944 = 0;
    D_8006CB78 = 0;
    D_8005F3B4 = 0;
    D_8006CB80 = INIT_ARG_FLAGS;
    if (!isSpecial) {
        D_8006CB84 += STEP_AMOUNT;
        if (*(s32 *)(arg0 + 0x34) != 0) {
            D_8006CD20 = 0;
        } else {
            D_8006CD20 += STEP_AMOUNT;
        }
    }
    func_8002F228(0);
    D_8006CC68 = *(s32 *)(arg0 + 0x103C);
}
