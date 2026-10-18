#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "common.h"
#include "game/nsf.h"

/* The game object: what the GOOL interpreter's operands (func_80036D0C, func_80038D08) and the object code read. Each
   field has the width and signedness the code reads it with. The size isn't known: the code reads up to 0x16C
   (func_8001D63C), and func_8001EE64 sets up a pool of 0x60 entries 0x23C bytes apart, linked through 0x44, 0x48 and
   0x4C as objects are.

   0x40 to 0xBB are also read as words by index, the interpreter's registers: ((s32 *)obj->unk_40)[index], the index
   reaching past the end of the structure (& 0x1FF in func_80038D08). */
typedef struct GoolObject {
    /* 0x00 */ s32 unk_0;
    /* 0x04 */ s32 unk_4;
    /* 0x08 */ void *unk_8;
    /* 0x0C */ NsfEntry *unk_C; /* what func_80014260 returns for an NSD record (func_80019988) */
    /* 0x10 */ void *unk_10;
    /* 0x14 */ void *unk_14;
    /* An entry from func_80014260, the one func_800200A8 keeps in D_800608CC (func_8002E884), or an error code
       (func_8002BF58) */
    /* 0x18 */ NsfEntry *unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ s16 unk_20[0x10]; /* halfwords, the block passed by address (obj + 0x20) */
    /* Indexed by 3 bits (func_80036D0C); [0] holds the object itself (func_80019988) */
    /* 0x40 */ struct GoolObject *unk_40[8];
    /* Six vectors: 0x60 and 0x90 read as x, y, z (func_8001CEDC), 0x6C and 0x78 passed by address (func_8001AA80),
       0x84, 0x90 and 0x9C read as a matrix's rows (func_8001ABF8) */
    /* 0x60 */ s32 unk_60[3];
    /* 0x6C */ s32 unk_6C[3];
    /* 0x78 */ s32 unk_78[3];
    /* 0x84 */ s32 unk_84[3];
    /* 0x90 */ s32 unk_90[3];
    /* 0x9C */ s32 unk_9C[3];
    /* 0xA8 */ s32 unk_A8;
    /* 0xAC */ s32 unk_AC;
    /* 0xB0 */ s32 unk_B0;
    /* 0xB4 */ s32 unk_B4;
    /* 0xB8 */ s32 unk_B8;
    /* 0xBC */ s32 *unk_BC; /* pushed and popped as a stack of words by the interpreter's operands */
    /* 0xC0 */ s32 unk_C0;
    /* 0xC4 */ u8 *unk_C4;
    /* 0xC8 */ s32 unk_C8;
    /* 0xCC */ s32 unk_CC;
    /* 0xD0 */ s32 unk_D0;
    /* 0xD4 */ s32 unk_D4;
    /* 0xD8 */ s32 unk_D8;
    /* 0xDC */ u32 unk_DC;
    /* 0xE0 */ u32 unk_E0;
    /* 0xE4 */ s32 unk_E4;
    /* 0xE8 */ u8 *unk_E8;
    /* 0xEC */ u32 unk_EC;
    /* 0xF0 */ void *unk_F0;
    /* 0xF4 */ s32 unk_F4;
    /* 0xF8 */ s32 unk_F8;
    /* 0xFC */ s32 unk_FC;
    /* 0x100 */ s32 unk_100;
    /* 0x104 */ s32 unk_104;
    /* 0x108 */ s32 unk_108;
    /* 0x10C */ u32 unk_10C;
    /* 0x110 */ s32 unk_110;
    /* 0x114 */ s32 unk_114;
    /* 0x118 */ s32 unk_118;
    /* 0x11C */ s32 unk_11C;
    /* 0x120 */ u8 unk_120; /* 0x120 to 0x123 also read as a u32 (func_8001ABF8) and cleared as one (func_80019988) */
    /* 0x121 */ u8 unk_121;
    /* 0x122 */ u8 unk_122;
    /* 0x123 */ u8 unk_123[1];
    /* 0x124 */ s32 unk_124;
    /* 0x128 */ s32 unk_128;
    /* 0x12C */ s32 unk_12C;
    /* 0x130 */ u8 unk_130[4];
    /* 0x134 */ s32 unk_134;
    /* 0x138 */ s32 unk_138;
    /* 0x13C */ s32 unk_13C;
    /* 0x140 */ u8 unk_140[0x28];
    /* 0x168 */ s32 unk_168;
    /* 0x16C */ s32 unk_16C;
} GoolObject;

#endif /* GAME_OBJECT_H */
