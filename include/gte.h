#ifndef GTE_MODEL_H
#define GTE_MODEL_H

#include "common.h"

/* A model of the GTE (coprocessor 2) in C, for the equivalent C of the routines the game wrote with its instructions.
   Written from psx-spx's GTE chapter (docs/REFERENCES.md); tools/gte.py models the same commands, and tools/equiv.py
   runs the retail code on that model and the C on this one. The registers stay in the GTE: the GTE_READ and GTE_WRITE
   macros move one. With GTE_MODEL (equiv.py's trials, a native port) a command reads what it needs into a GteRegs, runs
   in C, and writes back what it changed; without it (the equivalent build) a command is the GTE's own instruction, a
   projection one instruction there where the model takes about 2,300. The model's functions are static, for equiv.py
   to run them as the candidate's own code: include it only under NON_MATCHING */

#define GTE_REGISTERS 32

/* Data registers */
#define GTE_VXY0 0
#define GTE_VZ0 1
#define GTE_VXY1 2
#define GTE_VZ1 3
#define GTE_VXY2 4
#define GTE_VZ2 5
#define GTE_IR0 8
#define GTE_IR1 9
#define GTE_IR2 10
#define GTE_IR3 11
#define GTE_SXY0 12
#define GTE_SXY1 13
#define GTE_SXY2 14
#define GTE_SZ0 16
#define GTE_SZ1 17
#define GTE_SZ2 18
#define GTE_SZ3 19
#define GTE_MAC0 24
#define GTE_MAC1 25
#define GTE_MAC2 26
#define GTE_MAC3 27

/* Control registers: the rotation matrix's nine halves in five words, low half first, and the translation */
#define GTE_RT11RT12 0
#define GTE_RT13RT21 1
#define GTE_RT22RT23 2
#define GTE_RT31RT32 3
#define GTE_RT33 4
/* The light matrix's five words, after the rotation matrix's and the translation */
#define GTE_L11L12 8
#define GTE_L13L21 9
#define GTE_L22L23 10
#define GTE_L31L32 11
#define GTE_L33 12
#define GTE_TRX 5
#define GTE_TRY 6
#define GTE_TRZ 7
#define GTE_OFX 24
#define GTE_OFY 25
#define GTE_H 26
#define GTE_DQA 27
#define GTE_DQB 28
#define GTE_FLAG 31

/* FLAG's bits, for MAC1 and IR1 (MAC2's, MAC3's, IR2's and IR3's follow, one lower each) and the rest; bit 31, the OR of
   bits 30-23 and 18-13, is the GTE's to set */
#define GTE_FLAG_MAC1_POSITIVE (1 << 30)
#define GTE_FLAG_MAC1_NEGATIVE (1 << 27)
#define GTE_FLAG_IR1 (1 << 24)
#define GTE_FLAG_SZ3 (1 << 18)
#define GTE_FLAG_DIVIDE (1 << 17)
#define GTE_FLAG_MAC0_POSITIVE (1 << 16)
#define GTE_FLAG_MAC0_NEGATIVE (1 << 15)
#define GTE_FLAG_SX2 (1 << 14)
#define GTE_FLAG_IR0 (1 << 12)
#define GTE_FLAG_ERROR 0x80000000

#define GTE_FRACTION_BITS 12
#define GTE_HALF_BITS 16
#define GTE_HALF_MASK 0xFFFF
#define GTE_HALF_SIGN 0x8000
/* MAC1-3 accumulate in 44 bits, checked after each addition */
#define GTE_MAC_LIMIT ((s64)1 << 43)
#define GTE_MAC_SPARE_BITS (64 - 44)
#define GTE_MAC0_LIMIT ((s64)1 << 31)
#define GTE_IR_MIN -0x8000
#define GTE_IR_MAX 0x7FFF
#define GTE_SZ_MAX 0xFFFF
#define GTE_SCREEN_MIN -0x400
#define GTE_SCREEN_MAX 0x3FF
#define GTE_IR0_MAX 0x1000
/* The perspective division: H / SZ3 as unsigned 1.16, by Newton-Raphson from a seed of 257 */
#define GTE_DIVIDE_MAX 0x1FFFF
#define GTE_DIVIDE_SEED_BIAS 0x101
#define GTE_DIVIDE_SEED_BASE 0x7FC0
#define GTE_DIVIDE_SEED_SHIFT 7
#define GTE_DIVIDE_TWO 0x2000080
#define GTE_DIVIDE_ROUND 0x80
#define GTE_DIVIDE_STEP_SHIFT 8
#define GTE_DIVIDE_FINAL_ROUND 0x8000
#define GTE_SEED_NUMERATOR 0x40000
#define GTE_SEED_OFFSET 0x100

