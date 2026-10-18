#include "common.h"
#include "sdk.h"
#include "game.h"

#ifdef NON_MATCHING
/* sizeof(EntProperty), a plain int like the index it scales */
#define ENTRY_STRIDE 8

#define TIME_SHIFT 8
#define LERP_ONE_SHIFT 8
#define LERP_ROUND_SHIFT 7
#define DATA_ALIGN_MASK (-4)
#define DATA_ALIGN_ADD 3

#define LERP(a, b, t) ((((((b) - (a)) * (t)) >> LERP_ROUND_SHIFT) + 1) >> 1)

s32 Ent_GetProperty(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 *arg4, s32 arg5, s32 *arg6, s32 arg7, s32 arg8) {
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
    EntProperty *entry;
    u16 *sizes;
    s16 *times;
    u8 *data;
    u8 *data2;
    u8 *base;
    u8 *out;

    list = (u8 *)arg0;
    while (1) {
        n = *(s16 *)(list + ENT_PROPERTY_COUNT_OFFSET);
        entry = 0;
        if (n > 0) {
            lo = 0;
            hi = n - 1;
            mid = hi;
            while (1) {
                mid = (lo + hi) >> 1;
                entry = (EntProperty *)(list + ENT_PROPERTIES_OFFSET + mid * ENTRY_STRIDE);
                if (arg1 == entry->id) {
                    break;
                }
                if (entry->id < arg1) {
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
        list = *(u8 **)(list + ENT_NEXT_OFFSET);
        if (list == 0) {
            return 0;
        }
    }

    if (arg8 != 0) {
        *(EntProperty **)arg8 = entry;
    }
    n = entry->row_count;
    base = list + ENT_DATA_OFFSET + entry->offset;
    t6 = 0;
    if (entry->type & ENT_PROPERTY_SPARSE) {
        sizes = (u16 *)base;
        times = (s16 *)(base + n * 2);
    } else {
        sizes = 0;
        t6 = *(u16 *)base;
        times = (s16 *)(base + 2);
    }
    if (entry->type & ENT_PROPERTY_META) {
        data = (u8 *)(times + n);
    } else {
        data = (u8 *)times;
        times = 0;
    }
    data = (u8 *)(((s32)data + DATA_ALIGN_ADD) & DATA_ALIGN_MASK);

    if (arg5 == ENT_ROW_INDEX) {
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
        if (arg5 == ENT_ROW_AT_OR_BEFORE) {
            /* fall through to the lookup */
        } else if (arg5 == ENT_ROW_EXACT) {
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
        data += skip * entry->element_size;
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
        data += skip * entry->element_size;
    }
    out = (u8 *)arg6;

    data2 = data + t6 * entry->element_size;
    if (idx != entry->row_count - 1 && (sizes == 0 || t6 == sizes[idx + 1])) {
        num = arg2 - (times[idx] << TIME_SHIFT);
        denom = (times[idx + 1] << TIME_SHIFT) - (times[idx] << TIME_SHIFT);
        if (num != 0) {
            frac = (num << LERP_ONE_SHIFT) / denom;
            type = entry->type & ENT_PROPERTY_TYPE_MASK;
            switch (type) {
            case ENT_TYPE_U8:
            case ENT_TYPE_S8:
                if (n != 0) {
                    do {
                        if (type == ENT_TYPE_U8) {
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
            case ENT_TYPE_U16:
            case ENT_TYPE_S16:
                if (n != 0) {
                    do {
                        if (type == ENT_TYPE_U16) {
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

    n = n * entry->element_size;
    if (n != 0) {
        do {
            *out = *data;
            data += 1;
            out += 1;
        } while (--n != 0);
    }
    return (s32)arg6;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80031AE8", Ent_GetProperty);
#endif

s32 Ent_GetPropertyValue(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    s32 sp28[4];
    u8 *sp38;
    u8 *p;

    p = (u8 *) Ent_GetProperty(arg0, arg1, arg2, arg3, (s32 *) arg4, arg5, sp28, 1, (s32) &sp38);
    if (p == 0) {
        return arg6;
    }
    /* EntProperty's type byte */
    switch (sp38[4] & ENT_PROPERTY_TYPE_MASK) {
    case ENT_TYPE_U8:
        return *p;
    case ENT_TYPE_S8:
        return *(s8 *) p;
    case ENT_TYPE_U16:
        return *(u16 *) p;
    case ENT_TYPE_S16:
        return *(s16 *) p;
    default:
        return *(s32 *) p;
    }
}

s32 Ent_GetPropertyRowLength(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 sp28[4];
    s32 sp38;

    if (Ent_GetProperty(arg0, arg1, arg2, 0, &sp38, arg3, sp28, 1, 0) != 0) {
        return sp38;
    }
    return 0;
}

/* This and the next 19 functions only store arg3 in its slot of the caller's frame: what taking its address does */
void func_8003230C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032314(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003231C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032324(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003232C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032334(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003233C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032344(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003234C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032354(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003235C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032364(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003236C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032374(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003237C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032384(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003238C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_80032394(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_8003239C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

void func_800323A4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (void)&arg3;
}

#ifdef NON_MATCHING
extern u32 D_8005FC84;
extern s32 D_8005FC8C;
extern s32 D_8005FC94;
extern s32 D_8005FC98;
extern u8 D_8005FC9C;
extern u8 D_8005FC9D;
extern s16 D_8005FC9E;
extern s16 D_8005FCA0;
extern s16 D_8005FCA2;
extern s32 D_8005FCA4;
extern s32 D_8005FCA8;
extern s32 D_8005FCAC;

void func_800323AC(void) {
    D_8005FC94 = 0x6396347F;
    D_8005FC9C = 1;
    D_8005FC9D = 0x80;
    D_8005FC9E = 0x3FFF;
    D_8005FCA2 = 0x1000;
    D_8005FC8C = 0;
    D_8005FCA0 = 0;
    D_8005FC98 = 0;
    D_8005FCA4 = 0;
    D_8005FCA8 = 0;
    D_8005FCAC = 0;
    D_8005FC84 &= ~2;
    D_8005FC84 &= ~1;
    D_8005FC84 &= ~4;
    D_8005FC84 &= ~8;
    D_8005FC84 &= ~0x10;
    D_8005FC84 &= ~0x20;
    D_8005FC84 &= ~0x40;
    D_8005FC84 &= ~0x80;
    D_8005FC84 &= ~0x100;
    D_8005FC84 |= 0x600;
    D_8005FC84 &= ~0x800;
    D_8005FC84 &= ~0x7000;
    D_8005FC84 &= 0xFFFF7FFF;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80031AE8", func_800323AC);
#endif

void func_80032498(void) {
}

void func_800324A0(void) {
}
