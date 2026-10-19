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
extern unsigned char g_EidAlphabet[];

/* An odd value becomes five characters of g_EidAlphabet's 64, six bits each from the top, the low bit dropped; an even one is
   printed with the format at D_8005F198. Returns the static buffer */
char *Nsf_EidToName(unsigned int value)
{
    int i;

    if (value & 1) {
        D_8005F3F4[5] = 0;
        value >>= 1;
        for (i = 4; i >= 0; i--) {
            D_8005F3F4[i] = g_EidAlphabet[value & 0x3F];
            value >>= 6;
        }
    } else {
        sprintf(D_8005F3F4, D_8005F198, value);
    }
    return D_8005F3F4;
}

extern char D_8005F1A0[];
extern char D_8005F404[];

char *Nsf_PageRefName(s32 arg0) {
    if (arg0 & 1) {
        sprintf(D_8005F404, D_8005F1A0, arg0 >> 1);
    } else {
        sprintf(D_8005F404, D_8005F198, arg0);
    }
    return D_8005F404;
}

#ifdef NON_MATCHING
#define PACK_CHARS 5
#define CHAR_BITS 6
#define ALPHABET_SIZE 64
#define PAD_CODE 0x3E

s32 Nsf_NameToEid(char *arg0) {
    s32 packed;
    s32 remaining;
    s32 code;
    s32 c;

    packed = 0;
    for (remaining = PACK_CHARS; remaining != 0; remaining--) {
        c = (u8)*arg0++;
        if (c == 0) {
            /* end of string: pad every remaining slot */
            for (; remaining != 0; remaining--) {
                packed = (packed << CHAR_BITS) | PAD_CODE;
            }
            break;
        }
        for (code = 0; code != ALPHABET_SIZE; code++) {
            if (g_EidAlphabet[code] == c) {
                break;
            }
        }
        if (code == ALPHABET_SIZE) {
            code = PAD_CODE;
        }
        packed = (packed << CHAR_BITS) | code;
    }
    return (packed << 1) | 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80011E24", Nsf_NameToEid);
#endif