/* RTPT's three vectors */
#define GTE_RTPT_VECTORS 3
/* A vector's and the rotation matrix's dimension; a vector's registers (VXYn, VZn), a word's halves, the screen's axes */
#define GTE_DIMENSION 3
#define GTE_VECTOR_REGISTERS 2
#define GTE_HALVES_PER_WORD 2
#define GTE_SCREEN_AXES 2

typedef struct GteRegs {
    u32 data[GTE_REGISTERS];
    u32 control[GTE_REGISTERS];
} GteRegs;

#define GTE_STRING(x) #x
#define GTE_NUMBER(x) GTE_STRING(x)
/* A register's number is part of the instruction: reg is one of the names above. mfc2 and cfc2 load with a delay;
   swc2 and lwc2 move a data register to memory and back */
#define GTE_READ_DATA(reg, out) __asm__ volatile("mfc2 %0, $" GTE_NUMBER(reg) "\n\tnop" : "=r"(out))
#define GTE_READ_CONTROL(reg, out) __asm__ volatile("cfc2 %0, $" GTE_NUMBER(reg) "\n\tnop" : "=r"(out))
#define GTE_WRITE_DATA(reg, value) __asm__ volatile("mtc2 %0, $" GTE_NUMBER(reg) : : "r"(value))
#define GTE_WRITE_CONTROL(reg, value) __asm__ volatile("ctc2 %0, $" GTE_NUMBER(reg) : : "r"(value))
#define GTE_STORE_DATA(reg, lvalue) __asm__ volatile("swc2 $" GTE_NUMBER(reg) ", %0" : "=m"(lvalue))
#define GTE_LOAD_DATA(reg, lvalue) __asm__ volatile("lwc2 $" GTE_NUMBER(reg) ", %0" : : "m"(lvalue))
#define GTE_SAVE_DATA(regs, reg) GTE_STORE_DATA(reg, (regs)->data[reg])
#define GTE_SAVE_CONTROL(regs, reg) GTE_READ_CONTROL(reg, (regs)->control[reg])
#define GTE_RESTORE_DATA(regs, reg) GTE_LOAD_DATA(reg, (regs)->data[reg])

/* A command's 25 bits after the COP2 opcode, as the GTE takes them */
#define GTE_RTPS_CODE 0x0180001
#define GTE_RTPT_CODE 0x0280030
/* MVMVA with sf = 1, nothing added and lm = 0; the matrix's field (rotation 0, light 1) and the vector's (V0 to V2, 3
   for IR1-IR3) */
#define GTE_MVMVA_CODE 0x0486012
#define GTE_MVMVA_MATRIX_SHIFT 17
#define GTE_MVMVA_VECTOR_SHIFT 15

#ifdef GTE_MODEL

