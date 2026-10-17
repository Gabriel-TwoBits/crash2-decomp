#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010000);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010010);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010020);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010038);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010044);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010054);

extern u8 D_8005B198[];

s32 func_80012010(s32 arg0) {
    switch (D_8005B198[((u32)arg0 >> 1) & 0x3F]) {
    case 'V': return 1;
    case 'S': return 4;
    case 'G': return 2;
    case 'W': return 3;
    case 'T': return 5;
    case 'L': return 6;
    case 'Z': return 7;
    case 'C': return 0xB;
    case 'A': return 0xC;
    case 'O': return 0x14;
    case 'M': return 0xD;
    case 'I': return 0x10;
    case 'N': return 0xE;
    case 'P': return 0x11;
    case 'U': return 0x12;
    case 'B': return 0x13;
    case 'D': return 0x15;
    case 'X': return 0xF;
    default: return 0;
    }
}

u8 func_800120EC(s32 arg0) {
    return D_8005B198[arg0];
}

extern u8 D_8005B198[];

s32 func_80012100(s32 arg0) {
    s32 i;
    u8 c = arg0;

    for (i = 0; i < 0x40; i++) {
        if (D_8005B198[i] == c) {
            return i;
        }
    }
    if (c == 0x2D) {
        return 0x63;
    }
    return -0xE;
}


void func_8001214C(s32 a0, s32 a1) {
    func_80012170(a0, a1, 0, 0);
}

extern char D_80010000[];
extern char D_80010010[];
extern char D_80010020[];
extern char D_80010038[];
extern char D_80010044[];
extern char D_80010054[];
extern s32 D_8005F1A4;
extern char D_8005F414[];


char *func_80012170(s32 a0, u32 a1, s32 a2, s32 a3) {
    switch (a0) {
    case 0:
        if (D_8005F1A4 != 0) {
            sprintf(D_8005F414, D_80010000, a1 >> 4, a1);
        } else {
            sprintf(D_8005F414, D_80010010, D_80010020, D_80010038, a1);
        }
        break;
    case 1:
        if (D_8005F1A4 != 0) {
            sprintf(D_8005F414, D_80010044, a1 >> 4, a1);
        } else {
            sprintf(D_8005F414, D_80010054, D_80010020, D_80010038, a1);
        }
        break;
    default:
        return (char *)-14;
    }
    return D_8005F414;
}

s32 func_80012250(s32 *arg0) {
    s32 v;
    s32 r;

    v = arg0[0];
    if (v & 3) {
        r = func_80012010(arg0[1]);
    } else {
        r = ((s32 *)v)[2];
    }
    return r;
}

#ifdef NON_MATCHING
#define OWNER_OFFSET 0x18
#define COUNT_OFFSET 0x404
#define ENTRIES_OFFSET 0x520
#define ENTRY_SIZE 8

s32 func_80012294(char *arg0, s32 arg1) {
    s32 i;
    s32 matches;
    s32 offset;
    char *owner;

    matches = 0;
    offset = ENTRIES_OFFSET;
    for (i = 0; i < *(s32 *)((char *)(*(char **)(arg0 + OWNER_OFFSET)) + COUNT_OFFSET); i++) {
        owner = *(char **)(arg0 + OWNER_OFFSET);
        if (func_80012250((s32 *)(owner + offset)) == arg1) {
            matches++;
        }
        offset += ENTRY_SIZE;
    }
    return matches;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80012010", func_80012294);
#endif

#ifdef NON_MATCHING
s32 func_8001232C(s32 *arg0) {
    s32 *b;
    s32 v;
    s32 t = *(u16 *)((u8 *)arg0 + 2);

    b = arg0;
    if (t < 6) {
        if (t < 3 && t != 0) {
            if (t == 1) {
                return 0x10000;
            }
            return -0xE;
        }
        v = b[b[2] + 4];
    } else {
        if (t != 0x100) {
            return -0xE;
        }
        v = arg0[arg0[3] + 4];
    }
    if (v < 0) {
        return v - (s32)b;
    }
    return v;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80012010", func_8001232C);
#endif

s32 func_800123BC(s32 *arg0, s32 arg1) {
    s32 *p;
    s32 r;

    if (arg0[3] == 0) {
        r = 0;
    } else {
        p = arg0 + arg1;
        r = p[5] - p[4];
    }
    return r;
}
