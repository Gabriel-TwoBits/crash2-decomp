#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct {
    u16 w[4];
} Ent;

#define TBL_COUNT_OFFSET 0x1004
#define TBL_X_OFFSET 0x1008
#define TBL_Y_OFFSET 0x100C
#define TBL_Z_OFFSET 0x1010
#define TBL_FIELD(off) (*(s32 *)(D_80067FE4 + (off)))

/* Multiplies by 4 with 32-bit wrap-around, then divides by 8192 truncating. */
#define SCALE_SHIFT 2
#define SCALE_DIVISOR 8192
#define SCALE(x) (((s32)((u32)(x) << SCALE_SHIFT)) / SCALE_DIVISOR)

extern u8 D_80067FE4[];
extern s32 D_80068FE8;

s32 func_8002DFA8(s32 arg0, s32 arg1, s32 *arg2, void *arg3) {
    s32 a[3];
    s32 b[3];
    s32 d[3];
    s32 sp40;
    u16 s1, fp, s7, s6;
    s32 i;
    s32 cnt;
    u32 t;
    s32 kind;
    Ent *e;
    u16 m1, m2;
    s32 v;

    sp40 = arg0;
    s1 = 0; fp = 0; s7 = 0; s6 = 0; d[0] = 0; d[1] = 0; d[2] = 0;
    i = 0;
    cnt = 0;
    if (D_80068FE8 > 0) {
        do {
            e = (Ent *)D_80067FE4 + i;
            if (e->w[0] == 0) {
                d[0] = e->w[1] << 8;
                d[1] = e->w[2] << 8;
                i++;
                d[2] = e->w[3] << 8;
                e = (Ent *)D_80067FE4 + i;
                fp = e->w[1];
                s7 = e->w[2];
                s6 = e->w[3];
            } else {
                t = (e->w[0] >> 2) | 1;
                if (t != 0) {
                    kind = ((t & 0xE) >> 1) + 1;
                } else {
                    kind = 0;
                }
                if (kind != 5 && kind != 0) {
                    switch (arg1) {
                    case 0:
                        if (kind != 2 && kind != 6) {
                            goto next;
                        }
                        break;
                    case 1:
                        if (kind == 2 || kind == 6) {
                            goto next;
                        }
                        break;
                    default:
                        break;
                    }
                    m1 = s7;
                    a[1] = TBL_FIELD(TBL_Y_OFFSET) + (s16)e->w[2] * 16 - arg2[1];
                    if (s1 < m1) {
                        m1 = s1;
                    }
                    v = a[1] + (d[1] >> m1);
                    b[1] = v;
                    switch (arg1) {
                    case 0:
                        if (a[1] > 0x6400) goto next;
                        if (v < (s32)0xFFFE7000) goto next;
                        break;
                    case 1:
                        if (v < 0) goto next;
                        break;
                    default:
                        break;
                    }
                    a[0] = TBL_FIELD(TBL_X_OFFSET) + (s16)e->w[1] * 16 - arg2[0];
                    a[2] = TBL_FIELD(TBL_Z_OFFSET) + (s16)e->w[3] * 16 - arg2[2];
                    s1 = e->w[0] & 7;
                    m1 = fp;
                    if (s1 < m1) m1 = s1;
                    m2 = s6;
                    b[0] = a[0] + (d[0] >> m1);
                    if (s1 < m2) m2 = s1;
                    b[2] = a[2] + (d[2] >> m2);
                    func_8002CB68(SCALE(a[0]), SCALE(a[2]), SCALE(b[0]), SCALE(b[2]), sp40);
                    cnt++;
                }
            }
next:
            i++;
        } while (i < TBL_FIELD(TBL_COUNT_OFFSET));
    }
    return cnt;
}