/* What RTPS and RTPT read, into regs */
static void Gte_Save(GteRegs *regs) {
    GTE_SAVE_DATA(regs, GTE_VXY0);
    GTE_SAVE_DATA(regs, GTE_VZ0);
    GTE_SAVE_DATA(regs, GTE_VXY1);
    GTE_SAVE_DATA(regs, GTE_VZ1);
    GTE_SAVE_DATA(regs, GTE_VXY2);
    GTE_SAVE_DATA(regs, GTE_VZ2);
    GTE_SAVE_DATA(regs, GTE_SXY1);
    GTE_SAVE_DATA(regs, GTE_SXY2);
    GTE_SAVE_DATA(regs, GTE_SZ1);
    GTE_SAVE_DATA(regs, GTE_SZ2);
    GTE_SAVE_DATA(regs, GTE_SZ3);
    GTE_SAVE_CONTROL(regs, GTE_RT11RT12);
    GTE_SAVE_CONTROL(regs, GTE_RT13RT21);
    GTE_SAVE_CONTROL(regs, GTE_RT22RT23);
    GTE_SAVE_CONTROL(regs, GTE_RT31RT32);
    GTE_SAVE_CONTROL(regs, GTE_RT33);
    GTE_SAVE_CONTROL(regs, GTE_TRX);
    GTE_SAVE_CONTROL(regs, GTE_TRY);
    GTE_SAVE_CONTROL(regs, GTE_TRZ);
    GTE_SAVE_CONTROL(regs, GTE_OFX);
    GTE_SAVE_CONTROL(regs, GTE_OFY);
    GTE_SAVE_CONTROL(regs, GTE_H);
    GTE_SAVE_CONTROL(regs, GTE_DQA);
    GTE_SAVE_CONTROL(regs, GTE_DQB);
}

/* What RTPS and RTPT write, from regs */
static void Gte_Restore(GteRegs *regs) {
    GTE_RESTORE_DATA(regs, GTE_IR0);
    GTE_RESTORE_DATA(regs, GTE_IR1);
    GTE_RESTORE_DATA(regs, GTE_IR2);
    GTE_RESTORE_DATA(regs, GTE_IR3);
    GTE_RESTORE_DATA(regs, GTE_SXY0);
    GTE_RESTORE_DATA(regs, GTE_SXY1);
    GTE_RESTORE_DATA(regs, GTE_SXY2);
    GTE_RESTORE_DATA(regs, GTE_SZ0);
    GTE_RESTORE_DATA(regs, GTE_SZ1);
    GTE_RESTORE_DATA(regs, GTE_SZ2);
    GTE_RESTORE_DATA(regs, GTE_SZ3);
    GTE_RESTORE_DATA(regs, GTE_MAC0);
    GTE_RESTORE_DATA(regs, GTE_MAC1);
    GTE_RESTORE_DATA(regs, GTE_MAC2);
    GTE_RESTORE_DATA(regs, GTE_MAC3);
    GTE_WRITE_CONTROL(GTE_FLAG, regs->control[GTE_FLAG]);
}

/* RT(row+1)(column+1) */
static inline s32 Gte_Rotation(GteRegs *regs, s32 row, s32 column) {
    s32 index = row * GTE_DIMENSION + column;
    u32 word = regs->control[GTE_RT11RT12 + index / GTE_HALVES_PER_WORD];

    if (index % GTE_HALVES_PER_WORD != 0) {
        return (s16)(word >> GTE_HALF_BITS);
    } else {
        return (s16)word;
    }
}

/* A step of MAC1-3's accumulation: flagged past 43 bits either way, kept to 44 bits */
static inline s64 Gte_MacChecked(GteRegs *regs, s32 row, s64 value) {
    if (value >= GTE_MAC_LIMIT) {
        regs->control[GTE_FLAG] |= GTE_FLAG_MAC1_POSITIVE >> row;
    } else if (value < -GTE_MAC_LIMIT) {
        regs->control[GTE_FLAG] |= GTE_FLAG_MAC1_NEGATIVE >> row;
    } else {
        /* Within 44 bits */
    }
    return (value << GTE_MAC_SPARE_BITS) >> GTE_MAC_SPARE_BITS;
}

static s64 Gte_Mac0Checked(GteRegs *regs, s64 value) {
    if (value >= GTE_MAC0_LIMIT) {
        regs->control[GTE_FLAG] |= GTE_FLAG_MAC0_POSITIVE;
    } else if (value < -GTE_MAC0_LIMIT) {
        regs->control[GTE_FLAG] |= GTE_FLAG_MAC0_NEGATIVE;
    } else {
        /* Within 32 bits */
    }
    return value;
}

