#include "common.h"
#include "sdk.h"
#include "game.h"

#define WORLD_FLAGS_OFFSET 0x29C
#define WORLD_FLAG_SKIP 0x100000
#define OBJ_FLAGS_OFFSET 0x100
#define OBJ_FLAG_SKIP 0x10000000
#define OBJ_FLAG_FIXED_BASE 0x40000000
#define OBJ_MODE_OFFSET 0x108
#define OBJ_MODE_MIN 3
#define OBJ_MODE_SPAN 2
#define OBJ_MODE_ALT 2
#define OBJ_BITS_OFFSET 0xB0
#define OBJ_BITS_MASK 2
#define OBJ_ALT_VALUE 2
#define OBJ_X_OFFSET 0x64
#define OBJ_X_END_DELTA 0x3200
#define OBJ_PARENT_OFFSET 0x8
#define PARENT_BASE_OFFSET 0x40
#define FIXED_BASE_OFFSET 0x29900
#define BUF_B_OFFSET 0x1008
#define TABLE_A_LAST 0x7C
#define TABLE_A_WORDS 32
#define TABLE_B_LAST 0x1FC
#define TABLE_B_WORDS 128

extern void *D_800608CC;
extern s32 D_8005F204;
extern s32 D_8005F378;
extern s32 *D_8005F3C4;

void func_8002D6E4(s32 *, s32 *, void *, void *, s32);
void func_8003C88C(void *, void *, s32, s32, s32, s32, s32, s32);

void func_8002DE08(s32 *arg0, s32 *arg1, void *arg2, void *arg3) {
    s32 *p;
    s32 i;
    s32 mode;
    s32 flags;
    s32 base;
    s32 x;

    if (!(*(s32 *)(*(s8 **)((s8 *)D_800608CC + 0x10) + WORLD_FLAGS_OFFSET) & WORLD_FLAG_SKIP)) {
        if (!(*(s32 *)((s8 *)arg2 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_SKIP)) {
            p = (s32 *)((s8 *)D_8005F3C4 + TABLE_A_LAST);
            for (i = TABLE_A_WORDS - 1; i >= 0; i--) {
                *p = 0;
                p--;
            }
            p = (s32 *)(D_8005F378 + TABLE_B_LAST);
            for (i = TABLE_B_WORDS - 1; i >= 0; i--) {
                *p = 0;
                p--;
            }
            mode = *(s32 *)((s8 *)arg2 + OBJ_MODE_OFFSET);
            flags = *(s32 *)((s8 *)arg2 + OBJ_BITS_OFFSET);
            if ((u32)(mode - OBJ_MODE_MIN) < OBJ_MODE_SPAN) {
                flags = OBJ_ALT_VALUE;
            } else if (mode == OBJ_MODE_ALT) {
                flags = OBJ_ALT_VALUE;
            } else {
                flags = flags & OBJ_BITS_MASK;
            }
            base = arg0[1];
            if (*(s32 *)((s8 *)arg2 + OBJ_FLAGS_OFFSET) & OBJ_FLAG_FIXED_BASE) {
                base += FIXED_BASE_OFFSET;
            } else {
                base += *(s32 *)(*(s8 **)((s8 *)arg2 + OBJ_PARENT_OFFSET) + PARENT_BASE_OFFSET);
            }
            x = *(s32 *)((s8 *)arg2 + OBJ_X_OFFSET);
            func_8003C88C(arg3, (s8 *)arg3 + BUF_B_OFFSET, flags, x + D_8005F204,
                          x + OBJ_X_END_DELTA, base, arg0[0], arg0[2]);
        }
    }
    func_8002D6E4(arg0, arg1, arg2, arg3, 1);
}
