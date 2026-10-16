#include "common.h"
#include "sdk.h"
#include "game.h"
typedef struct { char c[6]; } S6;
extern char D_8005F218[];
extern char D_80067588[];
extern s32 D_8006758C, D_800675AC, D_8006CBB4, D_8006CD14, D_8005F3EC;
s32 func_80011F78(char *);
s8 func_800120EC(s32);
s32 func_80012294(char *, s32);
void func_8002FE58(s32 *arg0) {
    char buf[8];
    s32 r = func_80012294(D_80067588, 0x13);
    s32 t;
    *(S6 *)buf = *(S6 *)D_8005F218;
    memset(buf + 6, 0, 2);
    if (r == 0) {
        t = D_8005F3EC;
        *arg0 = 0x6396347F;
        if (t != 2) {
            D_8006CBB4 = 0;
        }
    } else {
        buf[2] = func_80031578(r) + 0x30;
        buf[3] = func_800120EC(D_8006758C);
        *arg0 = func_80011F78(buf);
        D_800675AC = -1;
        D_8006CD14 = 3;
    }
}