static s32 Gte_Saturated(GteRegs *regs, s32 value, s32 low, s32 high, u32 flag) {
    if (value < low) {
        regs->control[GTE_FLAG] |= flag;
        return low;
    } else if (value > high) {
        regs->control[GTE_FLAG] |= flag;
        return high;
    } else {
        return value;
    }
}

/* TRADEOFF: the division's seed computed each time; the GTE's 257-entry table if speed matters */
static inline u32 Gte_DivideSeed(u32 index) {
    s32 seed = (GTE_SEED_NUMERATOR / (index + GTE_SEED_OFFSET) + 1) / 2 - GTE_DIVIDE_SEED_BIAS;

    if (seed < 0) {
        return 0;
    } else {
        return seed;
    }
}

/* H / SZ3 as the GTE's division reckons it, flagged when H >= 2 * SZ3 */
static u32 Gte_Divide(GteRegs *regs, u32 h, u32 sz3) {
    s32 shift;
    u32 numerator;
    s32 divisor;
    s32 seed;
    u32 quotient;

    if (h >= sz3 * 2) {
        regs->control[GTE_FLAG] |= GTE_FLAG_DIVIDE;
        return GTE_DIVIDE_MAX;
    } else {
        /* SZ3 is at least 1 here: shifted until its top bit of 16 is set */
        shift = 0;
        while (((sz3 << shift) & GTE_HALF_SIGN) == 0) {
            shift++;
        }
        numerator = h << shift;
        divisor = sz3 << shift;
        seed = Gte_DivideSeed((divisor - GTE_DIVIDE_SEED_BASE) >> GTE_DIVIDE_SEED_SHIFT) + GTE_DIVIDE_SEED_BIAS;
        divisor = (GTE_DIVIDE_TWO - divisor * seed) >> GTE_DIVIDE_STEP_SHIFT;
        divisor = (GTE_DIVIDE_ROUND + divisor * seed) >> GTE_DIVIDE_STEP_SHIFT;
        quotient = ((u64)numerator * (u32)divisor + GTE_DIVIDE_FINAL_ROUND) >> GTE_HALF_BITS;
        if (quotient > GTE_DIVIDE_MAX) {
            return GTE_DIVIDE_MAX;
        } else {
            return quotient;
        }
    }
}

/* RTPS on vector index, as the game encodes it (sf = 1, lm = 0): rotated and translated into MAC1-3 and IR1-3, its Z
   pushed on the SZ FIFO, its screen position on the SXY FIFO; the depth cue into MAC0 and IR0 for the last vector only */
static void Gte_Perspective(GteRegs *regs, s32 index, s32 last) {
    s32 vector[GTE_DIMENSION];
    u32 xy;
    s32 row;
    s32 column;
    s64 value;
    s32 mac;
    s32 sz3;
    u32 quotient;
    s32 screen[GTE_SCREEN_AXES];
    s32 axis;

    xy = regs->data[GTE_VXY0 + index * GTE_VECTOR_REGISTERS];
    vector[0] = (s16)xy;
    vector[1] = (s16)(xy >> GTE_HALF_BITS);
    vector[2] = (s16)regs->data[GTE_VZ0 + index * GTE_VECTOR_REGISTERS];
    for (row = 0; row < GTE_DIMENSION; row++) {
        value = (s64)(s32)regs->control[GTE_TRX + row] << GTE_FRACTION_BITS;
        for (column = 0; column < GTE_DIMENSION; column++) {
            value = Gte_MacChecked(regs, row, value + Gte_Rotation(regs, row, column) * vector[column]);
        }
        mac = value >> GTE_FRACTION_BITS;
        regs->data[GTE_MAC1 + row] = mac;
        regs->data[GTE_IR1 + row] = Gte_Saturated(regs, mac, GTE_IR_MIN, GTE_IR_MAX, GTE_FLAG_IR1 >> row);
    }
    /* With sf = 1, SZ3 is MAC3 */
    sz3 = Gte_Saturated(regs, mac, 0, GTE_SZ_MAX, GTE_FLAG_SZ3);
    regs->data[GTE_SZ0] = regs->data[GTE_SZ1];
    regs->data[GTE_SZ1] = regs->data[GTE_SZ2];
    regs->data[GTE_SZ2] = regs->data[GTE_SZ3];
    regs->data[GTE_SZ3] = sz3;
    quotient = Gte_Divide(regs, regs->control[GTE_H] & GTE_HALF_MASK, sz3);
    for (axis = 0; axis < GTE_SCREEN_AXES; axis++) {
        value = Gte_Mac0Checked(regs, (s64)(s32)quotient * (s16)regs->data[GTE_IR1 + axis]
                                          + (s32)regs->control[GTE_OFX + axis]);
        regs->data[GTE_MAC0] = value;
        screen[axis] = Gte_Saturated(regs, value >> GTE_HALF_BITS, GTE_SCREEN_MIN, GTE_SCREEN_MAX,
                                     GTE_FLAG_SX2 >> axis);
    }
    regs->data[GTE_SXY0] = regs->data[GTE_SXY1];
    regs->data[GTE_SXY1] = regs->data[GTE_SXY2];
    regs->data[GTE_SXY2] = (screen[0] & GTE_HALF_MASK) | (screen[1] << GTE_HALF_BITS);
    if (last) {
        value = Gte_Mac0Checked(regs, (s64)(s32)quotient * (s16)regs->control[GTE_DQA] + (s32)regs->control[GTE_DQB]);
        regs->data[GTE_MAC0] = value;
        regs->data[GTE_IR0] = Gte_Saturated(regs, value >> GTE_FRACTION_BITS, 0, GTE_IR0_MAX, GTE_FLAG_IR0);
    } else {
        /* RTPT's first two vectors leave the depth cue alone */
    }
}

