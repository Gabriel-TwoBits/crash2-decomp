#include "common.h"
#include "sdk.h"
#include "game.h"

#define STATE_FLUSH 2
#define RESULT_DONE -0xFF

extern s32 D_8005F3EC;
extern s32 D_800654E8;
extern s32 DrawSync(s32);
extern s32 func_800363D0(void);

s32 func_800162FC(void) {
    s32 i;

    i = 0;
    func_800163B4(D_80063458[0]);
    DrawSync(0);
    func_800116C0(D_800654E8);
    if (D_8006344C.count > 0) {
        do {
            func_800116C0(D_8006344C.e[i].pad4[0]);
            i++;
        } while (i < D_8006344C.count);
    }
    if (D_8005F3EC == STATE_FLUSH) {
        func_800363D0();
    }
    return RESULT_DONE;
}
