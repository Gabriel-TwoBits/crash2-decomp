#include "common.h"
#include "sdk.h"
#include "game.h"

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
extern s32 D_80069334;
extern s32 D_8006933C;
extern s32 D_80069340;
extern s32 D_80069344;
extern s32 D_80069348;
extern s32 D_8006934C;
extern s32 D_80069350;
extern s32 D_80069354;
extern u8 *D_80069358;

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
