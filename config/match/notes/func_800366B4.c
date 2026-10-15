#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 _card_write(s32, s32, u8 *);
extern void _new_card(void);
extern void bzero(u8 *, s32);
extern void func_80035C74(void);
extern s32 func_80035C40(void);

s32 func_800366B4(void) {
    u8 buf[0x80];

    bzero(buf, 0x80);
    buf[1] = 0xFF;
    buf[0] = 0xFF;
    func_80035C74();
    _new_card();
    _card_write(0, 0, buf);
    return -(func_80035C40() != 0);
}
