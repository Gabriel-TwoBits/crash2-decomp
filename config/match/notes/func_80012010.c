#include "common.h"
#include "sdk.h"
#include "game.h"

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