/* RTPS (on V0) or RTPT (on V0, V1 and V2) */
static void Gte_Project(s32 vectors) {
    GteRegs regs;
    s32 index;

    Gte_Save(&regs);
    regs.control[GTE_FLAG] = 0;
    for (index = 0; index < vectors; index++) {
        Gte_Perspective(&regs, index, index == vectors - 1);
    }
    Gte_Restore(&regs);
}

/* The rtps and rtpt instructions */
#define Gte_Rtps() Gte_Project(1)
#define Gte_Rtpt() Gte_Project(GTE_RTPT_VECTORS)

#else

/* The GTE's own commands, two instructions after the last register move as it needs (PsyQ's macros wait as long) */
#define GTE_COMMAND(code) __asm__ volatile("nop\n\tnop\n\tcop2 %0" : : "i"(code))
#define Gte_Rtps() GTE_COMMAND(GTE_RTPS_CODE)
#define Gte_Rtpt() GTE_COMMAND(GTE_RTPT_CODE)

#endif /* GTE_MODEL */

/* GCC 2.7.2 emits a static function used or not: a file defines GTE_MVMVA before including this to have MVMVA */
#ifdef GTE_MVMVA

/* MVMVA's matrices and vectors as the game encodes them (sf = 1, nothing added, lm = 0): the rotation or the light
   matrix, times V0, V1, V2 or IR1-IR3 */
#define GTE_MATRIX_ROTATION GTE_RT11RT12
#define GTE_MATRIX_LIGHT GTE_L11L12
#define GTE_VECTOR_IR 3
#define GTE_MATRIX_WORDS 5

#ifdef GTE_MODEL

/* Element (row+1)(column+1) of the matrix whose five words start at control register base */
static inline s32 Gte_MatrixElement(GteRegs *regs, s32 base, s32 row, s32 column) {
    s32 index = row * GTE_DIMENSION + column;
    u32 word = regs->control[base + index / GTE_HALVES_PER_WORD];

    if (index % GTE_HALVES_PER_WORD != 0) {
        return (s16)(word >> GTE_HALF_BITS);
    } else {
        return (s16)word;
    }
}

