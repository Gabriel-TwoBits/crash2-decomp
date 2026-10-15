#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { s32 n; u8 pad[0x28C]; s32 *p[1]; } Unk8006CB6C;
extern s32 D_8006CB68;
extern Unk8006CB6C D_8006CB6C;

void func_8002CA1C(void) {
    s32 v;
    if (D_8006CB6C.n != 0) {
        D_8006CB6C.n -= 1;
        v = D_8006CB68;
        switch (*D_8006CB6C.p[D_8006CB6C.n] & 7) {
        case 0:
            v -= 0x20;
            break;
        case 1:
            v -= 0x44;
            break;
        }
        D_8006CB68 = v;
    }
}
