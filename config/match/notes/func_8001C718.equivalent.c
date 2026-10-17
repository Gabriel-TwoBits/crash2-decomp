#include "common.h"
#include "sdk.h"
#include "game.h"

#define OBJ_STATE_ACTIVE 3
#define RESULT_NOT_ACTIVE (-0xFF)
#define RESULT_ERROR_LIMIT (-0xFE)
#define MASK_LOW24 0xFFFFFF
#define SHIFT_HIGH8 24
#define ARG_FULL_RANGE 0xFFFF

#define OFF_KIND 0x04
#define OFF_MODEL 0x0C
#define OFF_MODEL_INFO 0x10
#define OFF_VEC_A 0x60
#define OFF_VEC_B 0x6C
#define OFF_FLAGS_A8 0xA8
#define OFF_FLAGS_AC 0xAC
#define OFF_STACK_TOP 0xBC
#define OFF_SEL_C0 0xC0
#define OFF_SEL_C8 0xC8
#define OFF_TIME_DC 0xDC
#define OFF_COUNT_E4 0xE4
#define OFF_FIELD_E8 0xE8
#define OFF_FLAGS_100 0x100
#define OFF_FIELD_108 0x108
#define OFF_BYTE_120 0x120

#define AC_BIT_COUNTDOWN 0x10000000
#define AC_BIT_COPY_VEC 0x10
#define AC_BIT_SKIP 0x100
#define AC_BIT_ALT 0x2000000
#define A8_BIT_CLEAR 0x20
#define F100_BIT_FORCE 0x100000
#define F100_BIT_ALT 0x20000

#define PAD_ANY 0x8
#define PAD_FORCE 0x8000
#define PAD_FIRE 0x4
#define PAD_MODE_A 0x10
#define PAD_FORCE2 0x4000
#define PAD_HOLD 0x8000000

#define FIELD(o, off, T) (*(T *)((u8 *)(o) + (off)))
#define MODEL_KIND(o) FIELD(FIELD(FIELD(o, OFF_MODEL, u8 *), OFF_MODEL_INFO, u8 *), 4, u32)

extern u32 D_8006CB94;
extern s32 D_8006CB60;
extern s32 D_8005F180;
void func_8001D63C(void *, s32 *, s32 *);

s32 func_8001C718(u8 *obj, s32 flag) {
    s32 out[2];
    s32 vec[6];
    u32 pad;
    u32 flags100;
    u32 sel;
    u32 hit;
    s32 r;
    u32 top;

    if (FIELD(obj, OFF_KIND, s32) != OBJ_STATE_ACTIVE) {
        return RESULT_NOT_ACTIVE;
    }
    if (obj == (u8 *)D_8005F38C) {
        func_800158E0();
    } else {
    }
    D_8006CB60 = (s32)obj;
    pad = D_8006CB94;
    hit = pad & PAD_ANY;
    if (hit != 0) {
        flags100 = FIELD(obj, OFF_FLAGS_100, u32);
        if (flags100 & F100_BIT_FORCE) {
            flag = 1;
        } else {
        }
        if (((FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_ALT) || (flags100 & F100_BIT_ALT)) && (pad & PAD_FORCE)) {
            hit = 1;
        } else {
            sel = MODEL_KIND(obj);
            switch (sel) {
            case 0x100: hit = D_8006CB94 & 0x20; break;
            case 0x200: hit = D_8006CB94 & 0x100; break;
            case 0x300:
            case 0x500:
            case 0x600: hit = D_8006CB94 & 0x80; break;
            case 0x400:
            case 0x900: hit = D_8006CB94 & 0x400; break;
            default: break;
            }
        }
    } else {
    }
    if (flag != 0 && hit != 0) {
        if ((FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_COUNTDOWN) && FIELD(obj, OFF_COUNT_E4, s32) != 0) {
            s32 n = FIELD(obj, OFF_COUNT_E4, s32) - 1;
            FIELD(obj, OFF_COUNT_E4, s32) = n;
            if (n == 0) {
                FIELD(obj, OFF_FLAGS_AC, u32) &= ~AC_BIT_COUNTDOWN;
            } else {
            }
            func_8001AA80(obj, out);
        } else {
            FIELD(obj, OFF_TIME_DC, u32) = D_8006CB64;
            if (FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_COPY_VEC) {
                vec[0] = FIELD(obj, OFF_VEC_A, s32);
                vec[1] = FIELD(obj, OFF_VEC_A + 4, s32);
                vec[2] = FIELD(obj, OFF_VEC_A + 8, s32);
                vec[3] = FIELD(obj, OFF_VEC_B, s32);
                vec[4] = FIELD(obj, OFF_VEC_B + 4, s32);
                vec[5] = FIELD(obj, OFF_VEC_B + 8, s32);
            } else {
            }
            if (FIELD(obj, OFF_SEL_C8, s32) != 0) {
                func_8001C620(obj, 0, ARG_FULL_RANGE);
                FIELD(obj, OFF_SEL_C0, s32) = FIELD(obj, OFF_SEL_C8, s32);
                r = func_8003A014(obj, 3, 0);
                if (r >= RESULT_ERROR_LIMIT) {
                    return r;
                } else {
                }
            } else {
            }
            top = FIELD(obj, OFF_STACK_TOP, u32);
            sel = *(u32 *)(top - 4);
            if (D_8006CB64 - (sel & MASK_LOW24) >= (sel >> SHIFT_HIGH8)) {
                FIELD(obj, OFF_STACK_TOP, u32) = top - 4;
                r = func_8003A014(obj, 4, 0);
                if (r >= RESULT_ERROR_LIMIT) {
                    return r;
                } else {
                }
            } else {
            }
            ((s32 (*)(void *, s32))func_8001BFDC)(obj, flag);
            func_8001D63C(obj, vec, out);
            FIELD(obj, OFF_FLAGS_A8, u32) &= ~A8_BIT_CLEAR;
        }
    } else {
        func_8001AA80(obj, out);
    }
    if (FIELD(obj, OFF_FIELD_E8, s32) != 0 && FIELD(obj, OFF_BYTE_120, u8) != 1
        && !(FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_SKIP) && (D_8006CB94 & PAD_FIRE)) {
        pad = D_8006CB94;
        if (((FIELD(obj, OFF_FLAGS_AC, u32) & AC_BIT_ALT) || (FIELD(obj, OFF_FLAGS_100, u32) & F100_BIT_ALT))
            && (pad & PAD_FORCE2)) {
            hit = 1;
        } else {
            sel = MODEL_KIND(obj);
            hit = 0;
            switch (sel) {
            case 0x100:
                if (D_8006CB94 & PAD_MODE_A) {
                    if ((D_80060944 & 1) || D_8005F180 != 0 || FIELD(obj, OFF_FIELD_108, s32) != 4) {
                        hit = 1;
                    } else {
                    }
                } else {
                }
                break;
            case 0x200: hit = D_8006CB94 & 0x200; break;
            case 0x300:
            case 0x500:
            case 0x600: hit = D_8006CB94 & 0x40; break;
            case 0x400:
            case 0x900: hit = D_8006CB94 & 0x800; break;
            default: break;
            }
        }
        if (flag == 0 && !(D_8006CB94 & PAD_HOLD) && !(FIELD(obj, OFF_FLAGS_100, u32) & F100_BIT_FORCE)) {
            hit = 0;
        } else {
        }
        if (hit != 0) {
            func_8001ABF8(obj, out);
        } else {
        }
    } else {
    }
    D_8006CB60 = 0;
    return RESULT_NOT_ACTIVE;
}
