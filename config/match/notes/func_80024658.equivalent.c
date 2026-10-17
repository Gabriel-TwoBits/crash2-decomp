#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_WORDS 4
#define FIRST_SLOT 1
#define SECOND_SLOT 2
#define QUERY_KIND 0x29
#define LIST_WORDS 28
#define SKIP_TYPE 2

s32 func_8002433C(s32 *, s32 *, s32 *, s32 *, s32 *);
s32 func_80024A48(void *, s32, s32 *, s32, void *, s32, s32 *);
s32 func_80024B6C(void *, s32, s32 *, s32, void *, s32, s32 *);
s32 func_800253B0(void *, s32, s32 *, s32, s32, s32, s32 *);
s32 func_800254D4(void *, s32, s32 *, s32, s32, s32, s32 *);

s32 func_80024658(s32 *arg0, s32 *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    /* The candidate list, the result words and the saved arguments share one stack area; a long list runs into them. */
    struct {
        s32 list[LIST_WORDS];
        s32 out;
        s32 altA;
        s32 altB;
        s32 altC;
        s32 a0;
        s32 pad0;
        s32 a2;
        s32 pad1;
        s32 a3;
        s32 pad2;
        s32 type;
        s32 pad3;
        s32 slot;
    } f;
    s32 *entry;
    s32 *cand;
    s32 kind;
    s32 score;
    s32 other;
    s32 count;
    s32 i;
    s32 hi;
    s32 lo;

    f.a0 = (s32)arg0;
    f.a2 = arg2;
    f.a3 = arg3;
    f.slot = arg5;
    entry = arg0 + (f.slot - FIRST_SLOT) * ENTRY_WORDS;
    kind = func_800242F8(func_800321D4(entry[1], QUERY_KIND, 0, 0, 0, SKIP_TYPE, 0));
    f.type = kind;
    if (f.type != SKIP_TYPE) {
        hi = func_8003D360(arg4);
        lo = func_8003D2EC(arg4);
    }
    if (f.slot == FIRST_SLOT) {
        if (f.a3 != 0) {
            score = func_800253B0(entry, (s32)arg1, (s32 *)f.a2, hi, lo, f.type, &f.out);
        } else {
            score = func_80024A48(entry, (s32)arg1, (s32 *)f.a2, hi, (void *)lo, f.type, &f.out);
        }
    } else if (f.a3 != 0) {
        score = func_800254D4(entry, (s32)arg1, (s32 *)f.a2, hi, lo, f.type, &f.out);
    } else {
        score = func_80024B6C(entry, (s32)arg1, (s32 *)f.a2, hi, (void *)lo, f.type, &f.out);
    }
    if (arg5 == FIRST_SLOT) {
        f.slot = SECOND_SLOT;
        entry += ENTRY_WORDS;
        arg0[4] = arg0[0];
        arg0[5] = arg0[1];
        arg0[7] = arg0[3];
    }
    entry[2] = f.out;
    if (score == 0) {
        return f.slot;
    } else if (arg5 != FIRST_SLOT) {
        return f.slot;
    } else {
        count = func_800267C4(f.list, arg0);
        cand = f.list;
        if (count == 0) {
            return f.slot;
        }
        for (i = 0; i < count; i++, cand += ENTRY_WORDS) {
            kind = func_800242F8(func_800321D4(cand[1], QUERY_KIND, 0, 0, 0, SKIP_TYPE, 0));
            if (f.type == kind) {
                if (f.a3 != 0) {
                    other = func_800254D4(cand, (s32)arg1, (s32 *)f.a2, hi, lo, kind, &f.out);
                } else {
                    other = func_80024B6C(cand, (s32)arg1, (s32 *)f.a2, hi, (void *)lo, kind, &f.out);
                }
            } else if (kind == 0) {
                continue;
            } else {
                f.a3 = func_8002433C(cand, arg1, &f.altA, &f.altB, &f.altC);
                if (f.a3 == 0) {
                    arg0[4] = cand[0];
                    arg0[5] = cand[1];
                    arg0[6] = cand[2];
                    arg0[7] = cand[3];
                    return f.slot;
                }
                if (kind != SKIP_TYPE) {
                    hi = func_8003D360(f.altC);
                    lo = func_8003D2EC(f.altC);
                }
                if (f.a3 < 0) {
                    other = func_800254D4(cand, (s32)arg1, (s32 *)f.altB, hi, lo, kind, &f.out);
                } else {
                    other = func_80024B6C(cand, (s32)arg1, (s32 *)f.altA, hi, (void *)lo, kind, &f.out);
                }
            }
            if (score < 0 || (other >= 0 && other < score)) {
                cand[2] = f.out;
                score = other;
                arg0[4] = cand[0];
                arg0[5] = cand[1];
                arg0[6] = cand[2];
                arg0[7] = cand[3];
            }
        }
    }
    return f.slot;
}
