#include "common.h"
#include "sdk.h"
#include "game.h"

extern unsigned char D_8005B198[];

#define PACK_CHARS 5
#define CHAR_BITS 6
#define ALPHABET_SIZE 64
#define PAD_CODE 0x3E

s32 func_80011F78(char *arg0) {
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
            if (D_8005B198[code] == c) {
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