/* MVMVA: the matrix times the vector into MAC1-3 (>> 12) and IR1-3 (saturated) */
static void Gte_MultiplyMatrix(s32 matrix, s32 vector) {
    GteRegs regs;
    s32 values[GTE_DIMENSION];
    s32 row;
    s32 column;
    s64 value;
    s32 mac;
    u32 xy;

    GTE_SAVE_DATA(&regs, GTE_VXY0);
    GTE_SAVE_DATA(&regs, GTE_VZ0);
    GTE_SAVE_DATA(&regs, GTE_VXY1);
    GTE_SAVE_DATA(&regs, GTE_VZ1);
    GTE_SAVE_DATA(&regs, GTE_VXY2);
    GTE_SAVE_DATA(&regs, GTE_VZ2);
    GTE_SAVE_DATA(&regs, GTE_IR1);
    GTE_SAVE_DATA(&regs, GTE_IR2);
    GTE_SAVE_DATA(&regs, GTE_IR3);
    for (row = 0; row < GTE_MATRIX_WORDS; row++) {
        /* A register's number is part of the instruction: the two matrices' words one by one */
        switch (matrix + row) {
            case GTE_RT11RT12: GTE_SAVE_CONTROL(&regs, GTE_RT11RT12); break;
            case GTE_RT13RT21: GTE_SAVE_CONTROL(&regs, GTE_RT13RT21); break;
            case GTE_RT22RT23: GTE_SAVE_CONTROL(&regs, GTE_RT22RT23); break;
            case GTE_RT31RT32: GTE_SAVE_CONTROL(&regs, GTE_RT31RT32); break;
            case GTE_RT33: GTE_SAVE_CONTROL(&regs, GTE_RT33); break;
            case GTE_L11L12: GTE_SAVE_CONTROL(&regs, GTE_L11L12); break;
            case GTE_L13L21: GTE_SAVE_CONTROL(&regs, GTE_L13L21); break;
            case GTE_L22L23: GTE_SAVE_CONTROL(&regs, GTE_L22L23); break;
            case GTE_L31L32: GTE_SAVE_CONTROL(&regs, GTE_L31L32); break;
            case GTE_L33: GTE_SAVE_CONTROL(&regs, GTE_L33); break;
            default: /* Not a matrix word: the loop keeps to the five */ break;
        }
    }
    if (vector == GTE_VECTOR_IR) {
        values[0] = (s16)regs.data[GTE_IR1];
        values[1] = (s16)regs.data[GTE_IR2];
        values[2] = (s16)regs.data[GTE_IR3];
    } else {
        xy = regs.data[GTE_VXY0 + vector * GTE_VECTOR_REGISTERS];
        values[0] = (s16)xy;
        values[1] = (s16)(xy >> GTE_HALF_BITS);
        values[2] = (s16)regs.data[GTE_VZ0 + vector * GTE_VECTOR_REGISTERS];
    }
    regs.control[GTE_FLAG] = 0;
    for (row = 0; row < GTE_DIMENSION; row++) {
        value = 0;
        for (column = 0; column < GTE_DIMENSION; column++) {
            value = Gte_MacChecked(&regs, row, value + Gte_MatrixElement(&regs, matrix, row, column) * values[column]);
        }
        mac = value >> GTE_FRACTION_BITS;
        regs.data[GTE_MAC1 + row] = mac;
        regs.data[GTE_IR1 + row] = Gte_Saturated(&regs, mac, GTE_IR_MIN, GTE_IR_MAX, GTE_FLAG_IR1 >> row);
    }
    GTE_RESTORE_DATA(&regs, GTE_IR1);
    GTE_RESTORE_DATA(&regs, GTE_IR2);
    GTE_RESTORE_DATA(&regs, GTE_IR3);
    GTE_RESTORE_DATA(&regs, GTE_MAC1);
    GTE_RESTORE_DATA(&regs, GTE_MAC2);
    GTE_RESTORE_DATA(&regs, GTE_MAC3);
    GTE_WRITE_CONTROL(GTE_FLAG, regs.control[GTE_FLAG]);
}

#else

#define Gte_MultiplyMatrix(matrix, vector) \
    GTE_COMMAND(GTE_MVMVA_CODE | (((matrix) == GTE_MATRIX_LIGHT) << GTE_MVMVA_MATRIX_SHIFT) | \
                ((vector) << GTE_MVMVA_VECTOR_SHIFT))

#endif /* GTE_MODEL */

#endif /* GTE_MVMVA */

#endif
