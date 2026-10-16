#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct Unk80013BD8 {
    u8 pad0[4];
    s16 state;
    s16 f_6;
    u8 pad8[0xC];
    s32 f_14;
    u8 pad18[0x14];
} Unk80013BD8;
typedef struct Unk80013BD8Table {
    s32 count;
    Unk80013BD8 e[1];
} Unk80013BD8Table;
typedef struct Unk80067950 {
    s32 count;
    Unk80013BD8 e[37];
    s32 active;
    s32 flag;
} Unk80067950;
extern Unk80067950 D_80067950;
extern Unk80013BD8Table D_800675B0;
extern u8 *D_800675A0;
extern s32 *D_80067FC4;
s32 D_8005F1A4 = 1;
extern u8 D_80067954[];
extern u8 D_80067968[];
void func_80013200(Unk80013BD8 *, s32);
void func_80013C98(void *, s32, s32, s32 *, s32);

void func_80012BC4(s32 arg0) {
    s32 sp18;
    s32 key;
    s32 found;
    s32 i;
    s32 v;
    s32 lim;
    s32 idx;
    s32 off;
    Unk80013BD8 *p;

    if (D_80067950.active != 0) {
        found = -1;
        if (D_80067950.flag == 0) {
            for (i = 0; i < D_80067950.count; i++) {
                p = &D_80067950.e[i];
                if (p->state == 2) {
                    v = p->f_14;
                    if ((arg0 == -1 && (found == arg0 || v < key)) || v == arg0) {
                        key = v;
                        found = i;
                        if (key == arg0) break;
                    }
                }
            }
            if (found != -1) {
                sp18 = 1;
                if (D_8005F1A4 != 0) {
                    idx = key >> 1;
                    lim = *(s32 *)(D_800675A0 + 0x400) - 1;
                    if (idx < lim) {
                        s32 *tbl = D_80067FC4;
                        s32 nxt = idx + 1;
                        do {
                            idx = (nxt * 2) >> 1;
                            v = tbl[idx];
                            if (v >= -0xFE) break;
                            if (((Unk80013BD8 *)v)->f_6 != 0) break;
                            sp18++;
                            nxt = idx + 1;
                        } while (idx < lim);
                    }
                }
                off = found * 0x2C;
                func_80013C98(D_80067954 + off, *(s32 *)(D_80067968 + off), 0x6396347F, &sp18, ~arg0 != 0);
            }
        }
        for (i = 0; i < D_800675B0.count; i++) {
            func_80013200(&D_800675B0.e[i], arg0);
        }
    }
}
