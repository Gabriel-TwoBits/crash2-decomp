#include "common.h"
#include "sdk.h"
#include "game.h"

s32 D_8005F24C = 0;
extern s32 D_8006CBFC;
extern s32 D_8005F3EC;
s32 D_8005F2F4;
s32 D_8005F39C;
s16 D_8005F3E4;
s16 D_8005F3E0;
s32 D_8005F304;
void func_80033C84(s32);

void func_80033D10(void) {
    D_8005F24C = 1;
    SsSetTickMode(1);
    func_80033C84((D_8006CBFC * 0x3FFF) << 8 >> 16);
    D_8005F2F4 = 0x6396347F;
    D_8005F39C = 0;
    D_8005F3E4 = -1;
    D_8005F3E0 = -1;
    switch (D_8005F3EC) {
    case 2: case 5: case 6: case 7: case 8: case 9:
    case 16: case 18: case 25: case 26: case 32: case 38:
        D_8005F304 = 0x12;
        break;
    case 23: case 24: case 27:
        D_8005F304 = 0x11;
        break;
    case 15:
        D_8005F304 = 0xC;
        break;
    case 1:
    default:
        D_8005F304 = 0x10;
        break;
    }
    SsSetReservedVoice((u8)D_8005F304);
}
