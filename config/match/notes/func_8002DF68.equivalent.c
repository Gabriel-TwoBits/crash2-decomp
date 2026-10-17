#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 *D_8005F3C4;

#define BIT_COUNT 32
#define TOP_BIT 0x80000000U

s32 func_8002DF68(u32 arg0, u32 arg1) {
    s32 result;

    result = 0;
    if ((arg0 < BIT_COUNT) && (arg1 < BIT_COUNT)) {
        result = (D_8005F3C4[arg1] & (TOP_BIT >> arg0)) == 0;
    } else {
        /* out of range: result stays 0 */
    }
    return result;
}
