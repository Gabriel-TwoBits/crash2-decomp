#include "common.h"
#include "sdk.h"

char D_8005F198[] = "[%X]";
char D_8005F1A0[] = "p%d";

INCLUDE_ASM("asm/nonmatchings/game/80011E24", func_80011E24);

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

INCLUDE_ASM("asm/nonmatchings/game/80011E24", func_80011F1C);

INCLUDE_ASM("asm/nonmatchings/game/80011E24", func_80011F78);
