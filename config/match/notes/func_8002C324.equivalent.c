#include "common.h"
#include "sdk.h"
#include "game.h"

extern s32 D_80062AD8;
extern u8 *D_8005F38C;

#define F(p, off) (*(s32 *)((u8 *)(p) + (off)))
#define SLOT_STRIDE 0x28
#define SLOT_BASE 0x18
#define POOL_NEXT_OFFSET (-4)
#define POOL_LIMIT_OFFSET 0x1410
#define POOL_MAX_ENTRIES 0x60
#define BOX_SIZE_PLAIN 0x20
#define BOX_SIZE_XFORM 0x44
#define SCALE_NUM 0xC8
#define SCALE_SHIFT 4
#define ANGLE_MASK 0xFFF
#define IDENTITY_SCALE 0x1000
#define FLAG_OFFSET_ON 0x200
#define OBJ_FLAG_SHIFT 0x20000000
#define SHIFT_NEG 0xFFFF5FD8
#define SHIFT_POS 0xA028

/* Builds a bounding box in the pool; pos/in/ext are arrays to keep the frame layout of the retail code. */
s32 *func_8002C324(void *arg0, s32 arg1, s32 arg2) {
    s32 pos[3];
    s32 in[3];
    s32 ext[9];
    u8 *obj = (u8 *)arg0;
    u8 *slot = 0;
    u8 *entry = 0;
    s32 idx = arg2;
    s32 kind;
    s32 *box;
    s32 ang;
    s32 a, b, c, d;
    s32 v;
    u32 next;
    u8 *flagp;

    if (idx == -2) {
        flagp = (u8 *)F(obj, 0xE8);
        if (flagp != 0 && *flagp == 1) {
            idx = 0;
        }
    }
    if (idx < 0) {
        kind = 0;
    } else {
        slot = (u8 *)F((u8 *)F(obj, 8), 0x20);
        if ((u32)idx >= (u32)F(slot, 0xC)) {
            return 0;
        }
        entry = slot + idx * SLOT_STRIDE + SLOT_BASE;
        kind = *(s32 *)entry & 7;
    }
    if (D_8006CB6C.n >= POOL_MAX_ENTRIES) {
        func_8002CA1C();
    }
    do {
        box = *(s32 **)((u8 *)&D_8006CB6C + POOL_NEXT_OFFSET);
        next = (u32)box + BOX_SIZE_PLAIN;
        if (kind != 0) {
            if (kind == 1) {
                next = (u32)box + BOX_SIZE_XFORM;
            } else {
                kind = 0;
            }
        }
    } while (next >= (u32)((u8 *)&D_8006CB6C + POOL_LIMIT_OFFSET));
    *(u32 *)((u8 *)&D_8006CB6C + POOL_NEXT_OFFSET) = next;
    D_8006CB6C.p[D_8006CB6C.n] = box;
    D_8006CB6C.n += 1;
    box[1] = (s32)arg0;
    box[0] = kind;

    if (slot == 0) {
        a = F(obj, 0x78);
        if (a < 0) a = -a;
        box[2] = F(obj, 0x60) - ((a * SCALE_NUM) >> SCALE_SHIFT);
        a = F(obj, 0x78);
        if (a < 0) a = -a;
        box[5] = F(obj, 0x60) + ((a * SCALE_NUM) >> SCALE_SHIFT);
        box[3] = F(obj, 0x64) - ((F(obj, 0x7C) * SCALE_NUM) >> SCALE_SHIFT);
        box[6] = F(obj, 0x64) + ((F(obj, 0x7C) * SCALE_NUM) >> SCALE_SHIFT);
        box[4] = F(obj, 0x68) - ((F(obj, 0x80) * SCALE_NUM) >> SCALE_SHIFT);
        box[7] = F(obj, 0x68) + ((F(obj, 0x80) * SCALE_NUM) >> SCALE_SHIFT);
        return box;
    }
    if (kind != 0) {
        if (kind == 1) {
            u8 *src = slot + idx * SLOT_STRIDE;
            box[2] = F(src, 0x28);
            box[3] = F(src, 0x2C);
            box[4] = F(src, 0x30);
            box[5] = F(src, 0x34);
            box[6] = F(src, 0x38);
            box[7] = F(src, 0x3C);
            box[8] = F(obj, 0x60);
            box[9] = F(obj, 0x64);
            box[10] = F(obj, 0x68);
            func_800309A8(box + 11, box + 2, (s16 *)F(obj, 8), box + 8);
            return box;
        }
        return box;
    }
    v = F(obj, 0x70);
    ang = v & ANGLE_MASK;
    if (ang == 0 && v == 0 && F(obj, 0x6C) == 0 && F(obj, 0x78) == IDENTITY_SCALE) {
        pos[0] = F(obj, 0x60) + F(entry, 4);
        pos[1] = F(obj, 0x64) + F(entry, 8);
        pos[2] = F(obj, 0x68) + F(entry, 0xC);
    } else {
        in[0] = F(entry, 4);
        in[1] = F(entry, 8);
        in[2] = F(entry, 0xC);
        func_8001E8E4(in, (s32 *)(obj + 0x60), (s32 *)(obj + 0x6C), (s32)(obj + 0x78), pos);
    }
    a = F(obj, 0x78);
    if (a < 0) a = -a;
    ext[6] = a;
    b = F(obj, 0x7C);
    ext[7] = b;
    c = F(obj, 0x80);
    ext[8] = c;
    d = (F(entry, 0x10) >> 8) * a >> SCALE_SHIFT;
    ext[0] = d;
    ext[1] = (F(entry, 0x14) >> 8) * b >> SCALE_SHIFT;
    ext[2] = (F(entry, 0x18) >> 8) * c >> SCALE_SHIFT;
    ext[3] = (F(entry, 0x1C) >> 8) * a >> SCALE_SHIFT;
    ext[4] = (F(entry, 0x20) >> 8) * b >> SCALE_SHIFT;
    ext[5] = (F(entry, 0x24) >> 8) * c >> SCALE_SHIFT;
    if ((u32)(ang - 0x200) >= 0xC01U) {
        box[2] = pos[0] + ext[0];
        box[3] = pos[1] + ext[1];
        box[4] = pos[2] + ext[2];
        box[5] = pos[0] + ext[3];
        box[6] = pos[1] + ext[4];
        v = pos[2] + ext[5];
    } else if (ang < 0x600) {
        box[2] = pos[0] + ext[2];
        box[5] = pos[0] + ext[5];
        box[3] = pos[1] + ext[1];
        box[6] = pos[1] + ext[4];
        box[4] = pos[2] - ext[3];
        v = pos[2] - ext[0];
    } else if (ang < 0xA00) {
        box[2] = pos[0] + ext[0];
        box[5] = pos[0] + ext[3];
        box[3] = pos[1] + ext[1];
        box[6] = pos[1] + ext[4];
        box[4] = pos[2] - ext[5];
        v = pos[2] - ext[2];
    } else {
        box[2] = pos[0] - ext[5];
        box[5] = pos[0] - ext[2];
        box[3] = pos[1] + ext[1];
        box[6] = pos[1] + ext[4];
        box[4] = pos[2] + ext[0];
        v = pos[2] + ext[3];
    }
    box[7] = v;
    if ((D_80062AD8 & FLAG_OFFSET_ON) && (F(D_8005F38C, 0x100) & OBJ_FLAG_SHIFT)) {
        box[2] += SHIFT_NEG;
        box[4] += SHIFT_NEG;
        box[5] += SHIFT_POS;
        box[7] += SHIFT_POS;
    }
    return box;
}
