#include "common.h"
#include "sdk.h"
#include "game.h"

char D_8005F198[] = "[%X]";
char D_8005F1A0[] = "p%d";

extern s32 D_8005F17C;
extern void *D_80062A2C;
extern void *D_80062A30;
extern void *D_80062A34;
extern void *D_80062A38;

void func_80011E24(void) {
    if (D_8005F17C == 0) {
        D_80062A2C = &func_80042420;
        D_80062A30 = &func_800426A0;
        D_80062A34 = &func_80042548;
        D_80062A38 = &func_80042818;
    }
}

void func_80011E80(void) {
}

void func_80011E88(void) {
}

void func_80011E90(void) {
}

extern char D_8005F3F4[];
extern char D_8005F198[];
extern unsigned char D_8005B198[];

/* An odd value becomes five characters of D_8005B198's 64, six bits each from the top, the low bit dropped; an even one is
   printed with the format at D_8005F198. Returns the static buffer */
char *func_80011E98(unsigned int value)
{
    int i;

    if (value & 1) {
        D_8005F3F4[5] = 0;
        value >>= 1;
        for (i = 4; i >= 0; i--) {
            D_8005F3F4[i] = D_8005B198[value & 0x3F];
            value >>= 6;
        }
    } else {
        sprintf(D_8005F3F4, D_8005F198, value);
    }
    return D_8005F3F4;
}

extern char D_8005F1A0[];
extern char D_8005F404[];

char *func_80011F1C(s32 arg0) {
    if (arg0 & 1) {
        sprintf(D_8005F404, D_8005F1A0, arg0 >> 1);
    } else {
        sprintf(D_8005F404, D_8005F198, arg0);
    }
    return D_8005F404;
}

INCLUDE_ASM("asm/nonmatchings/game/80011E24", func_80011F78);
