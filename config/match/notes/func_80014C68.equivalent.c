#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_A_OFFSET 0x2C
#define ENTRY_B_OFFSET 0x3CC
#define ENTRY_A_MAX 0x15
#define ENTRY_B_COUNT 0x25
#define ENTRY_A_SHIFT 16
#define DISPATCH_COUNT 0x16
#define FILE_TABLE_COUNT 0x100
#define FILE_ENTRIES_OFFSET 0x520
#define FILE_TABLE2_OFFSET 0x408
#define FILE_COUNT_OFFSET 0x404
#define FILE_SIZE_BASE 0x714
#define FILE_SIZE_STRIDE 0x18
#define FILE_WORDS_OFFSET 0x400
#define WORD_FILL -0x12
#define ADDR_MASK 0xFFFFFF
#define SLOT_COUNT 16
#define SLOT_NO_BUFFER 0x1E
#define SLOT_NO_BUFFER_TRY 0x15
#define SLOT_BUFFER 0x14
#define SLOT_IDLE 2
#define SLOT_ACTIVE_STATE 1
#define NO_ENTRY_LIMIT -0xFE
#define RET_DONE -0xFF
#define SLOT_QUAD_SPLIT 4
#define SLOT_HALF 8
#define CMD_FLAG_HI 0x10
#define CMD_FLAG_ODD 0x80
#define CMD_BASE 0x80000002
#define LEVEL_9 9
#define LEVEL_3C 0x3C
#define SCALE_DEFAULT 0x10000
#define SCALE_HALF 0x8000
#define SCALE_3C 0xEA60
#define SCALE_UNSET -1
#define SPAN_MUL 0xFF90
#define SPAN_BASE 0x1110
#define SPAN_3C 0x3EE40
#define SPAN_ARG 0xE

typedef struct Unk80014C68Entry {
    s32 v;
    s16 a;
    s16 b;
    u8 pad08[4];
    s16 c;
    u8 pad0E;
    s8 d;
    u8 pad10[0x1C];
} Unk80014C68Entry;

typedef struct Unk80014C68 {
    s32 init;
    s32 level;
    s32 *tbl8;
    s32 tblC;
    s32 tbl10;
    s32 *ptr14;
    s32 file;
    u8 pad1C[0xC];
    s32 countA;
    u8 pad2C[0xA28 - 0x2C];
    s32 f_A28;
    s32 f_A2C;
    u8 padA30[4];
    s32 f_A34;
    s32 f_A38;
    s32 f_A3C;
    s32 f_A40;
} Unk80014C68;

typedef struct Unk800125F8Slot {
    s32 unk0;
    u32 unk4;
} Unk800125F8Slot;

typedef struct Unk800125F8Child {
    u32 unk_0;
    u32 unk_4;
    s32 unk_8;
    s32 unk_C;
    s32 unk_10;
} Unk800125F8Child;

typedef struct Unk800125F8Obj {
    u16 unk_0;
    u16 unk_2;
    u32 unk_4;
    u32 unk_8;
    s32 unk_C;
    Unk800125F8Child *unk_10[1];
} Unk800125F8Obj;

typedef struct Unk800125F8Entry {
    Unk800125F8Obj *unk_0;
    s16 unk_4;
    u8 unk_6[6];
    s16 unk_C;
    u8 unk_E;
    u8 unk_F;
    u8 unk_10[4];
    s32 unk_14;
    u32 unk_18;
    s32 unk_1C;
    Unk800125F8Slot *unk_20;
    struct Unk800125F8Entry *unk_24;
    u8 unk_28[4];
} Unk800125F8Entry;

typedef struct Unk8005B1E0 {
    void (*first)(void);
    void (*second)(void);
    u8 pad08[0x14];
} Unk8005B1E0;

