#include "common.h"
#include "sdk.h"
#include "game.h"

extern void func_800200A8(s32, s32, s32, s32);
extern void func_80021A08(s32 *, s32);
extern void func_80021F4C(s32 *, s32);
extern void func_80022314(s32 *, s32);
extern void func_800225C0(s32 *, s32);
extern void func_8002271C(s32 *, s32);
extern void func_80022878(s32 *);
extern void func_80022A5C(s32 *);
extern void func_80022E74(s32 *);
extern void func_80022F58(s32 *);
extern void func_800237F8(void);
extern void func_80025D18(s32 *, s32 *);
extern s8 D_8005B994;
extern s8 D_8005B99B;
extern s32 D_8005F38C;
extern char *D_800608CC;
extern s32 D_800608D4;
extern s32 D_800608E0;
extern u32 D_800608F0;
extern s32 D_80062AD8;
extern s32 D_80069948;
extern s32 D_8006CB94[];
extern s32 D_8006CBB4;

s32 func_80026CA0(void) {
    s32 sp18;
    s32 s1;
    s32 *s0;
    s32 *p;

    if ((D_80062AD8 & 0x400) && (D_80069948 & 0xE0)) {
        sp18 = 0x8000;
        func_80025FDC(0, &sp18);
        func_8001D194(0, 0x4300, 0, 0, 0);
    }
    if (D_8005F38C == 0) {
        return 0;
    }
    p = D_8006CB94;
    if (!(*p & 2)) {
        return 0;
    }
    D_8005B99B = 0;
    s1 = 0;
    if (*p & 0x20000) {
        s0 = p + 0x77;
    } else {
        s0 = (s32 *)(D_8005F38C + 0x60);
    }
    func_80025D18(s0, p);
    func_800237F8();
    func_8002609C();
    D_8006CBB4 = ((D_80062AD8 & 0x400) == 0) << 8;
    if (D_8005B994 != 5) {
        switch (D_8005B994) {
        case 6:
            func_80022A5C(s0);
            break;
        case 7:
            func_80022E74(s0);
            s1 = 1;
            break;
        case 9:
            func_80022F58(s0);
            s1 = 1;
            break;
        default:
            break;
        }
        if (D_8005B994 != 5) {
            return 1;
        }
    }
    switch (D_800608F0) {
    case 0: func_8002271C(s0, s1); break;
    case 2: func_80022878(s0); break;
    case 4: func_80021A08(s0, s1); break;
    case 10: func_80021F4C(s0, s1); break;
    case 3: func_80022314(s0, s1); break;
    case 8: func_800225C0(s0, s1); break;
    default: func_800200A8((s32)D_800608CC, D_800608D4, D_800608E0, 0); break;
    }
    return 1;
}

#ifdef NON_MATCHING
extern s32 DrawSync(s32);
extern s32 LoadImage(s16 *, void *);

