#include "common.h"
#include "sdk.h"
#include "game.h"

#define STATUS_DONE 5
#define SEARCH_FOUND 1

s32 func_800267C4(s32 *, s32 *, s32, s32);
extern s8 D_8005B994;
extern s32 D_800608CC[];

/* Word indices into D_800608CC. */
#define IDX_A 0
#define IDX_B 2
#define IDX_C 5
#define IDX_EXPECTED 9
#define IDX_D 11

#define ARG_41 0x29
#define ARG_75 0x4B
#define ARG_FLAG 2
#define SHIFT_8 8

void func_80022F58(s32 *arg0) {
    s32 out[3];
    s32 in[4];

    do {
        in[0] = D_800608CC[IDX_A];
        in[1] = D_800608CC[IDX_B];
        in[2] = D_800608CC[IDX_C];
        in[3] = D_800608CC[IDX_D];
        if (func_800267C4(out, in, D_800608CC[IDX_C], D_800608CC[IDX_D]) == SEARCH_FOUND) {
            func_800200A8(out[0], out[1], out[2], 0);
            if (func_800321D4(out[1], ARG_41, 0, 0, 0, ARG_FLAG, 0) != D_800608CC[IDX_EXPECTED]) {
                D_8005B994 = STATUS_DONE;
            }
        } else {
            func_800200A8(D_800608CC[IDX_A], D_800608CC[IDX_B],
                          (func_800322C0(D_800608CC[IDX_B], ARG_75, 0, ARG_FLAG) - 1) << SHIFT_8, 0);
            D_8005B994 = STATUS_DONE;
        }
        func_800231B0(arg0);
    } while (D_8005B994 != STATUS_DONE);
}
