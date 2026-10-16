#include "common.h"
#include "sdk.h"
#include "game.h"

#define F(p, T, off) (*(T *)((u8 *)(p) + (off)))

s32 func_8001D22C(void *, u16, s32, s32 *);
s32 func_80030FEC(s32 *, s32 *);

s32 func_80018E68(void *arg0, void *arg1) {
    s32 sp10;
    s32 d;
    s32 sh;
    s32 new_var;
    s32 t;
    s32 st;
    s32 m;
    s32 s0;
    void *tbl;

    if (arg0 == F(arg1, void *, 4)) {
        return 0;
    }
    sh = F(F(F(arg0, void *, 0xC), void *, 0x10), s32, 4) >> 8;
    if (!((1 << sh) & F(arg1, u32, 0))) {
        return 0;
    }
    d = func_80030FEC((s32 *)((u8 *)arg0 + 0x60), (s32 *)((u8 *)F(arg1, void *, 4) + 0x60));
    if (d < F(arg1, s32, 0xC)) {
        t = F(arg1, s32, 0x10);
        if (t == 0xFF) {
            s0 = 0xFE;
        } else {
            tbl = F(arg0, void *, 0xC);
            s0 = *(u16 *)(F(tbl, u8 *, 0x1C) + (t >> 8) * 2);
            if (s0 == 0xFF) {
                if (t == 0x300) {
                    st = F(arg0, s32, 0x108);
                    new_var = F(arg0, s32, 0xB0);
                    if ((u32)(st - 3) >= 2 && st != 2 && !(new_var & 2)) {
                        s0 = 0xFE;
                        goto cmp;
                    }
                }
                if (F(arg1, s32, 0x10) == 0xA00 && !(F(arg0, s32, 0x100) & 0x800)) {
                    s0 = 0xFE;
                    goto cmp;
                }
                if (F(arg1, s32, 0x10) != 0x1D00) {
                    return 0;
                }
                if (F(arg0, s32, 0x100) & 0x02000000) {
                    return 0;
                }
                s0 = 0xFE;
                goto cmp;
            }
            if (!(s0 & 0x8000)) {
                m = F(arg0, s32, 0xB0);
                st = F(arg0, s32, 0x108);
                t = *(s32 *)(F(tbl, u8 *, 0x20) + s0 * 16);
                if ((u32)(st - 3) < 2 || st == 2) {
                    m |= 0x1002;
                }
                if (m & t) {
                    return 0;
                }
                if (!(s0 & 0x8000)) {
                    goto cmp;
                }
            }
            if (F(arg1, s32, 0x10) == 0xF00) {
                sp10 = 0x100;
                F(arg0, void *, 0x5C) = F(arg1, void *, 4);
                func_8001D22C(arg0, (u16)s0, 1, &sp10);
                if (F(F(arg1, void *, 4), s32, 0xD8) == 0) {
                    return 0;
                }
            }
        }
cmp:
        if (s0 != 0xFF) {
            F(arg1, void *, 8) = arg0;
            F(arg1, s32, 0xC) = d;
        }
    }
    return 0;
}
