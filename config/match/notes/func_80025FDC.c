#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    s32 count;
    u8 pad[0x74];
    s32 e[8][4];
} Unk80025FDC;

extern Unk80025FDC D_8005B9E0;

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
