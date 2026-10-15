#include "common.h"
#include "sdk.h"
#include "game.h"

s32 CdDataSync(s32 mode);
s32 CdReadSync(s32 mode, u8 *result);

s32 func_80012ED0(s32 wait) {
    s32 ret;

    ret = CdReadSync(wait == 0, 0);
    if (ret < 0) {
        return -16;
    }
    if (ret != 0) {
        return 0;
    }
    ret = CdDataSync(wait == 0);
    if (ret < 0) {
        return -16;
    }
    return ret == 0;
}
