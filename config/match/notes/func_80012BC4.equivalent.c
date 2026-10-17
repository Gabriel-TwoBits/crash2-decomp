#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80013BD8 {
    u8 pad0[4];
    s16 state;
    u8 pad6[8];
    s8 f_e;
    s8 f_f;
    u8 pad10[4];
    s32 f_14;
    s32 f_18;
    u8 pad1c[0xC];
    s32 f_28;
} Unk80013BD8;
typedef struct Unk80013BD8Table {
    s32 count;
    Unk80013BD8 e[1];
} Unk80013BD8Table;

#define CHANNEL_TABLE_LIMIT_OFFSET 0x400
#define SLOT_STATE_IN_USE 2
#define SLOT_KEY_ANY -1
#define CHANNEL_MARKER_MAX -0xFE
#define CALL_MAGIC 0x6396347F

extern s32 D_80067FB0[];
extern int D_80067FB4[];
extern Unk80013BD8Table D_80067950;
extern Unk80013BD8Table D_800675B0;
extern u8 *D_800675A0;
extern s32 *D_80067FC4;
s32 D_8005F1A4 = 1;
void func_80013200(Unk80013BD8 *, s32);

void func_80012BC4(s32 arg0) {
    s32 sp18;
    s32 key;
    s32 found;
    s32 i;
    s32 v;
    s32 lim;
    s32 idx;
    s32 nxt;
    s32 *tbl;
    Unk80013BD8 *p;

    if (D_80067FB0[0] != 0) {
        found = -1;
        if (D_80067FB4[0] == 0) {
            for (i = 0; i < D_80067950.count; i++) {
                p = &D_80067950.e[i];
                if (p->state == SLOT_STATE_IN_USE) {
                    v = p->f_14;
                    if ((arg0 == SLOT_KEY_ANY && (found == arg0 || v < key)) || v == arg0) {
                        key = v;
                        found = i;
                        if (key == arg0) {
                            break;
                        }
                    }
                }
            }
            if (found != -1) {
                sp18 = 1;
                if (D_8005F1A4 != 0) {
                    idx = key >> 1;
                    lim = *(s32 *)(D_800675A0 + CHANNEL_TABLE_LIMIT_OFFSET) - 1;
                    if (idx < lim) {
                        tbl = D_80067FC4;
                        nxt = idx + 1;
                        do {
                            idx = (nxt * 2) >> 1;
                            v = tbl[idx];
                            if (v >= CHANNEL_MARKER_MAX) {
                                break;
                            }
                            if (((s16 *)v)[3] != 0) {
                                break;
                            }
                            sp18++;
                            nxt = idx + 1;
                        } while (idx < lim);
                    }
                }
                p = &D_80067950.e[found];
                func_80013C98(p, p->f_14, CALL_MAGIC, &sp18, ~arg0 != 0);
            }
        }
        for (i = 0; i < D_800675B0.count; i++) {
            func_80013200(&D_800675B0.e[i], arg0);
        }
    }
}
