#include "common.h"
#include "sdk.h"
#include "game.h"

#define ENTRY_STRIDE 8
#define HEADER_COUNT_OFFSET 0xC
#define HEADER_NEXT_OFFSET 8
#define HEADER_ENTRIES_OFFSET 0x10

#define FLAG_SIZES 0x40
#define FLAG_TIMES 0x20
#define FLAG_TYPE_MASK 0x1F

#define MODE_FLOOR 0
#define MODE_EXACT 1
#define MODE_INDEX 2

#define TIME_SHIFT 8
#define LERP_ONE_SHIFT 8
#define LERP_ROUND_SHIFT 7
#define DATA_ALIGN_MASK (-4)
#define DATA_ALIGN_ADD 3

typedef struct Unk80031AE8Entry {
    /* 0x0 */ s16 key;
    /* 0x2 */ u16 unk_02;
    /* 0x4 */ u8 flags;
    /* 0x5 */ u8 unk_05;
    /* 0x6 */ u16 count;
} Unk80031AE8Entry;

#define LERP(a, b, t) ((((((b) - (a)) * (t)) >> LERP_ROUND_SHIFT) + 1) >> 1)

s32 func_80031AE8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 *arg4, s32 arg5, s32 *arg6, s32 arg7, s32 arg8) {
    u8 *list;
    s32 lo;
    s32 hi;
    s32 mid;
    s32 n;
    s32 idx;
    s32 frac;
    s32 denom;
    s32 num;
    s32 skip;
    s32 i;
    s32 t6;
    s32 a;
    s32 b;
    s32 type;
    s32 time;
    Unk80031AE8Entry *entry;
    u16 *sizes;
    s16 *times;
    u8 *data;
    u8 *data2;
    u8 *base;
    u8 *out;

    list = (u8 *)arg0;
    while (1) {
        n = *(s16 *)(list + HEADER_COUNT_OFFSET);
        entry = 0;
        if (n > 0) {
            lo = 0;
            hi = n - 1;
            mid = hi;
            while (1) {
                mid = (lo + hi) >> 1;
                entry = (Unk80031AE8Entry *)(list + HEADER_ENTRIES_OFFSET + mid * ENTRY_STRIDE);
                if (arg1 == entry->key) {
                    break;
                }
                if (entry->key < arg1) {
                    lo = mid + 1;
                } else {
                    hi = mid - 1;
                }
                if (hi < lo) {
                    entry = 0;
                    break;
                }
            }
        }
        if (entry != 0) {
            break;
        }
        list = *(u8 **)(list + HEADER_NEXT_OFFSET);
        if (list == 0) {
            return 0;
        }
    }

    if (arg8 != 0) {
        *(Unk80031AE8Entry **)arg8 = entry;
    }
    n = entry->count;
    base = list + HEADER_COUNT_OFFSET + entry->unk_02;
    t6 = 0;
    if (entry->flags & FLAG_SIZES) {
        sizes = (u16 *)base;
        times = (s16 *)(base + n * 2);
    } else {
        sizes = 0;
        t6 = *(u16 *)base;
        times = (s16 *)(base + 2);
    }
    if (entry->flags & FLAG_TIMES) {
        data = (u8 *)(times + n);
    } else {
        data = (u8 *)times;
        times = 0;
    }
    data = (u8 *)(((s32)data + DATA_ALIGN_ADD) & DATA_ALIGN_MASK);

    if (arg5 == MODE_INDEX) {
        idx = arg2;
        if (!(arg2 < n)) {
            return 0;
        }
    } else {
        if (times == 0) {
            return 0;
        }
        time = arg2 >> TIME_SHIFT;
        if (time < times[0]) {
            return 0;
        }
        hi = n - 1;
        if (!(time < times[hi])) {
            idx = hi;
        } else {
            lo = 0;
            while (1) {
                mid = (lo + hi) >> 1;
                if (time < times[mid]) {
                    hi = mid;
                } else if (time < times[mid + 1]) {
                    idx = mid;
                    break;
                } else {
                    lo = mid + 1;
                }
            }
        }
        if (arg5 == MODE_FLOOR) {
            /* fall through to the lookup */
        } else if (arg5 == MODE_EXACT) {
            if (times[idx] != time) {
                return 0;
            }
        } else {
            goto interpolate;
        }
    }

    /* Lookup: pointer to element arg3 of frame idx. */
    if (sizes != 0) {
        t6 = sizes[idx];
    }
    if (!(arg3 < t6)) {
        return 0;
    }
    if (arg4 != 0) {
        *arg4 = t6;
    }
    skip = arg3;
    if (idx > 0) {
        if (sizes != 0) {
            skip = 0;
            for (i = 0; i < idx; i++) {
                skip += sizes[i];
            }
            skip += arg3;
        } else {
            skip = t6 * idx + arg3;
        }
    }
    if (skip != 0) {
        data += skip * entry->unk_05;
    }
    return (s32)data;

interpolate:
    if (sizes != 0) {
        t6 = sizes[idx];
    }
    n = arg7;
    if (n == 0) {
        n = t6;
    }
    if (t6 < arg3 + n) {
        return 0;
    }
    if (arg4 != 0) {
        *arg4 = t6;
    }
    skip = arg3;
    if (idx > 0) {
        if (sizes != 0) {
            skip = 0;
            for (i = 0; i < idx; i++) {
                skip += sizes[i];
            }
            skip += arg3;
        } else {
            skip = t6 * idx + arg3;
        }
    }
    if (skip != 0) {
        data += skip * entry->unk_05;
    }
    out = (u8 *)arg6;

    data2 = data + t6 * entry->unk_05;
    if (idx != entry->count - 1 && (sizes == 0 || t6 == sizes[idx + 1])) {
        num = arg2 - (times[idx] << TIME_SHIFT);
        denom = (times[idx + 1] << TIME_SHIFT) - (times[idx] << TIME_SHIFT);
        if (num != 0) {
            frac = (num << LERP_ONE_SHIFT) / denom;
            type = entry->flags & FLAG_TYPE_MASK;
            switch (type) {
            case 1:
            case 17:
                if (n != 0) {
                    do {
                        if (type == 1) {
                            a = *data;
                            b = *data2;
                        } else {
                            a = *(s8 *)data;
                            b = *(s8 *)data2;
                        }
                        data += 1;
                        data2 += 1;
                        *out = a + LERP(a, b, frac);
                        out += 1;
                    } while (--n != 0);
                }
                return (s32)arg6;
            case 2:
            case 18:
                if (n != 0) {
                    do {
                        if (type == 2) {
                            a = *(u16 *)data;
                            b = *(u16 *)data2;
                        } else {
                            a = *(s16 *)data;
                            b = *(s16 *)data2;
                        }
                        data += 2;
                        data2 += 2;
                        *(s16 *)out = a + LERP(a, b, frac);
                        out += 2;
                    } while (--n != 0);
                }
                return (s32)arg6;
            case 3:
            case 4:
            case 5:
            case 19:
                if (n != 0) {
                    do {
                        a = *(s32 *)data;
                        b = *(s32 *)data2;
                        data += 4;
                        data2 += 4;
                        *(s32 *)out = a + LERP(a, b, frac);
                        out += 4;
                    } while (--n != 0);
                }
                return (s32)arg6;
            case 6:
                if (n != 0) {
                    do {
                        for (i = 0; i < 3; i++) {
                            a = ((s16 *)data)[i];
                            b = ((s16 *)data2)[i];
                            ((s16 *)out)[i] = a + LERP(a, b, frac);
                        }
                        data += 6;
                        data2 += 6;
                        out += 6;
                    } while (--n != 0);
                }
                return (s32)arg6;
            case 7:
                if (n != 0) {
                    do {
                        for (i = 0; i < 3; i++) {
                            a = ((s32 *)data)[i];
                            b = ((s32 *)data2)[i];
                            ((s32 *)out)[i] = a + LERP(a, b, frac);
                        }
                        data += 12;
                        data2 += 12;
                        out += 12;
                    } while (--n != 0);
                }
                return (s32)arg6;
            default:
                return (s32)arg6;
            }
        } else {
            /* identical keyframes: plain copy below */
        }
    } else {
        /* last frame or differing sizes: plain copy below */
    }

    n = n * entry->unk_05;
    if (n != 0) {
        do {
            *out = *data;
            data += 1;
            out += 1;
        } while (--n != 0);
    }
    return (s32)arg6;
}
