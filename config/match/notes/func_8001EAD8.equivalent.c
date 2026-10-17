#include "common.h"
#include "sdk.h"
#include "game.h"

#define MATRIX_ONE 0x1000
#define CALL_RESULT -0xFF

extern s32 D_80060808;
extern s32 D_80060914;
extern void MulMatrix(s16 *, s16 *);

/* Retail returns CALL_RESULT; the declared signature is void. */
void func_8001EAD8(s32 *arg0, s32 *arg1) {
    s16 a[9];
    s16 b[9];
    s32 s;
    s32 c;

    s = func_8003D2EC(-D_80060808);
    c = func_8003D360(-D_80060808);
    a[0] = c; a[1] = -s; a[2] = 0;
    a[3] = s; a[4] = c;  a[5] = 0;
    a[6] = 0; a[7] = 0;  a[8] = MATRIX_ONE;

    s = func_8003D2EC(-D_80060914);
    c = func_8003D360(-D_80060914);
    b[0] = c; b[1] = 0;          b[2] = s;
    b[3] = 0; b[4] = MATRIX_ONE; b[5] = 0;
    b[6] = -s; b[7] = 0;         b[8] = c;

    MulMatrix(a, b);

    /* b = transpose of a */
    b[0] = a[0]; b[1] = a[3]; b[2] = a[6];
    b[3] = a[1]; b[4] = a[4]; b[5] = a[7];
    b[6] = a[2]; b[7] = a[5]; b[8] = a[8];
    func_80018194(arg0, b, arg1);
}
