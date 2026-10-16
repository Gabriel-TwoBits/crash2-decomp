#include "common.h"
#include "sdk.h"
#include "game.h"

s32 _card_read(s32, s32, u8 *);

s32 func_80036414(s32 arg0) {
    u8 buf[0x80];

    bzero(buf, 0x80);
    func_80035C74();
    _new_card();
    _card_read(arg0, 0, buf);
    if (func_80035C40() == 0) {
        if (buf[0] == 0x4D) {
            return buf[1] == 0x43;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}