void func_80026EF4(s32 *arg0) {
    s16 rect[4];
    s32 y, w, h, rows, n, area;
    u16 *lut;
    u32 *src;
    u32 *dst;
    u32 p;

    func_80013A54(8, 1);
    func_80013A54(9, 1);
    func_80013A54(0xC, 1);
    func_80013A54(0xD, 1);
    w = arg0[6];
    h = arg0[7];
    area = w * h;
    rect[0] = 0;
    rect[1] = 0x100;
    rect[2] = w;
    rect[3] = h;
    rows = arg0[1];
    lut = (u16 *)arg0[0xE];
    for (y = 0; y < rows; y += 0x100) {
        func_800270C4(arg0, y);
        src = (u32 *)arg0[0xD];
        dst = (u32 *)arg0[0x12];
        for (n = area >> 2; n > 0; n--) {
            p = *src++;
            *dst = (lut[(p >> 8) & 0xFF] << 16) | lut[p & 0xFF];
            dst++;
            *dst = (lut[p >> 24] << 16) | lut[(p >> 16) & 0xFF];
            do { } while (0);
            dst++;
        }
        LoadImage(rect, (void *)arg0[0x12]);
        DrawSync(0);
        rect[1] += h;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80026EF4);
#endif

extern u8 *D_80069358;

extern u8 *D_80063454;

void func_8002706C(void) {
    RECT rect;
    u8 *p;
    s32 w;

    p = D_80069358;
    if (p != 0) {
        rect.x = 0;
        rect.y = 0x108;
        w = *(s32 *)(p + 0x18);
        rect.h = 0xF0;
        rect.w = w;
        MoveImage(&rect, *(s16 *)(D_80063454 + 0x202C), 0);
    }
}

s32 func_8002719C(s32 *arg0);

s32 func_800270C4(s32 *arg0, s32 arg1) {
    s32 pos;
    s32 end;
    s32 limit;
    s32 delta;
    s32 ret;

    ret = 0;
    pos = arg0[0] >> 8;
    if (pos < 0) {
        ret = func_8002719C(arg0);
        pos = arg0[0] >> 8;
    }
    end = arg1 >> 8;
    if (end < 0) {
        end = 0;
    }
    limit = arg0[1] >> 8;
    delta = end - pos;
    if (end >= limit) {
        end = limit - 1;
        arg1 = (limit << 8) - 1;
    }
    delta = end - pos;
    if (delta < 0) {
        ret |= func_8002719C(arg0);
        delta = end - (arg0[0] >> 8);
    }
    if (delta != 0) {
        ret |= func_800271D8(arg0, delta);
    }
    arg0[0] = arg1;
    return ret;
}


s32 func_8002719C(s32 *arg0) {
    s32 result;

    arg0[0] = -0x100;
    arg0[5] = 0;
    result = func_800271D8(arg0, 1);
    arg0[0] = 0;
    return result;
}

typedef struct Unk800271D8List {
    s32 pad0;
    s32 pad4;
    s32 pad8;
    s32 count;
    s32 *items[1];
} Unk800271D8List;

typedef struct Unk800271D8 {
    s32 pos;
    u8 pad4[0x10];
    s32 index;
    u8 pad18[0x38];
    Unk800271D8List *list;
} Unk800271D8;

s32 func_800272A4(Unk800271D8 *, u8 *);

s32 func_800271D8(void *object, s32 arg1) {
    Unk800271D8 *arg0 = object;
    s32 result;
    s32 count;
    u8 *item;
    s32 index;

    result = 0;
    count = arg0->list->count;
    while (arg1 > 0) {
        index = arg0->index;
        if (index >= count) {
            break;
        }
        item = (u8 *)arg0->list->items[index];
        arg0->index = index + 1;
        result |= func_800272A4(arg0, item);
        if ((*item >> 6) == 2) {
            arg1--;
            arg0->pos += 0x100;
        }
    }
    return result;
}

#ifdef NON_MATCHING
#define KIND_SHIFT 6
#define KIND_COPY_A 1
#define KIND_COPY_B 2
#define MODE_MASK 0x38
#define MODE_SHIFT 3
#define MODE_PLAIN 0
#define MODE_MASKED 1
#define LOW_BITS_MASK 7
#define FLIP_FLAG 8
#define RESULT_KIND1 0x200
#define RESULT_KIND2 0x100
#define HEADER_SIZE 4
#define OFS_WIDTH 0x18
#define OFS_STRIDE 0x1C
#define OFS_DST_PLAIN 0x34
#define OFS_DST_KIND1 0x38
#define OFS_DST_DATA 0x3C
#define OFS_DST_MASK 0x40
#define OFS_ARG5 0x44

#define FIELD_S32(base, ofs) (*(s32 *)((u8 *)(base) + (ofs)))
#define FIELD_PTR(base, ofs) (*(u8 **)((u8 *)(base) + (ofs)))

s32 func_800272A4(Unk800271D8 *arg0, u8 *arg1) {
    s32 outLen;
    s32 n;
    u32 mode;
    u8 *mask;
    u8 *data;
    s32 result;

    result = 0;
    switch (arg1[0] >> KIND_SHIFT) {
    case KIND_COPY_A:
        func_80027420(arg1 + HEADER_SIZE, FIELD_PTR(arg0, OFS_DST_KIND1), arg1[0] & LOW_BITS_MASK, 0, 0, FIELD_S32(arg0, OFS_ARG5));
        result = RESULT_KIND1;
        break;
    case KIND_COPY_B:
        mode = (arg1[0] & MODE_MASK) >> MODE_SHIFT;
        n = arg1[0] & LOW_BITS_MASK;
        switch (mode) {
        case MODE_PLAIN:
            if (n != 0) {
                func_80027420(arg1 + 1, FIELD_PTR(arg0, OFS_DST_PLAIN), n, 0, 0, FIELD_S32(arg0, OFS_ARG5));
                result = RESULT_KIND2;
            }
            break;
        case MODE_MASKED:
            mask = func_80027420(arg1 + ((arg1[2] << 8) | arg1[3]), FIELD_PTR(arg0, OFS_DST_MASK), arg1[1] & LOW_BITS_MASK, 0, mode, FIELD_S32(arg0, OFS_ARG5));
            data = func_80027420(arg1 + HEADER_SIZE, FIELD_PTR(arg0, OFS_DST_DATA), n, &outLen, mode, FIELD_S32(arg0, OFS_ARG5));
            func_80027500(data, mask, FIELD_PTR(arg0, OFS_DST_PLAIN), outLen, arg1[1] & FLIP_FLAG, FIELD_S32(arg0, OFS_WIDTH), FIELD_S32(arg0, OFS_STRIDE));
            result = RESULT_KIND2;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return result;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800272A4);
#endif

void *memcpy(void *, const void *, u32);

u8 *func_80027420(u8 *src, u8 *dst, s32 mode, s32 *outLen, s32 noCopy, s32 arg5) {
    s32 len;

    switch (mode) {
    case 1:
        len = (src[0] << 8) | src[1];
        src += 2;
        if (noCopy != 0) {
            dst = src;
        } else {
            memcpy(dst, src, len);
        }
        break;
    case 2:
        len = func_80027C00(src, dst);
        break;
    case 3:
        len = func_80027CD0(src, dst, arg5);
        break;
    default:
        len = 0;
        break;
    }
    if (outLen != 0) {
        *outLen = len;
    }
    return dst;
}

void func_80027500(u8 *src, u8 *mask, u8 *dst, s32 n, s32 mode, s32 w, s32 stride) {
    u8 *row;
    s32 x;
    s32 y;
    u32 bit;
    u32 m;

    if (n > 0) {
        if (mode != 0) {
            w >>= 3;
            y = stride - 1;
            w--;
            if (y >= 0) {
                row = mask;
                do {
                    x = w;
                    mask++;
                    if (x >= 0) {
                        do {
                            m = *row;
                            if (m != 0) {
                                bit = 0x80;
                                do {
                                    if (bit & m) {
                                        *dst = *src++;
                                        if (--n == 0) {
                                            return;
                                        }
                                    }
                                    bit >>= 1;
                                    dst++;
                                } while (bit & 0xFF);
                            } else {
                                dst += 8;
                            }
                            x--;
                            row += stride;
                        } while (x >= 0);
                    }
                    y--;
                    row = mask;
                } while (y >= 0);
            }
        } else {
            for (;;) {
                m = *mask++;
                if (m == 0) {
                    dst += 8;
                    continue;
                }
                bit = 0x80;
                do {
                    if (m & bit) {
                        *dst = *src++;
                        if (--n == 0) {
                            return;
                        }
                    }
                    bit >>= 1;
                    dst++;
                } while (bit & 0xFF);
            }
        }
    }
}

void func_80027600(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_t0;
    s32 var_v1;

    var_v1 = 0;
    var_t0 = 0;
    if (arg3 & 0x100) {
        var_v1 = *(s32 *)((char *)arg0 + 0x34);
    }
    if (arg3 & 0x200) {
        var_t0 = *(s32 *)((char *)arg0 + 0x38);
    }
    if ((var_t0 != 0) || (var_v1 != 0)) {
        func_800176DC(1, arg1, arg2, var_t0, var_v1, 1);
    }
}

#ifdef NON_MATCHING
#define TAG_SHIFT 6
#define TAG_HEADER 0
#define TAG_COUNTED 2
#define HEADER_FLAG_MASK 0x1
#define HEIGHT_HI_MASK 0x1F
#define DEPTH_MASK 0xE0
#define DEPTH_SHIFT 3
#define BYTE_BITS 8
#define COUNT_SHIFT 8
#define INIT_VALUE (-0x100)

typedef struct Unk80027664List {
    /* 0x00 */ u8 unk_00[0xC];
    /* 0x0C */ s32 count;
    /* 0x10 */ u8 *ent[1];
} Unk80027664List;

s32 func_80027664(s32 *arg0) {
    Unk80027664List *list;
    u8 *hdr;
    s32 i;
    s32 n;
    s32 flag;
    s32 *out;

    list = func_80014260(arg0, 1, 1);
    hdr = 0;
    n = 0;
    for (i = 0; i < list->count; i++) {
        u32 tag = list->ent[i][0] >> TAG_SHIFT;

        if (tag == TAG_HEADER) {
            hdr = list->ent[i];
        } else if (tag == TAG_COUNTED) {
            n++;
        } else {
            /* other tags are ignored */
        }
    }
    if (hdr == 0 || n == 0) {
        return 0;
    }
    flag = hdr[0] & HEADER_FLAG_MASK;
    out = func_800277A8((hdr[1] << BYTE_BITS) | hdr[2],
                        ((hdr[3] & HEIGHT_HI_MASK) << BYTE_BITS) | hdr[4],
                        (hdr[3] & DEPTH_MASK) >> DEPTH_SHIFT, flag);
    if (out == 0) {
        return 0;
    }
    out[0] = INIT_VALUE;
    out[5] = 0;
    out[1] = n << COUNT_SHIFT;
    out[0x50 / 4] = (s32)list;
    out[0x4C / 4] = *arg0;
    if (flag != 0) {
        D_80069358 = (u8 *)out;
        func_80026EF4(out);
    } else {
        /* no global registration */
    }
    return (s32)out;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027664);
#endif

#ifdef NON_MATCHING
typedef struct Slot80027 {
    s32 pad0[2];
    s32 used;
    s32 pad1;
    u32 size;
    s32 pad2;
    s32 w, h, bpp, arg3, stride, bytes, extra;
    s32 data, end, f3C, f40, f44, f48;
    s32 pad3[2];
} Slot80027;

typedef struct Pool80027 {
    s32 count;
    Slot80027 *slots;
    s32 pad;
    u32 free;
    s32 cursor;
    s32 a, b, c, d;
} Pool80027;

extern s32 D_80069338;

void *func_800277A8(s32 w, s32 h, s32 bpp, s32 arg3) {
    Pool80027 *pool;
    Slot80027 *s;
    s32 stride, bytes, extra, i, a;
    u32 total;

    if (bpp == 16) {
        stride = w * 2;
        extra = 0;
    } else if (bpp < 17) {
        if (bpp == 4) {
            stride = w >> 1;
            extra = 32;
        } else {
            stride = w;
            extra = 512;
        }
    } else if (bpp == 24) {
        stride = w * 3;
        extra = 0;
    } else {
        stride = w;
        extra = 512;
    }
    s = *(Slot80027 **)&D_80069338;
    i = 0;
    pool = (Pool80027 *)((char *)&D_80069338 - 4);
    bytes = h * stride;
loop:
    if (i >= *(s32 *)((char *)&D_80069338 - 4)) return 0;
    i++;
    if (s->used != 0) {
        s++;
        goto loop;
    }
    a = (bytes + 15) & ~15;
    total = a + ((extra + 15) & ~15);
    if (pool->free < total) return 0;
    s->used = 1;
    s->size = total;
    s->data = pool->cursor;
    s->end = pool->cursor + a;
    pool->cursor += total;
    pool->free -= total;
    s->f3C = pool->a;
    s->f40 = pool->b;
    s->f44 = pool->c;
    s->w = w; s->h = h; s->bpp = bpp; s->arg3 = arg3;
    s->stride = stride; s->bytes = bytes; s->extra = extra;
    s->f48 = pool->d;
    return s;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_800277A8);
#endif


void func_800278F8(s32 arg0) {
    if (*(s32 *)(arg0 + 0xC) != 1) {
        func_800143C0((void *)(arg0 + 0x4C), 1);
        if (*(s32 *)(arg0 + 0x24) != 0) {
            func_80013A54(8, 0);
            func_80013A54(9, 0);
            func_80013A54(0xC, 0);
            func_80013A54(0xD, 0);
            D_80069358 = 0;
        }
        func_80027980(arg0);
    }
}

#ifdef NON_MATCHING
#define ENTRY_SIZE 0x54
#define ENTRY_STATE 2
#define ENTRY_FLAG 3
#define ENTRY_AMOUNT 4
#define POOL_FREE 3
#define POOL_USED 4
#define OWNER_ACTIVE 2
#define OWNER_FLAG 3
#define STATE_LIVE 1

extern s32 D_80069334;

void func_80027980(s32 arg0) {
    s32 *owner = (s32 *)arg0;
    s32 *pool = &D_80069334;
    s32 i;

    if (owner[OWNER_ACTIVE] != 0) {
        owner[OWNER_FLAG] = 1;
        for (i = pool[0] - 1; i >= 0; i--) {
            s32 *entry = (s32 *)(D_80069338 + i * ENTRY_SIZE);

            if (entry[ENTRY_STATE] == STATE_LIVE) {
                if (entry[ENTRY_FLAG] != STATE_LIVE) {
                    break;
                } else {
                    entry[ENTRY_STATE] = 0;
                    entry[ENTRY_FLAG] = 0;
                    pool[POOL_USED] -= entry[ENTRY_AMOUNT];
                    pool[POOL_FREE] += entry[ENTRY_AMOUNT];
                }
            } else {
                continue;
            }
        }
    } else {
        return;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027980);
#endif

#ifdef NON_MATCHING
#define POOL_SLOTS 5
#define SLOT_TABLE_BYTES 0x1A4
#define SLOT_STRIDE_WORDS 0x15
#define SLOT_BUFFER_BYTES 0x1600
#define BUFFER_EXTRA_BYTES 0x4210
#define BUFFER_ALIGN_MASK 0xF
#define BUFFER_ALIGN 0x10
#define REGION_1_OFFSET 0x1000
#define REGION_2_OFFSET 0x2000
#define REGION_3_OFFSET 0x2200
#define REGION_HEAD_BYTES 0x4200
#define RESULT_OK -0xFF
#define RESULT_NO_MEMORY -0xF
#define MODE_A 2
#define MODE_B 0x3C

extern s32 D_8005F3EC;
extern s32 D_8006933C;
extern s32 D_80069340;
extern s32 D_80069344;
extern s32 D_80069348;
extern s32 D_8006934C;
extern s32 D_80069350;
extern s32 D_80069354;

s32 func_80027A38(void) {
    s32 *p;
    s32 i;
    s32 size;
    s32 buf;
    s32 pad;
    s32 left;
    s32 *g;

    g = &D_80069334;
    g[0] = 0;
    D_8006933C = 0;
    g[1] = 0;
    D_80069358 = 0;
    if (D_8005F3EC == MODE_A || D_8005F3EC == MODE_B) {
        g[0] = POOL_SLOTS;
        p = (s32 *)func_8001148C(SLOT_TABLE_BYTES);
        g[1] = (s32)p;
        if (p != 0) {
            for (i = 0; i < g[0]; i++) {
                p[2] = 0;
                p[3] = 0;
                p += SLOT_STRIDE_WORDS;
            }
            size = g[0] * SLOT_BUFFER_BYTES + BUFFER_EXTRA_BYTES;
            buf = func_8001148C(size);
            D_8006933C = buf;
            if (buf != 0) {
                pad = buf & BUFFER_ALIGN_MASK;
                if (pad != 0) {
                    pad = BUFFER_ALIGN - pad;
                } else {
                    pad = 0;
                }
                buf += pad;
                left = size - pad;
                D_80069348 = buf + REGION_1_OFFSET;
                D_8006934C = buf + REGION_2_OFFSET;
                D_80069344 = buf;
                D_80069350 = buf;
                D_80069340 = left;
                D_80069354 = buf + REGION_3_OFFSET;
                D_80069344 = buf + REGION_HEAD_BYTES;
                D_80069340 = left - REGION_HEAD_BYTES;
                return RESULT_OK;
            } else {
                return RESULT_NO_MEMORY;
            }
        } else {
            return RESULT_NO_MEMORY;
        }
    } else {
        return RESULT_OK;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027A38);
#endif

extern s32 D_80069334;
extern s32 D_80069338;
extern s32 D_8006933C;

s32 func_80027B9C(void) {
    D_80069334 = 0;
    if (D_8006933C != 0) {
        func_800116C0(D_8006933C);
        D_8006933C = 0;
    }
    if (D_80069338 != 0) {
        func_800116C0(D_80069338);
        D_80069338 = 0;
    }
    return -0xFF;
}

s32 func_80027C00(u8 *src, u8 *dst) {
    u8 *start;
    s8 c;
    s32 n;
    u8 b;
    s32 hi;

    start = dst;
    while (1) {
        c = *src++;
        if (c >= 0) {
            n = c + 1;
            if (c <= 0) {
                hi = *src++;
                n = (hi << 8) | *src++;
                if (n == 0) {
                    return dst - start;
                }
            }
            b = *src++;
            n--;
            while (n != -1) {
                *dst++ = b;
                n--;
            }
        } else {
            n = -c;
            if (c == -0x80) {
                hi = *src++;
                n = (hi << 8) | *src++;
            }
            n--;
            while (n != -1) {
                *dst++ = *src++;
                n--;
            }
        }
    }
}

#ifdef NON_MATCHING
s32 func_80027CD0(u8 *arg0, u8 *arg1, s32 arg2) {
    u8 *ring = (u8 *)arg2;
    u8 *src = arg0;
    u8 *start = arg1;
    u8 *data;
    s32 pos = 1;
    s32 mask;
    s32 flags;
    s32 off;
    s32 len;
    u32 size;
    u8 b;

    size = *src++ << 24;
    size |= *src++ << 16;
    size |= *src++ << 8;
    size |= *src++;
    data = src + size;
    while (1) {
        flags = *src++;
        for (mask = 0x80; mask & 0xFF; mask >>= 1) {
            if (flags & mask) {
                b = *data++;
                *arg1++ = b;
                ring[pos] = b;
                pos = (pos + 1) & 0xFFF;
            } else {
                len = (*data >> 4) + 2;
                off = data[1] | ((*data & 0xF) << 8);
                data += 2;
                if (off == 0) {
                    return arg1 - start;
                }
                while (len-- & 0xFFFF) {
                    b = ring[off];
                    off = (off + 1) & 0xFFF;
                    ring[pos] = b;
                    pos = (pos + 1) & 0xFFF;
                    *arg1++ = b;
                }
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027CD0);
#endif

extern char *D_800608CC;

typedef struct {
    char pad[0x190];
    s32 count;
    s32 ent[8];
    s32 flags[8];
} Unk80027DEC;

void *func_80027DEC(char *arg0, void *arg1) {
    Unk80027DEC *p;
    void *r;
    char **global;
    s32 i;

    p = *(Unk80027DEC **)(arg0 + 0x10);
    global = &D_800608CC;
    for (i = p->count - 1; i >= 0; i--) {
        if (p->flags[i] & 4) {
            r = func_80014260(&p->ent[i], 1, 0);
            if (func_80030D74(r, arg1)) {
                return r;
            }
        }
    }
    if (arg0 != *global) {
        p = *(Unk80027DEC **)(D_800608CC + 0x10);
        for (i = p->count - 1; i >= 0; i--) {
            if (p->flags[i] & 4) {
                r = func_80014260(&p->ent[i], 1, 0);
                if (func_80030D74(r, arg1)) {
                    return r;
                }
            }
        }
    }
    return (void *)-0x17;
}

INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80027F3C);

#ifdef NON_MATCHING
typedef struct {
    s32 x, y, z;
} Vec3i80028750;



s32 func_80028750(Vec3i80028750 *arg0, Vec3i80028750 *arg1) {
    Unk80027DEC *p = *(Unk80027DEC **)(D_800608CC + 0x10);
    Vec3i80028750 pos;
    s32 box[6];
    s32 r;
    s32 i;
    s32 *e;
    s32 *q;

    if (arg1->x == 0 && arg1->y == 0 && arg1->z == 0) {
        return 0;
    }
    do {
        pos = *arg0;
        for (i = 0; i < p->count; i++) {
            if (p->flags[i] & 4) {
                e = func_80014260(&p->ent[i], 1, 0);
                if (func_80030D74(e, arg0) != 0) {
                    q = *(s32 **)((char *)e + 0x14);
                    box[0] = q[0] << 8;
                    box[1] = q[1] << 8;
                    box[2] = q[2] << 8;
                    box[3] = q[3] << 8;
                    box[4] = q[4] << 8;
                    box[5] = q[5] << 8;
                    r = func_80027F3C(q, *(u16 *)((char *)q + 0x1C), box, arg0, arg1, 0);
                    break;
                }
            }
        }
        if (i == p->count) {
            return 1;
        }
    } while (r == 0);
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028750);
#endif

extern s32 D_8005BB68[];

void func_8002890C(u8 *arg0, s32 arg1, s32 arg2) {
    s32 off;

    if (*(s32 *)(arg0 + 0xAC) & 0x04000000) {
        off = -((*(s32 *)(arg0 + 0x100) & 0x10) == 0) & 0x18;
        if (arg1 >= 0) {
            *(s32 *)(arg0 + 0x118) = off + D_8005BB68[arg1];
        } else {
            *(s32 *)(arg0 + 0x118) = off + arg2;
        }
    }
}

extern u8 D_8005BB78[];

void func_80028968(s32 arg0, s32 *arg1) {
    if (((u32) (arg0 - 0x30) < 0x10U) && (*arg1 = 0, ((arg0 < 0x40) != 0))) {
        if (arg0 >= 0x30) {
            *arg1 = 0x1000 - (D_8005BB78[arg0] * 0x14);
        }
    } else {
        *arg1 = 0;
    }
}

void func_800289C4(s16 *arg0, s32 arg1, s32 arg2) {
    s32 sp10;

    func_80028968(arg1, &sp10);
    if (arg2 == 0) {
        arg0[0x11] = sp10;
    } else {
        arg0[0x11] = func_8001E5FC(arg0[0x11], sp10, 0x100);
    }
}

#ifdef NON_MATCHING
extern s32 D_8006CB6C;

typedef struct {
    s32 x, y, z;
} V3;

void *func_80028A28(void *unused, s32 *arg) {
    V3 pos;
    s32 box[10];
    s32 i;
    char *p;
    s32 *ent;
    void *res;
    void *best;
    s32 bestd;
    s32 d;
    s32 *q;
    char *o;

    pos = *(V3 *)arg;
    res = 0;
    p = *(char **)(D_800608CC + 0x10);
    while (1) {
        i = 0;
        if (*(s32 *)(p + 0x190) > 0) {
            do {
                if (*(s32 *)(p + 0x1B4 + i * 4) & 4) {
                    ent = func_80014260(p + 0x194 + i * 4, 1, 0);
                    if (func_80030D74(ent, arg) != 0) {
                        res = func_8003DA00(ent[5], arg, 9);
                        break;
                    }
                }
                i++;
            } while (i < *(s32 *)(p + 0x190));
        }
        if (i == *(s32 *)(p + 0x190)) {
            res = 0;
            break;
        } else if (res != 0) {
            do { } while (0);
            break;
        } else {
            continue;
        }
    }
    if (D_8006CB6C > 0) {
        bestd = -0x3B9AC9FF;
        best = 0;
        box[0] = pos.x - 0x4E20;
        box[3] = pos.x + 0x4E20;
        box[2] = pos.z - 0x4E20;
        box[5] = pos.z + 0x4E20;
        box[4] = pos.y;
        box[1] = pos.y;
        if (D_8006CB6C > 0) {
            q = &D_8006CB6C + 0x290 / 4;
            i = 0;
        again:
            o = *(char **)(*q + 4);
            if (*(s32 *)(o + 0xAC) & 0x20000) {
                d = func_8002A8D0((void *)*q, box, 1, 0, bestd, pos.y, 0);
                if (d != -0x3B9AC9FF) {
                    best = o;
                    bestd = d;
                }
            }
            i++;
            q++;
            if (i < D_8006CB6C) {
                goto again;
            }
        }
        if (best != 0 && (res == 0 || !(bestd < arg[1]))) {
            arg[1] = bestd;
            res = best;
        }
    }
    return res;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028A28);
#endif

#ifdef NON_MATCHING
#define FLAG_ACTIVE 0x04000000
#define SEARCH_NONE (-0x3B9AC9FF)
#define SLOT_FLAG_MASK 0x40020000
#define SLOT_FLAG_VALUE 0x20000
#define KIND_MASK 7
#define KIND_ALT 1
#define ALT_BOX_OFFSET 0x2C
#define BOX_OFFSET 8
#define BOX_MARGIN 0x88B8
#define LIST_OFFSET 0x290
#define ID_MASK 0x3C00
#define ID_SHIFT 10
#define FIXED_SHIFT 8


/* func_8003DA00 takes six more stack arguments than game.h's prototype. */
typedef void *(*Func8003DA00Full)(s32, void *, s32, s32, s32, s32, s32, s32, s32);

void func_80028C48(s32 unit, s32 *arg, s32 arg2) {
    char *arg0 = (char *)unit;
    s32 x, y, z;
    s32 i;
    char *p;
    s32 *ent;
    s32 *rng;
    void *res;
    void *best;
    s32 bestd;
    s32 d;
    s32 *q;
    s32 *a;
    char *o;
    s32 hit;

    bestd = SEARCH_NONE;
    res = 0;
    best = 0;
    hit = 0;
    if (!(*(s32 *)(arg0 + 0xAC) & FLAG_ACTIVE)) {
        return;
    }
    p = *(char **)(D_800608CC + 0x10);
    x = arg[0];
    y = arg[1];
    z = arg[2];
    while (1) {
        i = 0;
        if (*(s32 *)(p + 0x190) > 0) {
            do {
                if (*(s32 *)(p + 0x1B4 + i * 4) & 4) {
                    ent = func_80014260(p + 0x194 + i * 4, 1, 0);
                    if (func_80030D74(ent, arg) != 0) {
                        rng = (s32 *)ent[5];
                        res = ((Func8003DA00Full)func_8003DA00)(
                            (s32)rng, arg, 1, rng[0] << FIXED_SHIFT, rng[1] << FIXED_SHIFT,
                            rng[2] << FIXED_SHIFT, rng[3] << FIXED_SHIFT, rng[4] << FIXED_SHIFT,
                            rng[5] << FIXED_SHIFT);
                        break;
                    }
                }
                i++;
            } while (i < *(s32 *)(p + 0x190));
        }
        if (i == *(s32 *)(p + 0x190)) {
            res = 0;
            break;
        } else if (res != 0) {
            break;
        } else {
            continue;
        }
    }
    if (D_8006CB6C > 0) {
        q = (s32 *)((char *)&D_8006CB6C + LIST_OFFSET);
        for (i = 0; i < D_8006CB6C; i++, q++) {
            o = *(char **)(*q + 4);
            if ((*(s32 *)(o + 0xAC) & SLOT_FLAG_MASK) != SLOT_FLAG_VALUE) {
                continue;
            }
            if ((*(s32 *)*q & KIND_MASK) == KIND_ALT) {
                a = (s32 *)(*q + ALT_BOX_OFFSET);
            } else {
                a = (s32 *)(*q + BOX_OFFSET);
            }
            d = a[4];
            if (x < a[0] - BOX_MARGIN || a[3] + BOX_MARGIN < x || z < a[2] - BOX_MARGIN ||
                a[5] + BOX_MARGIN < z) {
                continue;
            }
            if (!(d < arg[1]) && !(arg[1] < a[1])) {
                res = o;
                arg[1] = d;
                hit = 1;
                goto done;
            }
            if (bestd < d && !(y < d)) {
                best = o;
                bestd = d;
            }
        }
        if (best != 0 && (res == 0 || !(bestd > arg[1]))) {
            arg[1] = bestd;
            res = best;
            hit = 1;
        }
    }
done:
    if (res != 0) {
        if (hit != 0) {
            func_8002890C(*(u8 **)(arg0 + 0x44), -1, *(s32 *)((char *)res + 0x118));
        } else {
            func_8002890C(*(u8 **)(arg0 + 0x44), ((s32)res & ID_MASK) >> ID_SHIFT, -1);
        }
    } else {
        func_8002890C(*(u8 **)(arg0 + 0x44), 0, -1);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028C48);
#endif

#ifdef NON_MATCHING
#define OBJ_FLAGS 0xAC
#define OBJ_LINK 0x44
#define OBJ_ID 0x138
#define OBJ_FLAG_ACTIVE 0x04000000
#define MODE_AXIS_Y 1
#define MODE_AXIS_Z 2
#define MODE_LINKED 4
#define MODE_MASK 3
#define Y_OFFSET 0x61A8
#define SCORE_MIN (-0x3B9AC9FF)
#define LIST_COUNT 0x190
#define LIST_ENTRIES 0x194
#define LIST_FLAGS 0x1B4
#define LIST_FLAG_CHECK 4
#define BOX_FRAME_BYTES 8
#define BOX_ALT_BYTES 0x2C
#define SLOT_KIND_MASK 7
#define SLOT_KIND_ALT 1
#define SLOT_TABLE_BYTES 0x290
#define NO_ID 0xFFFF
#define BOX_SCALE_SHIFT 8
#define ID_MASK 0x3F0
#define ID_SHIFT 4
#define ID_BASE 0x30
#define ID_RANGE 0x10
#define LEVEL_OFFSET 0x10
#define WORD_BYTES 4


typedef void *(*Unk8003DA00Fn)(s32, void *, s32, s32, s32, s32, s32, s32, s32, s32);

void func_80028FE0(s32 arg0, s32 *arg1, s32 arg2, s32 arg3) {
    s32 start[3];
    s32 pos[3];
    s32 bestTop;
    s32 hitIsBox;
    s32 i;
    s32 id;
    s32 val;
    s32 count;
    s32 *slots;
    s32 *entry;
    s32 *box;
    s32 *rec;
    s32 *found;
    char *level;
    char *hit;
    char *cand;
    char *pend;

    hit = 0;
    pend = 0;
    hitIsBox = 0;
    bestTop = SCORE_MIN;
    if (!(*(s32 *)(arg0 + OBJ_FLAGS) & OBJ_FLAG_ACTIVE)) {
        return;
    } else if ((arg2 & MODE_LINKED) &&
               !(*(s32 *)(*(s32 *)(arg0 + OBJ_LINK) + OBJ_FLAGS) & OBJ_FLAG_ACTIVE)) {
        return;
    } else {
        /* active: continue */
    }
    pos[0] = arg1[0];
    pos[1] = arg1[1];
    pos[2] = arg1[2];
    pos[1] += Y_OFFSET;
    level = *(char **)(D_800608CC + LEVEL_OFFSET);
    start[0] = pos[0];
    start[1] = pos[1];
    start[2] = pos[2];

    do {
        i = 0;
        found = 0;
        if (*(s32 *)(level + LIST_COUNT) > 0) {
            do {
                if (*(s32 *)(level + LIST_FLAGS + i * WORD_BYTES) & LIST_FLAG_CHECK) {
                    found = func_80014260(level + LIST_ENTRIES + i * WORD_BYTES, 1, 0);
                    if (func_80030D74(found, pos) != 0) {
                        rec = (s32 *)found[5];
                        hit = ((Unk8003DA00Fn)func_8003DA00)((s32)rec, pos, arg2 & MODE_MASK, arg3,
                            rec[0] << BOX_SCALE_SHIFT, rec[1] << BOX_SCALE_SHIFT, rec[2] << BOX_SCALE_SHIFT,
                            rec[3] << BOX_SCALE_SHIFT, rec[4] << BOX_SCALE_SHIFT, rec[5] << BOX_SCALE_SHIFT);
                        break;
                    }
                }
                i++;
            } while (i < *(s32 *)(level + LIST_COUNT));
        }
        count = *(s32 *)(level + LIST_COUNT);
        if (i == count) {
            hit = 0;
            break;
        } else {
            /* hit found: retry while the result is null */
        }
    } while (hit == 0);

    if (D_8006CB6C > 0) {
        count = D_8006CB6C;
        slots = (s32 *)((char *)&D_8006CB6C + SLOT_TABLE_BYTES);
        for (i = 0; i < count; i++, slots++) {
            entry = (s32 *)*slots;
            cand = (char *)entry[1];
            if (cand == (char *)arg0 || *(s32 *)(cand + OBJ_ID) == NO_ID) {
                continue;
            }
            box = (s32 *)((char *)entry + BOX_FRAME_BYTES);
            if ((entry[0] & SLOT_KIND_MASK) == SLOT_KIND_ALT) {
                box = (s32 *)((char *)entry + BOX_ALT_BYTES);
            }
            if (arg2 & MODE_AXIS_Y) {
                val = box[4];
                if (start[0] < box[0] || box[3] < start[0] ||
                    start[2] < box[2] || box[5] < start[2]) {
                    continue;
                }
                if (val >= pos[1] && pos[1] >= box[1]) {
                    hit = cand;
                    hitIsBox = 1;
                    pos[1] = box[1];
                    break;
                }
                if (bestTop < val && start[1] >= val) {
                    pend = cand;
                    bestTop = val;
                }
            } else if (arg2 & MODE_AXIS_Z) {
                val = box[5];
                if (start[0] < box[0] || box[3] < start[0] ||
                    start[1] < box[1] || box[4] < start[1]) {
                    continue;
                }
                if (val >= pos[2] && pos[2] >= box[2]) {
                    hit = cand;
                    hitIsBox = 1;
                    pos[2] = box[2];
                    break;
                }
                if (bestTop < val && start[2] >= val) {
                    pend = cand;
                    bestTop = val;
                }
            } else {
                continue;
            }
        }
        if (!hitIsBox && pend != 0) {
            if (arg2 & MODE_AXIS_Y) {
                if (hit == 0 || bestTop >= pos[1]) {
                    pos[1] = bestTop;
                    hit = pend;
                    hitIsBox = 1;
                }
            } else if (arg2 & MODE_AXIS_Z) {
                if (hit == 0 || bestTop >= pos[2]) {
                    pos[2] = bestTop;
                    hit = pend;
                    hitIsBox = 1;
                }
            } else {
                /* no axis: the pending box is dropped */
            }
        }
    }

    id = -1;
    if (hit != 0) {
        if (hitIsBox) {
            id = *(s32 *)(hit + OBJ_ID);
            if (id < 0) {
                id = -id;
                arg3 = 0;
            }
        } else {
            id = ((u32)hit & ID_MASK) >> ID_SHIFT;
            if ((u32)(id - ID_BASE) >= ID_RANGE) {
                id = -1;
            }
        }
    }
    func_800289C4((s16 *)arg0, id, arg3);
    if (arg2 & MODE_LINKED) {
        func_800289C4(*(s16 **)(arg0 + OBJ_LINK), id, arg3);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80028FE0);
#endif

void func_800294B4(s16 *arg0) {
    s32 temp_a0;
    s32 temp_v0;
    u32 var_a1;

    temp_a0 = *(s32 *)((u8 *)arg0 + 0xF0);
    if (temp_a0 != 0) {
        temp_v0 = func_800321D4(temp_a0, 0xF2, 0, 0, 0, 2, 0);
        if ((temp_v0 & 0xFFFF) != 0xFFFF) {
            var_a1 = (u32)(temp_v0 & 0x3F0) >> 4;
            if (var_a1 - 0x30 >= 0x10U) {
                var_a1 = -1;
            }
            func_800289C4(arg0, var_a1, 0);
        }
    }
}

#ifdef NON_MATCHING
#define ENTRY_COUNT 0x200
#define FLAG_FIXED_HEIGHT 0x40000000
#define SLOT_ACTIVE 4
#define LIST_END 0xFFFF
#define LIST_OFFSET 0x10
#define HALF_EXTENT 0x12C00
#define Y_LOW_OFFSET 0x25800
#define Y_LOW_FIXED_OFFSET 0x10B80
#define Y_HIGH_FIXED_OFFSET 0x3A480

typedef struct { u16 id; char pad[6]; } Unk80029538Entry;
typedef struct {
    Unk80029538Entry ent[ENTRY_COUNT];
    s32 one;
    s32 count;
    s32 b[6];
} Unk80029538Out;
typedef struct { char pad[0x40]; s32 unk_40; } Unk80029538Sub;
typedef struct { char pad[8]; Unk80029538Sub *sub; char pad2[0xF4]; u32 flags; } Unk80029538Ctx;
typedef struct { char pad[0x190]; s32 n; void *slot[1]; } Unk80029538Lst;


void func_80029538(s32 arg0, s32 arg1, s32 arg2) {
    s32 *a = (s32 *)arg0;
    Unk80029538Ctx *b = (Unk80029538Ctx *)arg1;
    Unk80029538Out *c = (Unk80029538Out *)arg2;
    Unk80029538Lst *l;
    s32 *s;
    s32 *t;
    s32 y;
    s32 i;

    c->count = 0;
    c->one = 1;
    c->b[0] = a[0] - HALF_EXTENT;
    y = a[1];
    if (!(b->flags & FLAG_FIXED_HEIGHT)) {
        c->b[1] = y + b->sub->unk_40 / 2 - Y_LOW_OFFSET;
    } else {
        c->b[1] = y - Y_LOW_FIXED_OFFSET;
    }
    c->b[2] = a[2] - HALF_EXTENT;
    c->b[3] = a[0] + HALF_EXTENT;
    y = a[1];
    if (!(b->flags & FLAG_FIXED_HEIGHT)) {
        c->b[4] = y + b->sub->unk_40 / 2 + Y_LOW_OFFSET;
    } else {
        c->b[4] = y + Y_HIGH_FIXED_OFFSET;
    }
    c->b[5] = a[2] + HALF_EXTENT;
    l = *(Unk80029538Lst **)(D_800608CC + LIST_OFFSET);
    s = c->b;
    for (i = 0; i < l->n; i++) {
        if (*(s32 *)((char *)l + 0x1B4 + i * 4) & SLOT_ACTIVE) {
            t = *(s32 **)((char *)func_80014260((char *)l + 0x194 + i * 4, 1, 0) + 0x14);
            if (func_80030E1C(s, t) != 0) {
                c->count = func_8003BF90(t, s, &c->ent[c->count], c->count);
            }
        }
    }
    c->ent[c->count].id = LIST_END;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80026CA0", func_80029538);
#endif

void func_8002973C(s32 arg0, s32 arg1) {
    func_80029538(arg0 + 0x60, arg0, arg1);
}

extern s32 D_8005F390;
extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_80029768(void) {
    s16 *p;

    p = (s16 *)func_8001148C(0xBE4);
    D_8005F390 = (s32)p;
    D_8005F3D0 = (s32)p;
    if (p != 0) {
        p[1] = 0;
        p = (s16 *)func_8001148C(0xBE4);
        D_8005F398 = (s32)p;
        if (p != 0) {
            p[1] = 0;
            return -0xFF;
        }
    }
    return -0xF;
}

extern s32 D_8005F398;
extern s32 D_8005F3D0;

s32 func_800297C8(void) {
    func_800116C0(D_8005F3D0);
    func_800116C0(D_8005F398);
    return -0xFF;
}

u16 *func_80029800(u16 *arg0, s32 arg1, u16 *arg2, s32 arg3) {
    if (!(arg0[1] & 1)) {
        func_8003B940(arg0 + 2, arg2 + 2, arg0[0]);
        *arg2 = arg0[0];
        return arg2;
    }
    if (arg3 == 0) {
        func_8003B24C(arg0, arg1, arg2);
        return arg2;
    }
    func_8003AE54(arg0, arg1, arg2);
    return arg2;
}