extern void func_8001214C(s32, s32);
typedef char *(*NameLookupFn)(s32, s32);
/* The declared prototype returns void; the callee leaves a string pointer in v0. */
#define func_8001214C_str(a, b) (((NameLookupFn)func_8001214C)((a), (b)))
extern s32 func_80014B90(s32, s32);
extern s32 func_80015344(Unk80014C68 *, s32, s32, s32, s32);
extern int printf(const char *, ...);
extern char D_80010110[];
extern Unk8005B1E0 D_8005B1E0[];
extern s32 D_8005F38C;
extern s32 D_8005F3EC;
extern s32 D_8005F3F0;
extern Unk800125F8Entry D_80062D6C[];
extern s32 D_800675A8;
extern s32 D_8006903C[];

s32 func_80014C68(Unk80014C68 *arg0, s32 arg1) {
    char name[0x100];
    s32 i;
    s32 count;
    s32 file;
    s32 *p;
    s32 a0;
    s32 a2;
    s32 cmd;
    s32 ent;
    s32 mapped;
    s32 hi;
    s32 lo;
    s32 scale;
    s32 mode;
    s32 span;
    s32 num;
    Unk80014C68Entry *e;

    if (arg0->init != 0) {
        return RET_DONE;
    }
    D_8005F3F0 = -1;
    D_8005F38C = 0;
    D_8005F3EC = arg1;
    arg0->init = 1;
    arg0->f_A2C = 0;
    arg0->f_A28 = 0;
    arg0->f_A34 = 0;
    arg0->level = arg1;
    arg0->f_A38 = 0;
    arg0->f_A40 = 1;
    strcpy(name, func_8001214C_str(0, arg1));
    if (D_8005F1A4 != 0) {
        a0 = (s32)&D_80069034[arg1 * 3];
        a2 = 1;
    } else {
        a0 = (s32)name;
        a2 = 0;
    }
    arg0->file = func_800123F0((s32 *)a0, 0, a2);
    file = arg0->file;
    arg0->tbl10 = arg0->file + FILE_TABLE2_OFFSET;
    arg0->tbl8 = (s32 *)arg0->file;
    arg0->tblC = arg0->file + FILE_ENTRIES_OFFSET;
    arg0->ptr14 = (s32 *)(file + *(s32 *)(file + FILE_COUNT_OFFSET) * 8 + FILE_ENTRIES_OFFSET);
    file = arg0->file;
    arg0->file = func_800116D4(file, *(s32 *)(file + FILE_COUNT_OFFSET) * 8 + (*arg0->ptr14 - 1) * FILE_SIZE_STRIDE + FILE_SIZE_BASE);
    i = 0;
    do {
        p = &arg0->tbl8[i];
        i++;
        *p = arg0->tblC + *p * 8;
    } while (i < FILE_TABLE_COUNT);
    strcpy(name, func_8001214C_str(1, arg1));
    if (D_8005F1A4 != 0) {
        D_800675A8 = D_8006903C[arg1 * 3] & ADDR_MASK;
    }
    arg0->f_A3C = func_8001148C(*(s32 *)(arg0->file + FILE_WORDS_OFFSET) * 4);
    i = 0;
    if (*(s32 *)(arg0->file + FILE_WORDS_OFFSET) > 0) {
        do {
            ((s32 *)arg0->f_A3C)[i] = WORD_FILL;
            i++;
        } while (i < *(s32 *)(arg0->file + FILE_WORDS_OFFSET));
    }
    for (i = 0; i < DISPATCH_COUNT; i++) {
        if (D_8005B1E0[i].first != 0) {
            D_8005B1E0[i].first();
        }
    }
    arg0->countA = ENTRY_A_MAX;
    count = ENTRY_A_MAX;
    do {
        *(s32 *)((u8 *)arg0 + ENTRY_A_OFFSET) = func_8001148C(count << ENTRY_A_SHIFT);
        if (*(s32 *)((u8 *)arg0 + ENTRY_A_OFFSET) != 0) {
            break;
        }
        count--;
    } while (count >= 0);
    arg0->countA = count;
    num = 0;
    i = 0;
    e = (Unk80014C68Entry *)((u8 *)arg0 + ENTRY_A_OFFSET);
    if (arg0->countA > 0) {
        do {
            e->b = 1;
            e->c = i;
            e->d = 0;
            e->a = 1;
            e->v = *(s32 *)((u8 *)arg0 + ENTRY_A_OFFSET) + (i << ENTRY_A_SHIFT);
            num++;
            i++;
            e++;
        } while (i < arg0->countA);
    }
    *(s32 *)((u8 *)arg0 + ENTRY_B_OFFSET - 4) = ENTRY_B_COUNT;
    i = 0;
    e = (Unk80014C68Entry *)((u8 *)arg0 + ENTRY_B_OFFSET);
    do {
        e->a = 1;
        e->b = 0;
        e->v = 0;
        e->d = 0;
        i++;
        e++;
    } while (i < *(s32 *)((u8 *)arg0 + ENTRY_B_OFFSET - 4));
    printf(D_80010110, num, func_800120EC(arg1) & 0xFF);
    i = 0;
    do {
        Unk800125F8Entry *slot = &D_80062D6C[i];

        *(s16 *)&slot->unk_6[0] = SLOT_IDLE;
        hi = i >> 2;
        lo = i - hi * 4;
        cmd = lo * 4;
        if (i >= SLOT_HALF) {
            cmd |= CMD_FLAG_HI;
        }
        if (hi & 1) {
            cmd |= (lo << 8) | (hi << 17) | CMD_FLAG_ODD;
        } else {
            cmd |= (lo << 8) | (hi << 17);
        }
        cmd |= CMD_BASE;
        slot->unk_1C = cmd;
        mapped = SLOT_NO_BUFFER;
        if ((u32)i >= SLOT_QUAD_SPLIT && (u32)(i - SLOT_QUAD_SPLIT) >= SLOT_QUAD_SPLIT) {
            mapped = SLOT_ACTIVE_STATE;
            if ((u32)((u16)slot->unk_4 - SLOT_BUFFER) < 2) {
                ent = func_80014B90(D_80062D84[i].v, hi);
                if (ent < NO_ENTRY_LIMIT) {
                    D_80062D70[i].state = SLOT_BUFFER;
                    D_80062D80[i].v = *(s32 *)ent;
                    D_80062D8C[i].v = (s32 *)ent;
                    slot->unk_F = 0;
                    slot->unk_E = 0;
                    D_80067FC4[D_80062D80[i].v >> 1] = (s32)slot;
                    *(s32 *)ent = slot->unk_1C;
                    continue;
                }
                D_80062D70[i].state = SLOT_NO_BUFFER_TRY;
                slot->unk_F = 0;
                slot->unk_E = 0;
                continue;
            }
        }
        D_80062D70[i].state = mapped;
    } while (++i < SLOT_COUNT);
    scale = SCALE_DEFAULT;
    mode = SCALE_UNSET;
    span = func_80012294((char *)arg0, SPAN_ARG) * SPAN_MUL - SPAN_BASE;
    switch (arg1) {
    case LEVEL_9:
        scale = SCALE_HALF;
        mode = 2;
        break;
    case 2:
    case 7:
    case 0x1C:
    case 0x28:
    case 0x29:
    case 0x3B:
        scale = SCALE_DEFAULT;
        mode = 2;
        break;
    case LEVEL_3C:
        scale = SCALE_3C;
        mode = 1;
        span = SPAN_3C;
        break;
    default:
        break;
    }
    func_80015344(arg0, 0, span, scale, mode);
    for (i = 0; i < DISPATCH_COUNT; i++) {
        if (D_8005B1E0[i].second != 0) {
            D_8005B1E0[i].second();
        }
    }
    return RET_DONE;
}
