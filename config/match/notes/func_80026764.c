#include "common.h"
#include "sdk.h"

typedef struct { s32 a, b, c, d; } V;
typedef struct { V v; s32 pad[20]; } Buf;
s32 func_800267C4(Buf *, s32);

s32 func_80026764(s32 arg0, V *arg1) {
    Buf buf;

    if (func_800267C4(&buf, arg0) != 0) {
        *arg1 = buf.v;
        return 1;
    }
    return 0;
}
