/* The handwritten code's equivalent C (docs/WORKFLOW.md, "The handwritten code"). Only the equivalent build compiles
 * it: configure.py leaves each function defined here out of that build's copy of asm/game/80039D2C.s, and links this
 * file after it. The default build assembles the handwritten code as it is. */
#include "common.h"
#include "game.h"
#define GTE_MVMVA
#define GTE_DPCS
#include "gte.h"

#ifndef NON_MATCHING
#error "the handwritten code's C is the equivalent build's only"
#endif

/* Handwritten routines only a function written here reaches, such as the tails its dispatch jumps into: configure.py
 * leaves them out of the equivalent build with it (tools/handwritten.py) */
#define HANDWRITTEN_PARTS(function, parts...)

/* An angle's units in a turn, and the sine table's quarter wave (0x401 entries of round(4096 sin)) */
#define ANGLE_TURN 0x1000
#define ANGLE_MASK (ANGLE_TURN - 1)
#define ANGLE_QUARTER 0x400
#define ANGLE_HALF 0x800
#define ANGLE_THREE_QUARTERS 0xC00

extern s16 g_SinTable[];

/* The most halfwords a call to Sys_Memcpy copies: func_80029800 passes a u16 count */
#define MEMCPY_MAX_HALFWORDS 0xFFFF

/* The scratchpad's first 16 words: where handwritten functions save registers ($s0-$s4, up to all 16 they keep) on
 * entry and reload them before returning. Each of the 14 that use them writes them first (in their code's order, through
 * their absolute addresses), and no compiled code touches them */
#define SAVE_AREA 0x1F800000
#define SAVE_AREA_BYTES 0x40

/* A shift's count as the CPU takes it from a register: its low 5 bits */
#define SHIFT_COUNT_MASK 0x1F

/* An octree func_8003DA00 descends: its origin and size per axis, scaled by OCTREE_SHIFT when read, then its root node
 * and how many levels split each axis. A node is a halfword: 0 empty, odd a leaf (its kind in bits 1 to 3), else the
 * offset from the tree's start of its children, 2 to 8 halfwords by the axes it splits (x the highest bit of the index) */
typedef struct {
    s32 origin[3];
    u32 size[3];
    s32 unk_18;
    u16 root;
    u16 depth[3];
} Octree;

#define OCTREE_SHIFT 8
#define OCTREE_LEAF 1
#define OCTREE_LEAF_KIND 0xE
/* Two leaf kinds func_8003DA00 passes through when asked to */
#define OCTREE_KIND_SKIPPED_A 6
#define OCTREE_KIND_SKIPPED_B 8
/* func_8003DA00's flags: the height it moves the point to is y, not z; leaves of the skipped kinds count as empty */
#define OCTREE_ALONG_Y 1
#define OCTREE_SKIP_KINDS 8

/* The scratchpad's grid func_8003CBE4 marks: a bit per cell, two words a row (cells x -32 to -1 in the first, 0 to 31 in
 * the second) for rows y -32 to 31; where a cell is first marked, the rows from GRID_ROWS on get a shape of 32 words
 * OR-ed in, shifted by x */
#define GRID_CELLS 0x1F800180
#define GRID_ROWS 0x1F800100
#define GRID_SHAPE 0x1F800380
#define GRID_HALF 32
#define GRID_ROW_WORDS 2
/* func_8003C88C marks a box's edges every 8 cells; it keeps its flags and lower height in the scratchpad while it does */
#define GRID_EDGE_STEP 8
#define GRID_KEPT 0x1F8000E0
/* A box's corner in cells: its distance from the grid's base, in 1/2048ths (<< 2 then >> 13, signed) */
#define CELL_SCALE_SHIFT 2
#define CELL_SHIFT 13

/* A collision list (func_80029538's, at most 0x1008 bytes): records of 8 bytes. func_8003BF90 starts each zone's part
 * with a header (low half 0, 16 bytes: the zone's size per axis, then how often each axis halves), boxes follow, and
 * 0xFFFF ends the list. A box's first word holds its kind in bits 3 to 5 and its level in bits 0 to 2, and its corner's
 * offsets from the query box's near corner, as halves of 16ths */
#define RECORD_END 0xFFFF
#define RECORD_HEADER 0
#define RECORD_HALF_MASK 0xFFFF
#define RECORD_HALF_BITS 16
#define RECORD_KIND_SHIFT 2
#define RECORD_KIND_MASK 0xE
#define RECORD_LEVEL_MASK 7
#define RECORD_SIZE_SHIFT 8
#define RECORD_OFFSET_SHIFT 4
#define RECORD_WORDS 2
#define HEADER_WORDS 4
#define AXES 3
/* A kind as func_8003D040's callers number it, from 1 */
#define KIND_FROM_NUMBER(number) (((number) - 1) << 1)
#define NO_HEIGHT 0x7FFFFFFF

/* The fraction bits Math_SqrMagnitude2 and Math_SqrMagnitude3 drop from their sums of squares */
#define SQR_MAGNITUDE2_SHIFT 13
#define SQR_MAGNITUDE3_SHIFT 20

/* func_80041A78 takes two bits of its value a step, from the top */
#define SQRT_STEPS 16
#define SQRT_STEP_BITS 2
#define SQRT_TOP_SHIFT 30

/* Its callers pass 0x14 (Save_SortDirectory), a byte count halved (Gool_InitScratchpad) and a u16 (func_80029800); a
 * negative count jumps into code. Their buffers, never NULL and never overlapping: a card directory's entries and a
 * local, the opcode table and the scratchpad, an entity's item and a sort list (Cam_MoveAlongPath) */
EQUIV_RANGE(Sys_Memcpy, 2, 0, MEMCPY_MAX_HALFWORDS)
EQUIV_NONNULL(Sys_Memcpy, 0)
EQUIV_NONNULL(Sys_Memcpy, 1)
EQUIV_DISJOINT(Sys_Memcpy, 0, 1, 2, 2)
HANDWRITTEN_PARTS(Sys_Memcpy, Sys_Memcpy1, Sys_Memcpy2, Sys_Memcpy3, Sys_Memcpy4)

void Sys_Memcpy(void *src, void *dst, s32 halfwords) {
    u16 *from = src;
    u16 *to = dst;

    for (; halfwords > 0; halfwords--) {
        *to++ = *from++;
    }
}

/* Copies COUNT halfwords; nothing in the game calls it */
void func_8003BC34(u16 *from, u16 *to, s32 count) {
    for (; count != 0; count--) {
        *to++ = *from++;
    }
}

/* An ordering table's slots, linked each to the next (a 24-bit address), in blocks of 64; the last holds the end mark */
#define OT_BLOCK_SLOTS 64
#define OT_BLOCK_SHIFT 6
#define OT_LINK_MASK 0xFFFFFF

/* Every caller (Sys_MainLoop, Gfx_AllocFrameBuffers, func_80041E5C) passes a frame's table of 0x800 slots; the retail
 * code links at least a block, a count under 64 sending it on without end */
EQUIV_NONNULL(Gfx_ResetOt, 0)
EQUIV_RANGE(Gfx_ResetOt, 1, OT_BLOCK_SLOTS, OT_SLOTS)

/* Resets an ordering table of SLOTS slots: each linked to the one after it, the last the end */
void Gfx_ResetOt(u8 *ot, s32 slots) {
    u32 *slot = (u32 *)ot;
    u32 next = (u32)ot & OT_LINK_MASK;
    u32 blocks = (u32)slots >> OT_BLOCK_SHIFT;
    u32 index;

    for (index = 0; index < blocks * OT_BLOCK_SLOTS; index++) {
        next += sizeof(u32);
        slot[index] = next;
    }
    slot[index - 1] = OT_LINK_MASK;
}

extern u32 g_TimerTicks;

/* Root counter 2's interrupt (Sys_Init opens its event): the game's ticks */
void Sys_TimerHandler(void) {
    g_TimerTicks++;
}

s32 Math_SqrMagnitude2(s32 x, s32 y) {
    u64 sum = (s64)x * x + (s64)y * y;

    return sum >> SQR_MAGNITUDE2_SHIFT;
}

s32 Math_SqrMagnitude3(s32 x, s32 y, s32 z) {
    u64 sum = (s64)x * x + (s64)y * y + (s64)z * z;

    return sum >> SQR_MAGNITUDE3_SHIFT;
}

/* The sine and cosine, which Math_Sin and Math_Cos are and func_8003D3CC computes in line */
static s32 sine(s32 angle) {
    angle &= ANGLE_MASK;
    if (angle < ANGLE_QUARTER) {
        return g_SinTable[angle];
    } else if (angle < ANGLE_HALF) {
        return g_SinTable[ANGLE_HALF - angle];
    } else if (angle < ANGLE_THREE_QUARTERS) {
        return -g_SinTable[angle - ANGLE_HALF];
    } else {
        return -g_SinTable[ANGLE_TURN - angle];
    }
}

static s32 cosine(s32 angle) {
    angle &= ANGLE_MASK;
    if (angle < ANGLE_QUARTER) {
        return g_SinTable[ANGLE_QUARTER - angle];
    } else if (angle < ANGLE_HALF) {
        return -g_SinTable[angle - ANGLE_QUARTER];
    } else if (angle < ANGLE_THREE_QUARTERS) {
        return -g_SinTable[ANGLE_THREE_QUARTERS - angle];
    } else {
        return g_SinTable[angle - ANGLE_THREE_QUARTERS];
    }
}

s32 Math_Sin(s32 angle) {
    return sine(angle);
}

s32 Math_Cos(s32 angle) {
    return cosine(angle);
}

/* func_8003EAD4's vectors: three words, x, y and z; its rotation: the GTE's five words */
#define VECTOR_WORDS 3
#define MATRIX_WORDS 5

/* Its callers (func_8002A8D0, func_8002CC50) pass a box's eight corners, an object's rotation (a word: the matrix's
 * address) and its position */
#define BOX_CORNERS 8
EQUIV_NONNULL(func_8003EAD4, 3)
EQUIV_RANGE(func_8003EAD4, 4, 0, BOX_CORNERS)

/* Transforms COUNT vectors from IN to OUT: each shifted down by SHIFT, rotated by MATRIX (its five words into the GTE),
 * shifted back up and moved by TRANSLATION */
void func_8003EAD4(s32 *out, s32 *in, s32 matrix, void *translation, s32 count, s32 shift) {
    u32 *rotation = (u32 *)matrix;
    s32 *move = translation;
    u32 words[MATRIX_WORDS];
    s32 x, y, z, rx, ry, rz, tx, ty, tz, word;

    for (word = 0; word < MATRIX_WORDS; word++) {
        words[word] = rotation[word];
    }
    tx = move[0];
    ty = move[1];
    tz = move[2];
    GTE_WRITE_CONTROL(GTE_RT11RT12, words[0]);
    GTE_WRITE_CONTROL(GTE_RT13RT21, words[1]);
    GTE_WRITE_CONTROL(GTE_RT22RT23, words[2]);
    GTE_WRITE_CONTROL(GTE_RT31RT32, words[3]);
    GTE_WRITE_CONTROL(GTE_RT33, words[4]);
    for (; count > 0; count--) {
        x = in[0] >> (shift & SHIFT_COUNT_MASK);
        y = in[1] >> (shift & SHIFT_COUNT_MASK);
        z = in[2] >> (shift & SHIFT_COUNT_MASK);
        GTE_WRITE_DATA(GTE_VXY0, ((u32)y << GTE_HALF_BITS) | ((u32)x & GTE_HALF_MASK));
        GTE_WRITE_DATA(GTE_VZ0, z);
        in += VECTOR_WORDS;
        Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, 0);
        GTE_READ_DATA(GTE_IR1, rx);
        GTE_READ_DATA(GTE_IR2, ry);
        GTE_READ_DATA(GTE_IR3, rz);
        out[0] = (rx << (shift & SHIFT_COUNT_MASK)) + tx;
        out[1] = (ry << (shift & SHIFT_COUNT_MASK)) + ty;
        out[2] = (rz << (shift & SHIFT_COUNT_MASK)) + tz;
        out += VECTOR_WORDS;
    }
}

/* An object's matrix as func_8004400C and its entries build it: the rotation's five words as the GTE takes them, then
 * the scale per axis; 1.0 in the GTE's 4.12 */
#define FIXED_ONE 0x1000
#define MATRIX_SCALE MATRIX_WORDS

/* An angle's cosine and sine as halves of the GTE's words, from the quarter wave as func_80043270 reads it: the
 * cosine negated in the second and third quarters, the sine in the second half, a negated value masked to 16 bits and
 * one not as the halfword loads it */
#define ANGLE_IN_QUARTER (ANGLE_QUARTER - 1)

static u32 wave_half(s32 value, s32 negated) {
    if (negated) {
        return -value & GTE_HALF_MASK;
    } else {
        return value;
    }
}

static void cosine_sine(s32 angle, u32 *cosine_word, u32 *sine_word) {
    s32 index = angle & ANGLE_IN_QUARTER;
    s32 rising = g_SinTable[index];
    s32 falling = g_SinTable[ANGLE_QUARTER - index];
    s32 odd_quarter = (angle & ANGLE_QUARTER) != 0;
    s32 second_half = (angle & ANGLE_HALF) != 0;

    if (odd_quarter) {
        *cosine_word = wave_half(rising, !second_half);
        *sine_word = wave_half(falling, second_half);
    } else {
        *cosine_word = wave_half(falling, second_half);
        *sine_word = wave_half(rising, second_half);
    }
}

/* A rotation by ANGLE about the x, y or z axis, its five words */
static void rotation_x(u32 *words, s32 angle) {
    u32 c, s;

    cosine_sine(angle, &c, &s);
    words[0] = FIXED_ONE;
    words[1] = 0;
    words[2] = ((u32)-(s32)s << GTE_HALF_BITS) | c;
    words[3] = s << GTE_HALF_BITS;
    words[4] = c;
}

static void rotation_y(u32 *words, s32 angle) {
    u32 c, s;

    cosine_sine(angle, &c, &s);
    words[0] = c;
    words[1] = s;
    words[2] = FIXED_ONE;
    words[3] = (u32)-(s32)s & GTE_HALF_MASK;
    words[4] = c;
}

static void rotation_z(u32 *words, s32 angle) {
    u32 c, s;

    cosine_sine(angle, &c, &s);
    words[0] = (s << GTE_HALF_BITS) | c;
    words[1] = (u32)-(s32)s << GTE_HALF_BITS;
    words[2] = c;
    words[3] = 0;
    words[4] = FIXED_ONE;
}

/* The GTE's rotation set to the five words (func_80043074) */
static void rotation_load(u32 *words) {
    GTE_WRITE_CONTROL(GTE_RT11RT12, words[0]);
    GTE_WRITE_CONTROL(GTE_RT13RT21, words[1]);
    GTE_WRITE_CONTROL(GTE_RT22RT23, words[2]);
    GTE_WRITE_CONTROL(GTE_RT31RT32, words[3]);
    GTE_WRITE_CONTROL(GTE_RT33, words[4]);
}

/* The GTE's rotation times the matrix of the five words, column by column, into the words and the rotation
 * (func_80042FD0) */
static void rotation_multiply(u32 *words) {
    s32 x1, y1, z1, x2, y2, z2, x3, y3, z3;

    GTE_WRITE_DATA(GTE_VXY0, (words[0] & GTE_HALF_MASK) | (words[1] & ~GTE_HALF_MASK));
    GTE_WRITE_DATA(GTE_VZ0, words[3]);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, 0);
    GTE_WRITE_DATA(GTE_VXY1, (words[0] >> GTE_HALF_BITS) | (words[2] << GTE_HALF_BITS));
    GTE_WRITE_DATA(GTE_VZ1, words[3] >> GTE_HALF_BITS);
    GTE_READ_DATA(GTE_IR1, x1);
    GTE_READ_DATA(GTE_IR2, y1);
    GTE_READ_DATA(GTE_IR3, z1);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, 1);
    GTE_WRITE_DATA(GTE_VXY2, (words[1] & GTE_HALF_MASK) | (words[2] & ~GTE_HALF_MASK));
    GTE_WRITE_DATA(GTE_VZ2, words[4]);
    GTE_READ_DATA(GTE_IR1, x2);
    GTE_READ_DATA(GTE_IR2, y2);
    GTE_READ_DATA(GTE_IR3, z2);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, 2);
    GTE_READ_DATA(GTE_IR1, x3);
    GTE_READ_DATA(GTE_IR2, y3);
    GTE_READ_DATA(GTE_IR3, z3);
    words[0] = ((u32)x1 & GTE_HALF_MASK) | ((u32)x2 << GTE_HALF_BITS);
    words[1] = ((u32)y1 << GTE_HALF_BITS) | ((u32)x3 & GTE_HALF_MASK);
    words[2] = ((u32)y2 & GTE_HALF_MASK) | ((u32)y3 << GTE_HALF_BITS);
    words[3] = ((u32)z1 & GTE_HALF_MASK) | ((u32)z2 << GTE_HALF_BITS);
    words[4] = z3;
    rotation_load(words);
}

typedef void (*AxisTurn)(u32 *words, s32 angle);

/* MATRIX from a turn by FIRST about FIRST_AXIS, then by X about x and by LAST about LAST_AXIS, each when not 0, and the
 * scale (SCALE, times FACTORS in 4.12 when given): the turns' rotation, scaled unless the scale's words or together to
 * 1.0 */
static void turned_matrix(u32 *matrix, AxisTurn first_axis, s32 first, s32 x, AxisTurn last_axis, s32 last, s32 *scale,
                          s32 *factors) {
    u32 words[MATRIX_WORDS];
    u32 scale_x, scale_y, scale_z;

    first_axis(words, first);
    rotation_load(words);
    if (x != 0) {
        rotation_x(words, x);
        rotation_multiply(words);
    } else {
        /* No turn about x */
    }
    if (last != 0) {
        last_axis(words, last);
        rotation_multiply(words);
    } else {
        /* No last turn */
    }
    scale_x = scale[0];
    scale_y = scale[1];
    scale_z = scale[2];
    if (factors != 0) {
        scale_x = ((u32)factors[0] * scale_x) >> GTE_FRACTION_BITS;
        scale_y = ((u32)factors[1] * scale_y) >> GTE_FRACTION_BITS;
        scale_z = ((u32)factors[2] * scale_z) >> GTE_FRACTION_BITS;
    } else {
        /* The scale as it is */
    }
    matrix[MATRIX_SCALE] = scale_x;
    matrix[MATRIX_SCALE + 1] = scale_y;
    matrix[MATRIX_SCALE + 2] = scale_z;
    if ((scale_x | scale_y | scale_z) == FIXED_ONE) {
        GTE_READ_CONTROL(GTE_RT11RT12, matrix[0]);
        GTE_READ_CONTROL(GTE_RT13RT21, matrix[1]);
        GTE_READ_CONTROL(GTE_RT22RT23, matrix[2]);
        GTE_READ_CONTROL(GTE_RT31RT32, matrix[3]);
        GTE_READ_CONTROL(GTE_RT33, matrix[4]);
    } else {
        words[0] = scale_x;
        words[1] = 0;
        words[2] = scale_y;
        words[3] = 0;
        words[4] = scale_z;
        rotation_multiply(words);
        matrix[0] = words[0];
        matrix[1] = words[1];
        matrix[2] = words[2];
        matrix[3] = words[3];
        matrix[4] = words[4];
    }
}

/* func_8001AA80 calls them through the table at 0x8005B650 by an object's kind: its matrix from its angles (x, y, z)
 * and scale, the scale times FACTORS when given; each turns by the angles in its own order */
EQUIV_NONNULL(func_8004400C, 0)
EQUIV_NONNULL(func_8004400C, 1)
EQUIV_NONNULL(func_8004400C, 2)
EQUIV_NONNULL(func_80044178, 0)
EQUIV_NONNULL(func_80044178, 1)
EQUIV_NONNULL(func_80044178, 2)
EQUIV_NONNULL(func_8004427C, 0)
EQUIV_NONNULL(func_8004427C, 1)
EQUIV_NONNULL(func_8004427C, 2)
EQUIV_NONNULL(func_80044334, 0)
EQUIV_NONNULL(func_80044334, 1)
EQUIV_NONNULL(func_80044334, 2)
EQUIV_RUNS(func_8004400C, func_80042FD0, func_80043074, func_80043090, func_80043110, func_80043118)
EQUIV_RUNS(func_8004400C, func_80043270, func_800433A4, func_800435F4, func_80043658, func_800436C8)
EQUIV_RUNS(func_8004400C, func_800436F4, func_80043750, func_80043764, func_80043778, func_8004378C)
EQUIV_RUNS(func_8004400C, func_8004379C, func_800437BC, func_800437D8, func_800437F4, func_8004399C)
EQUIV_RUNS(func_8004400C, func_800439D4, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C)
EQUIV_RUNS(func_8004400C, func_80043C48, func_80043D34, func_80043D54, func_80043D84, func_80043EA0)
EQUIV_RUNS(func_8004400C, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_8004400C, func_80043504, func_8004351C, func_80043554, func_80043594)
EQUIV_RUNS(func_80044178, func_80042FD0, func_80043074, func_80043090, func_80043110, func_80043118)
EQUIV_RUNS(func_80044178, func_80043270, func_800433A4, func_800435F4, func_80043658, func_800436C8)
EQUIV_RUNS(func_80044178, func_800436F4, func_80043750, func_80043764, func_80043778, func_8004378C)
EQUIV_RUNS(func_80044178, func_8004379C, func_800437BC, func_800437D8, func_800437F4, func_8004399C)
EQUIV_RUNS(func_80044178, func_800439D4, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C)
EQUIV_RUNS(func_80044178, func_80043C48, func_80043D34, func_80043D54, func_80043D84, func_80043EA0)
EQUIV_RUNS(func_80044178, func_8004400C)
EQUIV_RUNS(func_80044178, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_80044178, func_80043504, func_8004351C, func_80043554, func_80043594)
EQUIV_RUNS(func_8004427C, func_80042FD0, func_80043074, func_80043090, func_80043110, func_80043118)
EQUIV_RUNS(func_8004427C, func_80043270, func_800433A4, func_800435F4, func_80043658, func_800436C8)
EQUIV_RUNS(func_8004427C, func_800436F4, func_80043750, func_80043764, func_80043778, func_8004378C)
EQUIV_RUNS(func_8004427C, func_8004379C, func_800437BC, func_800437D8, func_800437F4, func_8004399C)
EQUIV_RUNS(func_8004427C, func_800439D4, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C)
EQUIV_RUNS(func_8004427C, func_80043C48, func_80043D34, func_80043D54, func_80043D84, func_80043EA0)
EQUIV_RUNS(func_8004427C, func_8004400C, func_80044178)
EQUIV_RUNS(func_8004427C, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_8004427C, func_80043504, func_8004351C, func_80043554, func_80043594)
EQUIV_RUNS(func_80044334, func_80042FD0, func_80043074, func_80043090, func_80043110, func_80043118)
EQUIV_RUNS(func_80044334, func_80043270, func_800433A4, func_800435F4, func_80043658, func_800436C8)
EQUIV_RUNS(func_80044334, func_800436F4, func_80043750, func_80043764, func_80043778, func_8004378C)
EQUIV_RUNS(func_80044334, func_8004379C, func_800437BC, func_800437D8, func_800437F4, func_8004399C)
EQUIV_RUNS(func_80044334, func_800439D4, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C)
EQUIV_RUNS(func_80044334, func_80043C48, func_80043D34, func_80043D54, func_80043D84, func_80043EA0)
EQUIV_RUNS(func_80044334, func_8004400C, func_80044178, func_8004427C)
EQUIV_RUNS(func_80044334, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_80044334, func_80043504, func_8004351C, func_80043554, func_80043594)

/* z then x */
void func_8004400C(u32 *matrix, s32 *angles, s32 *scale, s32 *factors) {
    s32 x = angles[0];

    turned_matrix(matrix, rotation_z, angles[2], x, 0, 0, scale, factors);
}

/* y by the z angle, x, then y by the y angle less the z */
void func_80044178(u32 *matrix, s32 *angles, s32 *scale, s32 *factors) {
    s32 z = angles[2];
    s32 y = angles[1];

    turned_matrix(matrix, rotation_y, z, angles[0], rotation_y, y - z, scale, factors);
}

/* y, x, then z */
void func_8004427C(u32 *matrix, s32 *angles, s32 *scale, s32 *factors) {
    turned_matrix(matrix, rotation_y, angles[1], angles[0], rotation_z, angles[2], scale, factors);
}

/* z, x, then y */
void func_80044334(u32 *matrix, s32 *angles, s32 *scale, s32 *factors) {
    turned_matrix(matrix, rotation_z, angles[2], angles[0], rotation_y, angles[1], scale, factors);
}

void func_8003DD0C(void) {
}

/* The integer square root, digit by digit in base 4 */
s32 func_80041A78(s32 value) {
    u32 bits = value;
    s32 root = 0;
    s32 remainder = 0;
    s32 trial;
    s32 step;

    for (step = 0; step < SQRT_STEPS; step++) {
        remainder |= bits >> SQRT_TOP_SHIFT;
        trial = remainder - ((root << SQRT_STEP_BITS) + 1);
        root <<= 1;
        if (trial >= 0) {
            root++;
            remainder = trial;
        } else {
            /* The digit is 0: the remainder stays */
        }
        bits <<= SQRT_STEP_BITS;
        remainder <<= SQRT_STEP_BITS;
    }
    return root;
}

/* Every caller passes a tree and a point it holds; the save area is the handwritten code's (SAVE_AREA) */
EQUIV_SCRATCH(func_8003DA00, SAVE_AREA, SAVE_AREA_BYTES)

/* The leaf of an octree holding a point (each coordinate a word), or 0 when none does; the point's height (z, or y with
 * OCTREE_ALONG_Y) is moved by where the search stopped */
void *func_8003DA00(s32 tree, void *point, s32 flags) {
    Octree *octree = (Octree *)tree;
    s32 *at = point;
    s32 dx = (octree->origin[0] << OCTREE_SHIFT) - at[0];
    s32 dy = (octree->origin[1] << OCTREE_SHIFT) - at[1];
    s32 dz = (octree->origin[2] << OCTREE_SHIFT) - at[2];
    u32 sx = octree->size[0] << OCTREE_SHIFT;
    u32 sy = octree->size[1] << OCTREE_SHIFT;
    u32 sz = octree->size[2] << OCTREE_SHIFT;
    u32 node = octree->root;
    s32 level = 0;
    s32 split_x, split_y, split_z;
    s32 past_x, past_y, past_z;
    s32 kind;

    while (node != 0 && !(node & OCTREE_LEAF)) {
        split_x = level < octree->depth[0];
        split_y = level < octree->depth[1];
        split_z = level < octree->depth[2];
        sx >>= split_x;
        sy >>= split_y;
        sz >>= split_z;
        past_x = (s32)(dx + sx) < 0;
        past_y = (s32)(dy + sy) < 0;
        past_z = (s32)(dz + sz) < 0;
        if (past_x) {
            dx += sx;
        } else {
            /* In the lower half along x */
        }
        if (past_y) {
            dy += sy;
        } else {
            /* In the lower half along y */
        }
        if (past_z) {
            dz += sz;
        } else {
            /* In the lower half along z */
        }
        node = *(u16 *)((u8 *)octree + node + ((((past_x << split_y) << split_z) + (past_y << split_z) + past_z) << 1));
        level++;
    }

    kind = node & OCTREE_LEAF_KIND;
    if (node == 0) {
        /* No leaf: empty */
    } else if ((flags & OCTREE_SKIP_KINDS) && (kind == OCTREE_KIND_SKIPPED_A || kind == OCTREE_KIND_SKIPPED_B)) {
        node = 0;
    } else if (flags & OCTREE_ALONG_Y) {
        at[1] += dy + sy;
        return (void *)node;
    } else {
        at[2] += dz - 1;
        return (void *)node;
    }
    if (flags & OCTREE_ALONG_Y) {
        at[1] += dy - 1;
    } else {
        at[2] += dz - 1;
    }
    return (void *)node;
}

/* A collision list's records before its end mark (0x1008 bytes); func_8003BF90 leaves out the leaves past them */
#define RECORDS_MAX 0x200
/* A box's first word keeps its leaf's bits from 1 up, past the level */
#define RECORD_LEAF_MASK 0xFFF8
/* A record's halfwords, and the records a header takes */
#define RECORD_HALVES 4
#define HEADER_RECORDS 2
/* The most times an octree halves an axis: the retail code keeps the nodes of 7 levels above a leaf in registers and
 * breaks past them; the disc's 1,843 zones halve x and y at most 7 times, z 6 (docs/TARGET.md) */
#define OCTREE_HALVINGS_MAX 7

/* func_8003BF90's walk: the tree, the query box's far corner from its near one, where the next record goes */
typedef struct {
    Octree *octree;
    u16 depth[AXES];
    s32 extent[AXES];
    u32 *record;
    s32 count;
} OctreeQuery;

/* Lists the leaves under a node that overlap the query box: a node's corner is from the box's near corner, in the box's
 * units (the octree's shifted by OCTREE_SHIFT) */
static void query_node(OctreeQuery *query, u32 node, s32 level, s32 *corner, u32 *size) {
    s32 split[AXES];
    u32 half[AXES];
    s32 child_corner[AXES];
    s32 splits = 0;
    s32 axis, child, shift, middle, overlaps;
    u16 *children;

    if (node == 0) {
        return;
    } else if (node & OCTREE_LEAF) {
        if (query->count < RECORDS_MAX) {
            query->record[0] = ((node << RECORD_KIND_SHIFT) & RECORD_LEAF_MASK) | level |
                               (((u32)corner[0] >> RECORD_OFFSET_SHIFT) << RECORD_HALF_BITS);
            query->record[1] = (((u32)corner[2] >> RECORD_OFFSET_SHIFT) << RECORD_HALF_BITS) |
                               (((u32)corner[1] >> RECORD_OFFSET_SHIFT) & RECORD_HALF_MASK);
            query->record += RECORD_WORDS;
            query->count++;
        } else {
            /* The list is full */
        }
        return;
    } else {
        /* Children: one for each half of the axes this level splits */
    }

    for (axis = 0; axis < AXES; axis++) {
        split[axis] = level < query->depth[axis];
        splits += split[axis];
        half[axis] = size[axis] >> split[axis];
    }
    /* The retail code loops forever on a node with children where no axis splits; no zone on the disc has one */
    EQUIV_ASSUME(splits != 0);
    if (splits == 0) {
        return;
    } else {
        /* At least two children */
    }
    children = (u16 *)((u8 *)query->octree + node);
    for (child = 0; child < (1 << splits); child++) {
        overlaps = 1;
        shift = splits;
        for (axis = 0; axis < AXES; axis++) {
            child_corner[axis] = corner[axis];
            if (!split[axis]) {
                continue;
            } else {
                /* The child's bit for this axis, x the highest */
                shift--;
            }
            middle = corner[axis] + half[axis];
            if ((child >> shift) & 1) {
                child_corner[axis] = middle;
                overlaps &= middle <= query->extent[axis];
            } else {
                overlaps &= middle >= 0;
            }
        }
        if (overlaps) {
            query_node(query, children[child], level + 1, child_corner, half);
        } else {
            /* Outside the box along some axis */
        }
    }
}

/* Every caller (func_80029538) passes a zone's octree, a box around an object and a record in its collision list; the
 * octree halves no axis more than OCTREE_HALVINGS_MAX times (the depths' offsets in Octree) */
EQUIV_SCRATCH(func_8003BF90, SAVE_AREA, SAVE_AREA_BYTES)
EQUIV_NONNULL(func_8003BF90, 0)
EQUIV_NONNULL(func_8003BF90, 1)
EQUIV_NONNULL(func_8003BF90, 2)
EQUIV_HALF(func_8003BF90, 0, 0x1E, 0, OCTREE_HALVINGS_MAX)
EQUIV_HALF(func_8003BF90, 0, 0x20, 0, OCTREE_HALVINGS_MAX)
EQUIV_HALF(func_8003BF90, 0, 0x22, 0, OCTREE_HALVINGS_MAX)

/* Writes a zone's part of a collision list at RECORDS, COUNT of its records before it: the zone's header, then a box for
 * each leaf of its octree overlapping BOX (its least corner then its greatest). Returns the list's records after it */
s32 func_8003BF90(s32 *tree, s32 *box, void *records, s32 count) {
    OctreeQuery query;
    u16 *header = records;
    s32 corner[AXES];
    u32 size[AXES];
    u32 root;
    s32 axis;

    query.octree = (Octree *)tree;
    for (axis = 0; axis < AXES; axis++) {
        query.extent[axis] = box[AXES + axis] - box[axis];
        corner[axis] = (query.octree->origin[axis] << OCTREE_SHIFT) - box[axis];
        size[axis] = query.octree->size[axis];
    }
    root = query.octree->root;
    for (axis = 0; axis < AXES; axis++) {
        query.depth[axis] = query.octree->depth[axis];
    }

    header[0] = RECORD_HEADER;
    header[RECORD_HALVES] = RECORD_HEADER;
    for (axis = 0; axis < AXES; axis++) {
        header[1 + axis] = size[axis];
        header[RECORD_HALVES + 1 + axis] = query.depth[axis];
        size[axis] <<= OCTREE_SHIFT;
    }
    query.record = (u32 *)records + HEADER_WORDS;
    query.count = count + HEADER_RECORDS;
    query_node(&query, root, 0, corner, size);
    return query.count;
}

/* func_8003CBF0, which func_8003CBE4 runs into and func_8003C88C calls with x and y in $v1 and $a3 */
static void mark_cell(s32 x, s32 y) {
    u32 *cells = (u32 *)GRID_CELLS + (y + GRID_HALF) * GRID_ROW_WORDS;
    u32 *shape = (u32 *)GRID_SHAPE;
    u32 *rows;
    u32 bit;
    s32 first, last;

    if (x < 0) {
        bit = 1 << (x + GRID_HALF);
    } else {
        cells++;
        bit = 1 << x;
    }
    if (*cells & bit) {
        return;
    }
    *cells |= bit;
    if (y < 0) {
        first = -y;
        last = GRID_HALF;
        y = 0;
    } else {
        first = 0;
        last = GRID_HALF - y;
    }
    rows = (u32 *)GRID_ROWS + y;
    for (; first < last; first++, rows++) {
        if (x < 0) {
            *rows |= shape[first] << -x;
        } else {
            *rows |= shape[first] >> x;
        }
    }
}

EQUIV_RUNS(func_8003CBE4, func_8003CBF0)

void func_8003CBE4(s32 x, s32 y) {
    mark_cell(x, y);
}

EQUIV_SCRATCH(func_8003D040, SAVE_AREA, SAVE_AREA_BYTES)

/* Of the boxes of three kinds in a collision list whose corners, at an origin, overlap a query box (its least corner
 * then its greatest), the lowest y (lowest set) or their y's average; none when no box does. The first argument goes
 * unread */
s32 func_8003D040(void *unused, void *records, void *origin, s32 *box, s32 kind_a, s32 kind_b, s32 kind_c, s32 none,
                  s32 lowest) {
    u32 *record = records;
    s32 *at = origin;
    s32 kinds[3];
    /* Set in the code, not by an initialiser: GCC would copy that from .rodata, which moves all the code after it */
    u32 size[AXES];
    s32 halvings[AXES];
    s32 hits = 0;
    s32 sum = 0;
    s32 least = NO_HEIGHT;
    s32 corner[AXES];
    s32 kind, level, axis, shift;
    u32 first, second;

    for (axis = 0; axis < AXES; axis++) {
        size[axis] = 0;
        halvings[axis] = 0;
    }
    kinds[0] = KIND_FROM_NUMBER(kind_a);
    kinds[1] = KIND_FROM_NUMBER(kind_b);
    kinds[2] = KIND_FROM_NUMBER(kind_c);
    for (;;) {
        first = record[0];
        second = record[1];
        if ((first & RECORD_HALF_MASK) == RECORD_END) {
            break;
        } else if ((first & RECORD_HALF_MASK) == RECORD_HEADER) {
            size[0] = (first >> RECORD_HALF_BITS) << RECORD_SIZE_SHIFT;
            size[1] = (second & RECORD_HALF_MASK) << RECORD_SIZE_SHIFT;
            size[2] = (second >> RECORD_HALF_BITS) << RECORD_SIZE_SHIFT;
            halvings[0] = record[2] >> RECORD_HALF_BITS;
            halvings[1] = record[3] & RECORD_HALF_MASK;
            halvings[2] = record[3] >> RECORD_HALF_BITS;
            record += HEADER_WORDS;
            continue;
        } else {
            /* A box */
            record += RECORD_WORDS;
        }
        kind = (first >> RECORD_KIND_SHIFT) & RECORD_KIND_MASK;
        if (kind != kinds[0] && kind != kinds[1] && kind != kinds[2]) {
            continue;
        }
        level = first & RECORD_LEVEL_MASK;
        corner[0] = at[0] + ((s16)(first >> RECORD_HALF_BITS) << RECORD_OFFSET_SHIFT);
        corner[1] = at[1] + ((s16)second << RECORD_OFFSET_SHIFT);
        corner[2] = at[2] + ((s16)(second >> RECORD_HALF_BITS) << RECORD_OFFSET_SHIFT);
        if (box[3] < corner[0] || box[4] < corner[1] || box[5] < corner[2]) {
            continue;
        }
        for (axis = 0; axis < AXES; axis++) {
            if (halvings[axis] < level) {
                shift = halvings[axis];
            } else {
                shift = level;
            }
            if (corner[axis] + (s32)(size[axis] >> shift) < box[axis]) {
                break;
            } else {
                /* Overlaps along this axis */
            }
        }
        if (axis < AXES) {
            continue;
        }
        hits++;
        sum += corner[1];
        if (corner[1] < least) {
            least = corner[1];
        } else {
            /* Not lower than the lowest so far */
        }
    }
    if (hits == 0) {
        return none;
    } else if (lowest) {
        return least;
    } else {
        return sum / hits;
    }
}

/* The box kinds func_8003C88C passes over, the two it measures from its first lower height, and the subkinds it leaves
 * unless flags are set (the box's first word, bits 6 to 11) */
#define EDGE_KIND_SHIFT 1
#define EDGE_KIND_MASK 7
#define EDGE_SKIPPED_A 3
#define EDGE_SKIPPED_B 4
#define EDGE_LOW_A 1
#define EDGE_LOW_B 5
#define EDGE_SUBKIND_SHIFT 6
#define EDGE_SUBKIND_MASK 0x3F
#define EDGE_SUBKIND_LIMIT 0x1B

static s32 cell(s32 value, s32 base) {
    return (s32)((u32)(value - base) << CELL_SCALE_SHIFT) >> CELL_SHIFT;
}

EQUIV_SCRATCH(func_8003C88C, SAVE_AREA, SAVE_AREA_BYTES)
EQUIV_RUNS(func_8003C88C, func_8003CBE4)
HANDWRITTEN_PARTS(func_8003C88C, func_8003CBF0)

/* Marks in the scratchpad's grid (func_8003CBE4) the edges, every 8 cells, of the boxes of a collision list whose
 * heights overlap one span (from low_a for kinds 1 and 5, from low_b for the others, up to high), their corners in cells
 * from base_x and base_z */
void func_8003C88C(void *records, void *origin, s32 flags, s32 low_a, s32 low_b, s32 high, s32 base_x, s32 base_z) {
    u32 *record = records;
    s32 *at = origin;
    s32 *kept = (s32 *)GRID_KEPT;
    /* Set in the code, not by an initialiser: GCC would copy that from .rodata, which moves all the code after it */
    u32 size[AXES];
    s32 halvings[AXES];
    s32 least[AXES], most[AXES];
    s32 x_least, x_most, z_least, z_most, x, z, low;
    s32 kind, subkind, level, axis, shift;
    u32 first, second;

    for (axis = 0; axis < AXES; axis++) {
        size[axis] = 0;
        halvings[axis] = 0;
    }
    for (;;) {
        first = record[0];
        second = record[1];
        if ((first & RECORD_HALF_MASK) == RECORD_END) {
            break;
        } else if ((first & RECORD_HALF_MASK) == RECORD_HEADER) {
            size[0] = (first >> RECORD_HALF_BITS) << RECORD_SIZE_SHIFT;
            size[1] = (second & RECORD_HALF_MASK) << RECORD_SIZE_SHIFT;
            size[2] = (second >> RECORD_HALF_BITS) << RECORD_SIZE_SHIFT;
            halvings[0] = record[2] >> RECORD_HALF_BITS;
            halvings[1] = record[3] & RECORD_HALF_MASK;
            halvings[2] = record[3] >> RECORD_HALF_BITS;
            record += HEADER_WORDS;
            continue;
        } else {
            /* A box */
            record += RECORD_WORDS;
        }
        level = first & RECORD_LEVEL_MASK;
        least[0] = at[0] + ((s16)(first >> RECORD_HALF_BITS) << RECORD_OFFSET_SHIFT);
        least[1] = at[1] + ((s16)second << RECORD_OFFSET_SHIFT);
        least[2] = at[2] + ((s16)(second >> RECORD_HALF_BITS) << RECORD_OFFSET_SHIFT);
        for (axis = 0; axis < AXES; axis++) {
            if (halvings[axis] < level) {
                shift = halvings[axis];
            } else {
                shift = level;
            }
            most[axis] = least[axis] + (size[axis] >> shift);
        }
        kind = ((first >> RECORD_KIND_SHIFT) & RECORD_KIND_MASK) >> EDGE_KIND_SHIFT;
        subkind = (first >> EDGE_SUBKIND_SHIFT) & EDGE_SUBKIND_MASK;
        if (kind == EDGE_SKIPPED_A || kind == EDGE_SKIPPED_B) {
            continue;
        } else if (kind == EDGE_LOW_A || kind == EDGE_LOW_B) {
            low = low_a;
        } else if (subkind != 0 && subkind < EDGE_SUBKIND_LIMIT && flags == 0) {
            continue;
        } else {
            low = low_b;
        }
        if (!(low < most[1] && least[1] < high)) {
            continue;
        }

        kept[0] = flags;
        kept[1] = low_a;
        x_least = cell(least[0], base_x);
        x_most = cell(most[0], base_x);
        z_least = cell(least[2], base_z);
        z_most = cell(most[2], base_z);
        if (-GRID_HALF < z_least && z_least < GRID_HALF) {
            for (x = x_least;; x += GRID_EDGE_STEP) {
                if (x <= -GRID_HALF) {
                    continue;
                } else if (x >= GRID_HALF || x >= x_most) {
                    break;
                } else {
                    mark_cell(x, z_least);
                }
            }
        }
        if (-GRID_HALF < x_most && x_most < GRID_HALF) {
            for (z = z_least;; z += GRID_EDGE_STEP) {
                if (z <= -GRID_HALF) {
                    continue;
                } else if (z >= GRID_HALF || z >= z_most) {
                    break;
                } else {
                    mark_cell(x_most, z);
                }
            }
        }
        if (-GRID_HALF < z_most && z_most < GRID_HALF) {
            for (x = x_most;; x -= GRID_EDGE_STEP) {
                if (x >= GRID_HALF) {
                    continue;
                } else if (x <= -GRID_HALF || x <= x_least) {
                    break;
                } else {
                    mark_cell(x, z_most);
                }
            }
        }
        if (x_least < GRID_HALF && -GRID_HALF < x_least) {
            for (z = z_most;; z -= GRID_EDGE_STEP) {
                if (z >= GRID_HALF) {
                    continue;
                } else if (z <= -GRID_HALF || z <= z_least) {
                    break;
                } else {
                    mark_cell(x_least, z);
                }
            }
        }
    }
}

/* func_8003D3CC's object: its position, angles and scale (words); the camera's position and angles; the depth beyond
 * which no object is drawn, and the angles' correction toward the camera at most */
#define VIEW_SHIFT 8
#define VIEW_FAR 0x2EE0
#define CAMERA_TURN_LIMIT 2000
#define CAMERA_ANGLE_BITS 11
#define CAMERA_ROLL_BITS 12
#define WORD_BITS 32
/* The last rows of the matrix it leaves are 5/8 of what the scale makes, negated */
#define ROW_SCALE_SHIFT 2
#define ROW_SCALE_DIVISOR_SHIFT 3

typedef struct {
    s32 pos[3];
    s32 angle[3];
    s32 scale[3];
} ViewObject;

typedef struct {
    s32 pos[3];
    s32 angle[3];
} ViewCamera;

static s32 clamped_turn(s32 angle) {
    s32 turn = -angle;

    if (angle < -CAMERA_TURN_LIMIT) {
        return CAMERA_TURN_LIMIT;
    } else if (turn < -CAMERA_TURN_LIMIT) {
        return -CAMERA_TURN_LIMIT;
    } else {
        return turn;
    }
}

/* MVMVA of the rotation matrix diag(a, b, c) (the off-diagonal words zero) times V0, as func_8003D3CC uses the GTE to
 * multiply: the diagonal's words written as it writes them */
static void multiply_diagonal(u32 first, u32 second, u32 third) {
    GTE_WRITE_CONTROL(GTE_RT11RT12, first);
    GTE_WRITE_CONTROL(GTE_RT22RT23, second);
    GTE_WRITE_CONTROL(GTE_RT33, third);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, 0);
}

EQUIV_SCRATCH(func_8003D3CC, SAVE_AREA, SAVE_AREA_BYTES)

/* Sets the GTE up to draw an object: the translation (its position, or with on_screen its offset from the camera seen
 * through the view matrix, 0 when that is not between near and VIEW_FAR or far), and a rotation from its angles, each
 * turned toward the camera's but for on_screen, times its scale >> scale_shift. 0 when culled, else 1; the last argument
 * goes unread */
s32 func_8003D3CC(void *object, void *camera, s32 on_screen, s32 scale_shift, void *view, s32 near, s32 far,
                  s32 unused) {
    ViewObject *o = object;
    ViewCamera *c = camera;
    u32 *matrix = view;
    s32 x, y, z, yaw, pitch, roll;
    s32 sin_yaw, sin_pitch, sin_roll, cos_yaw, cos_pitch, cos_roll;
    s32 product, a2, a3, b1, b2, b3, c1, c2, c3, d, e, f, g;
    s32 u1, u2, u3, w1, w2, w3;

    GTE_WRITE_CONTROL(GTE_RT11RT12, matrix[0]);
    GTE_WRITE_CONTROL(GTE_RT13RT21, matrix[1]);
    GTE_WRITE_CONTROL(GTE_RT22RT23, matrix[2]);
    GTE_WRITE_CONTROL(GTE_RT31RT32, matrix[3]);
    GTE_WRITE_CONTROL(GTE_RT33, matrix[4]);
    if (on_screen) {
        GTE_WRITE_CONTROL(GTE_TRX, o->pos[0] >> VIEW_SHIFT);
        GTE_WRITE_CONTROL(GTE_TRY, -o->pos[1] >> VIEW_SHIFT);
        GTE_WRITE_CONTROL(GTE_TRZ, near);
    } else {
        GTE_WRITE_DATA(GTE_IR1, (o->pos[0] - c->pos[0]) >> VIEW_SHIFT);
        GTE_WRITE_DATA(GTE_IR2, (o->pos[1] - c->pos[1]) >> VIEW_SHIFT);
        GTE_WRITE_DATA(GTE_IR3, (o->pos[2] - c->pos[2]) >> VIEW_SHIFT);
        Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, GTE_VECTOR_IR);
        GTE_READ_DATA(GTE_IR1, x);
        GTE_READ_DATA(GTE_IR2, y);
        GTE_READ_DATA(GTE_IR3, z);
        GTE_WRITE_CONTROL(GTE_TRX, x);
        GTE_WRITE_CONTROL(GTE_TRY, y);
        GTE_WRITE_CONTROL(GTE_TRZ, z);
        if (!(near < z) || VIEW_FAR < z || (far != 0 && far < z)) {
            return 0;
        }
    }

    pitch = o->angle[0];
    yaw = o->angle[1];
    roll = o->angle[2];
    if (!on_screen) {
        pitch += clamped_turn((s32)(c->angle[0] << (WORD_BITS - CAMERA_ANGLE_BITS)) >> (WORD_BITS - CAMERA_ANGLE_BITS));
        yaw += clamped_turn((s32)(c->angle[1] << (WORD_BITS - CAMERA_ANGLE_BITS)) >> (WORD_BITS - CAMERA_ANGLE_BITS));
        roll -= (s32)(c->angle[2] << (WORD_BITS - CAMERA_ROLL_BITS)) >> (WORD_BITS - CAMERA_ROLL_BITS);
    } else {
        /* Drawn as it is */
    }
    sin_roll = sine(roll);
    sin_pitch = sine(pitch);
    sin_yaw = sine(yaw);
    cos_roll = cosine(roll);
    cos_pitch = cosine(pitch);
    cos_yaw = cosine(yaw);

    /* V0 = (sin yaw, cos yaw, cos roll); the matrix's other words zero */
    GTE_WRITE_CONTROL(GTE_RT13RT21, 0);
    GTE_WRITE_CONTROL(GTE_RT31RT32, 0);
    GTE_WRITE_DATA(GTE_VZ0, cos_roll & GTE_HALF_MASK);
    GTE_WRITE_DATA(GTE_VXY0, ((cos_yaw & GTE_HALF_MASK) << GTE_HALF_BITS) | (sin_yaw & GTE_HALF_MASK));
    product = (sin_pitch * sin_yaw) >> GTE_FRACTION_BITS;
    multiply_diagonal(0, sin_pitch & GTE_HALF_MASK, cos_pitch & GTE_HALF_MASK);
    GTE_READ_DATA(GTE_IR2, a2);
    GTE_READ_DATA(GTE_IR3, a3);
    d = (sin_roll * cos_pitch) >> GTE_FRACTION_BITS;
    multiply_diagonal(cos_roll & GTE_HALF_MASK, cos_roll & GTE_HALF_MASK, product & GTE_HALF_MASK);
    GTE_READ_DATA(GTE_IR1, b1);
    GTE_READ_DATA(GTE_IR2, b2);
    GTE_READ_DATA(GTE_IR3, b3);
    e = (sin_roll * product) >> GTE_FRACTION_BITS;
    multiply_diagonal(sin_roll & GTE_HALF_MASK, sin_roll & GTE_HALF_MASK, a2 & GTE_HALF_MASK);
    GTE_READ_DATA(GTE_IR1, c1);
    GTE_READ_DATA(GTE_IR2, c2);
    GTE_READ_DATA(GTE_IR3, c3);
    /* The sine's low half, unsigned, as the retail code multiplies it */
    f = (s32)((sin_roll & GTE_HALF_MASK) * a2) >> GTE_FRACTION_BITS;
    g = c2 + b3;
    b2 -= e;
    d = -d;
    b1 += f;
    c1 -= c3;

    /* Times the scale */
    GTE_WRITE_CONTROL(GTE_RT11RT12, o->scale[0] >> scale_shift);
    GTE_WRITE_CONTROL(GTE_RT13RT21, 0);
    GTE_WRITE_CONTROL(GTE_RT22RT23, o->scale[1] >> scale_shift);
    GTE_WRITE_CONTROL(GTE_RT31RT32, 0);
    GTE_WRITE_CONTROL(GTE_RT33, o->scale[2] >> scale_shift);
    GTE_WRITE_DATA(GTE_IR1, g);
    GTE_WRITE_DATA(GTE_IR2, a3);
    GTE_WRITE_DATA(GTE_IR3, c1);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, GTE_VECTOR_IR);
    GTE_READ_DATA(GTE_IR1, u1);
    GTE_READ_DATA(GTE_IR2, u2);
    GTE_READ_DATA(GTE_IR3, u3);
    GTE_WRITE_DATA(GTE_IR1, b2);
    GTE_WRITE_DATA(GTE_IR2, d);
    GTE_WRITE_DATA(GTE_IR3, b1);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, GTE_VECTOR_IR);
    GTE_READ_DATA(GTE_IR1, w1);
    GTE_READ_DATA(GTE_IR2, w2);
    GTE_READ_DATA(GTE_IR3, w3);
    u1 = -(((u1 << ROW_SCALE_SHIFT) + u1) >> ROW_SCALE_DIVISOR_SHIFT);
    u2 = -(((u2 << ROW_SCALE_SHIFT) + u2) >> ROW_SCALE_DIVISOR_SHIFT);
    u3 = -(((u3 << ROW_SCALE_SHIFT) + u3) >> ROW_SCALE_DIVISOR_SHIFT);
    GTE_WRITE_CONTROL(GTE_RT11RT12, (w2 << GTE_HALF_BITS) | (w1 & GTE_HALF_MASK));
    GTE_WRITE_CONTROL(GTE_RT13RT21, (u1 << GTE_HALF_BITS) | (w3 & GTE_HALF_MASK));
    GTE_WRITE_CONTROL(GTE_RT22RT23, (u3 << GTE_HALF_BITS) | (u2 & GTE_HALF_MASK));
    GTE_WRITE_CONTROL(GTE_RT31RT32, 0);
    GTE_WRITE_CONTROL(GTE_RT33, 0);
    return 1;
}

/* func_8003CD0C's words in the scratchpad: its arguments (the value for none, the callback, the output, the object), the
 * second sum, and 13 words it holds across each call (what its registers hold then, as its code saves them) */
#define CALLED_KEPT 0x1F8000E0
#define CALLED_SUM 0x1F8000F0
#define CALLED_SAVED 0x1F800070
/* The box kinds below this (2 * kind), with no subkind or one past 0x2F, go uncalled */
#define CALLED_KIND_LIMIT 4
#define CALLED_SUBKIND_MASK 0x3F0
#define CALLED_SUBKIND_LIMIT 0x300

typedef s32 (*BoxCallback)(void *object, s32 kind);

EQUIV_SCRATCH(func_8003CD0C, SAVE_AREA, SAVE_AREA_BYTES)
/* Its one caller passes func_8002A1B8, of two arguments, which writes neither $t8 nor $t9 in either build: the retail
 * code keeps two of a header's halvings in them across the call */
EQUIV_CALLBACK(func_8003CD0C, 7, 2, t8, t9)

/* Over the boxes of a collision list overlapping a box, those whose top is at most base: the average top of kind 0's
 * into out[1] and of the others' into out[2] (none when there are none). A box of kind 2 or more, or with a subkind under
 * 0x30, first goes to callback with the object and its kind word, and counts only when that returns 0 */
void func_8003CD0C(void *object, void *records, void *origin, s32 *box, s32 base, s32 *out, s32 none,
                   BoxCallback callback) {
    u32 *record = records;
    s32 *at = origin;
    u32 *kept = (u32 *)CALLED_KEPT;
    u32 *saved = (u32 *)CALLED_SAVED;
    u32 size[AXES];
    s32 halvings[AXES];
    s32 least[AXES], most[AXES];
    s32 count_a = 0, sum_a = 0, count_b = 0, sum_b = 0;
    s32 word, kind, level, axis, shift;
    u32 first, second;

    kept[0] = none;
    kept[1] = (u32)callback;
    kept[2] = (u32)out;
    kept[3] = (u32)object;
    for (axis = 0; axis < AXES; axis++) {
        size[axis] = 0;
        halvings[axis] = 0;
    }
    for (;; record += RECORD_WORDS) {
        first = record[0];
        second = record[1];
        if ((first & RECORD_HALF_MASK) == RECORD_END) {
            break;
        } else if ((first & RECORD_HALF_MASK) == RECORD_HEADER) {
            size[0] = (first >> RECORD_HALF_BITS) << RECORD_SIZE_SHIFT;
            size[1] = (second & RECORD_HALF_MASK) << RECORD_SIZE_SHIFT;
            size[2] = (second >> RECORD_HALF_BITS) << RECORD_SIZE_SHIFT;
            halvings[0] = record[2] >> RECORD_HALF_BITS;
            halvings[1] = record[3] & RECORD_HALF_MASK;
            halvings[2] = record[3] >> RECORD_HALF_BITS;
            /* With the loop's step, a header's 16 bytes */
            record += HEADER_WORDS - RECORD_WORDS;
            continue;
        } else {
            /* A box */
        }
        word = (((first & ~RECORD_LEVEL_MASK) & RECORD_HALF_MASK) >> RECORD_KIND_SHIFT) | 1;
        kind = word & RECORD_KIND_MASK;
        level = first & RECORD_LEVEL_MASK;
        least[0] = at[0] + ((s16)(first >> RECORD_HALF_BITS) << RECORD_OFFSET_SHIFT);
        least[1] = at[1] + ((s16)second << RECORD_OFFSET_SHIFT);
        least[2] = at[2] + ((s16)(second >> RECORD_HALF_BITS) << RECORD_OFFSET_SHIFT);
        if (box[3] < least[0] || box[4] < least[1] || box[5] < least[2]) {
            continue;
        }
        for (axis = 0; axis < AXES; axis++) {
            if (halvings[axis] < level) {
                shift = halvings[axis];
            } else {
                shift = level;
            }
            most[axis] = least[axis] + (size[axis] >> shift);
        }
        if (most[0] < box[0] || most[1] < box[1] || most[2] < box[2]) {
            continue;
        }
        if (kind >= CALLED_KIND_LIMIT
            || ((word & CALLED_SUBKIND_MASK) != 0 && (word & CALLED_SUBKIND_MASK) < CALLED_SUBKIND_LIMIT)) {
            ((s32 *)CALLED_SUM)[0] = sum_b;
            saved[0] = base;
            saved[1] = word;
            saved[2] = count_a;
            saved[3] = sum_a;
            saved[4] = kind;
            saved[5] = count_b;
            saved[6] = (u32)record;
            saved[7] = most[0];
            saved[8] = most[1];
            saved[9] = most[2];
            saved[10] = box[0];
            saved[11] = box[1];
            saved[12] = at[2];
            if (callback(object, word) != 0) {
                continue;
            }
        } else {
            /* Counted without asking */
        }
        if (base < most[1]) {
            continue;
        } else if (kind == 0) {
            count_a++;
            sum_a += most[1];
        } else {
            count_b++;
            sum_b += most[1];
        }
    }
    out[1] = none;
    if (count_a != 0) {
        out[1] = sum_a / count_a;
    }
    out[2] = none;
    if (count_b != 0) {
        out[2] = sum_b / count_b;
    }
}

/* A sort list's delta (SLST, between two of a zone's sort lists): halfwords, the first four its header (the end of its
 * swaps, unread, the end of its first stream, the start of its swaps), then from the fifth on its two streams and its
 * swaps, each end an index from the fifth. A stream's command holds a run's length less one (bits 12 to 15) and its
 * position (bits 0 to 11), then the run's entries; 0xFFFF, or a command in a stream's last halfword, ends it. A sort list
 * is its count of entries, a halfword unread, then its entries */
#define SLST_DATA 4
#define SLST_STREAM 8
#define SLST_END_FIRST 2
#define SLST_START_SWAPS 3
#define SLST_COUNT 0
#define SLST_NONE 0xFFFF
#define SLST_POSITION_MASK 0xFFF
#define SLST_RUN_SHIFT 12
/* A stream once ended: past any position */
#define SLST_PAST (SLST_NONE << 8)
/* A swap: entry index with the one length + 1 after it. 0x8000: length in bits 11 to 14, index in 0 to 10; 0x4000:
 * length 16 + bits 0 to 4, index the previous swap's + bits 5 to 13; 0x2000: bits 11 and 12 of entry bits 0 to 10
 * flipped; none of them: index then length in the next halfword, 12 bits each */
#define SWAP_SHORT 0x8000
#define SWAP_SHORT_LENGTH_MASK 0x7800
#define SWAP_SHORT_LENGTH_SHIFT 11
#define SWAP_INDEX_MASK 0x7FF
#define SWAP_NEAR 0x4000
#define SWAP_NEAR_LENGTH_MASK 0x1F
#define SWAP_NEAR_LENGTH_BASE 0x10
#define SWAP_NEAR_OFFSET_MASK 0x3FE0
#define SWAP_NEAR_OFFSET_SHIFT 5
#define SWAP_FLIP 0x2000
#define SWAP_FLIP_BITS 0x1800
#define SWAP_KINDS 0xE000
/* A copy shorter than this goes one entry at a time */
#define SLST_RUN_COPY 2
/* Gfx_AllocSortLists's two sort list buffers, and the entries one holds past its header ((0xBE4 - 4) / 2); the disc's
 * largest full sort list has 1026 */
#define SORT_LIST_BYTES 0xBE4
#define SORT_LIST_ENTRIES 0x5F0
/* An SLST item lies within its 64 KB NSF page (NSF_PAGE_SHIFT), its header's ends below ((0x10000 - 4) / 2); the
 * disc's largest delta is 0x638 bytes, its largest end 794 */
#define SLST_ITEM_BYTES 0x10000
#define SLST_ITEM_HALVES 0x7FFE

typedef struct {
    u16 *next;
    u16 *end;
    s32 left;
    s32 position;
    u16 entry;
} SlstStream;

/* Reads a stream's next command, and its run's first entry */
static void slst_command(SlstStream *stream) {
    u16 command = *stream->next++;

    stream->left = 0;
    if (!(stream->next < stream->end) || command == SLST_NONE) {
        stream->position = SLST_PAST;
    } else {
        stream->left = (command >> SLST_RUN_SHIFT) + 1;
        stream->position = command & SLST_POSITION_MASK;
    }
    if (stream->left > 0) {
        stream->entry = *stream->next++;
    } else {
        /* Ended: no entry */
    }
}

/* One entry of the run used: the next, or the next command */
static void slst_used(SlstStream *stream) {
    stream->left--;
    if (stream->left > 0) {
        stream->entry = *stream->next++;
    } else {
        slst_command(stream);
    }
}

static void slst_swap(u16 *entries, s32 index, s32 length) {
    s32 other = index + length + 1;
    u16 first = entries[index];
    u16 second = entries[other];

    entries[other] = first;
    entries[index] = second;
}

/* The two directions' merge: the entries of a list, with the runs of one stream removed and the other's inserted;
 * forward measures removals and insertions as func_8003AE54 does, else as func_8003B24C does */
static s32 slst_merge(u16 *delta, u16 *from, u16 *to, s32 forward) {
    SlstStream first, second;
    SlstStream *removed, *inserted;
    u16 *source = from + SLST_DATA / 2;
    u16 *target = to + SLST_DATA / 2;
    s32 count = from[SLST_COUNT];
    s32 index = 0, written = 0, gone = 0, added = 0;
    s32 run, other, room;

    first.next = delta + SLST_STREAM / 2;
    first.end = delta + SLST_DATA / 2 + delta[SLST_END_FIRST];
    second.next = first.end;
    second.end = delta + SLST_DATA / 2 + delta[SLST_START_SWAPS];
    if (forward) {
        removed = &first;
        inserted = &second;
    } else {
        inserted = &first;
        removed = &second;
    }
    slst_command(removed);
    slst_command(inserted);
    while (index < count || inserted->left > 0 || removed->left > 0) {
        if (forward) {
            run = removed->position - 1 - index;
            other = inserted->position + gone - index;
        } else {
            run = removed->position + gone - index;
            other = inserted->position + gone - added - 1 - index;
        }
        if (index < count && removed->left > 0 && run == 0) {
            gone++;
            index++;
            source++;
            slst_used(removed);
        } else if (inserted->left > 0 && other == 0) {
            *target++ = inserted->entry;
            written++;
            added++;
            slst_used(inserted);
        } else if (index < count) {
            if (other < run) {
                run = other;
            }
            room = count - index - 1;
            if (room < run) {
                run = room;
            }
            if (run < SLST_RUN_COPY) {
                run = 1;
            }
            written += run;
            index += run;
            for (; run > 0; run--) {
                *target++ = *source++;
            }
        } else {
            /* Entries to come from a stream only */
        }
    }
    *to = written;
    return written;
}

/* func_80029800 passes an SLST item and the zone's two sort lists, heap buffers (Cam_MoveAlongPath's D_8005F390 and
 * D_8005F398) */
EQUIV_SCRATCH(func_8003B24C, SAVE_AREA, SAVE_AREA_BYTES)
EQUIV_NONNULL(func_8003B24C, 0)
EQUIV_NONNULL(func_8003B24C, 1)
EQUIV_NONNULL(func_8003B24C, 2)
EQUIV_BUFFER(func_8003B24C, 0, SLST_ITEM_BYTES)
EQUIV_BUFFER(func_8003B24C, 1, SORT_LIST_BYTES)
EQUIV_BUFFER(func_8003B24C, 2, SORT_LIST_BYTES)
EQUIV_HALF(func_8003B24C, 1, 0, 0, SORT_LIST_ENTRIES)
/* The delta header's end of swaps, end of first stream and start of swaps, at bytes 0, 4 and 6 */
EQUIV_HALF(func_8003B24C, 0, 0, 0, SLST_ITEM_HALVES)
EQUIV_HALF(func_8003B24C, 0, 4, 0, SLST_ITEM_HALVES)
EQUIV_HALF(func_8003B24C, 0, 6, 0, SLST_ITEM_HALVES)
HANDWRITTEN_PARTS(func_8003B24C, func_8003B89C, func_8003B8BC, func_8003B8DC, func_8003B8FC, func_8003B91C)

/* A sort list from the next one and their delta, backward: the delta's swaps undone on from, last first, then its
 * first stream's runs inserted and its second's removed, into to */
void func_8003B24C(void *delta, u16 *from, u16 *to) {
    u16 *d = delta;
    u16 *list = from;
    u16 *entries = list + SLST_DATA / 2;
    u16 *swap = d + SLST_DATA / 2 + d[SLST_COUNT] - 1;
    u16 *stop = d + SLST_DATA / 2 + d[SLST_START_SWAPS];
    u16 command, operand;
    s32 index, length;

    while (!(swap < stop)) {
        command = *swap--;
        if (command == SLST_NONE) {
            continue;
        } else if (command & SWAP_SHORT) {
            length = (command & SWAP_SHORT_LENGTH_MASK) >> SWAP_SHORT_LENGTH_SHIFT;
            index = command & SWAP_INDEX_MASK;
        } else if (command & SWAP_NEAR) {
            /* The base comes from the swap before it, read without being taken */
            operand = swap[0];
            if (operand & SWAP_KINDS) {
                index = operand & SWAP_INDEX_MASK;
            } else {
                index = swap[-1] & SLST_POSITION_MASK;
            }
            length = (command & SWAP_NEAR_LENGTH_MASK) + SWAP_NEAR_LENGTH_BASE;
            index += (command & SWAP_NEAR_OFFSET_MASK) >> SWAP_NEAR_OFFSET_SHIFT;
        } else if (command & SWAP_FLIP) {
            entries[command & SWAP_INDEX_MASK] ^= command & SWAP_FLIP_BITS;
            continue;
        } else {
            length = command & SLST_POSITION_MASK;
            index = *swap-- & SLST_POSITION_MASK;
        }
        slst_swap(entries, index, length);
    }
    slst_merge(d, list, to, 0);
}

EQUIV_SCRATCH(func_8003AE54, SAVE_AREA, SAVE_AREA_BYTES)
EQUIV_NONNULL(func_8003AE54, 0)
EQUIV_NONNULL(func_8003AE54, 1)
EQUIV_NONNULL(func_8003AE54, 2)
EQUIV_BUFFER(func_8003AE54, 0, SLST_ITEM_BYTES)
EQUIV_BUFFER(func_8003AE54, 1, SORT_LIST_BYTES)
EQUIV_BUFFER(func_8003AE54, 2, SORT_LIST_BYTES)
EQUIV_HALF(func_8003AE54, 1, 0, 0, SORT_LIST_ENTRIES)
/* The delta header's end of swaps, end of first stream and start of swaps, at bytes 0, 4 and 6 */
EQUIV_HALF(func_8003AE54, 0, 0, 0, SLST_ITEM_HALVES)
EQUIV_HALF(func_8003AE54, 0, 4, 0, SLST_ITEM_HALVES)
EQUIV_HALF(func_8003AE54, 0, 6, 0, SLST_ITEM_HALVES)
EQUIV_RUNS(func_8003AE54, func_8003B24C, func_8003B89C, func_8003B8BC, func_8003B8DC, func_8003B8FC, func_8003B91C)

/* The next sort list from one and their delta, forward: its first stream's runs removed from from and its second's
 * inserted, into to, then the delta's swaps done on to, up to a 0xFFFF */
void func_8003AE54(void *delta, u16 *from, u16 *to) {
    u16 *d = delta;
    u16 *entries = to + SLST_DATA / 2;
    u16 *swap;
    u16 *stop;
    u16 command;
    s32 index = 0;
    s32 length;
    /* Whether the swap before is a short or long one, whose index a near swap starts from */
    s32 based = 0;

    slst_merge(d, from, to, 1);
    /* The swaps' bounds read after the merge, which may have written over the delta */
    swap = d + SLST_DATA / 2 + d[SLST_START_SWAPS];
    stop = d + SLST_DATA / 2 + d[SLST_COUNT];
    while (swap < stop) {
        command = *swap++;
        if (command == SLST_NONE) {
            break;
        } else if (command & SWAP_SHORT) {
            length = (command & SWAP_SHORT_LENGTH_MASK) >> SWAP_SHORT_LENGTH_SHIFT;
            index = command & SWAP_INDEX_MASK;
        } else if (command & SWAP_NEAR) {
            /* Every near swap on the disc follows a short or long one (docs/TARGET.md); the retail code's base is
             * otherwise a register's leftover: the caller's, the copy's jump target, or a flipped entry's address */
            EQUIV_ASSUME(based);
            length = (command & SWAP_NEAR_LENGTH_MASK) + SWAP_NEAR_LENGTH_BASE;
            index += (command & SWAP_NEAR_OFFSET_MASK) >> SWAP_NEAR_OFFSET_SHIFT;
        } else if (command & SWAP_FLIP) {
            entries[command & SWAP_INDEX_MASK] ^= command & SWAP_FLIP_BITS;
            based = 0;
            continue;
        } else {
            index = command & SLST_POSITION_MASK;
            length = *swap++ & SLST_POSITION_MASK;
        }
        based = 1;
        slst_swap(entries, index, length);
    }
}

/* func_80044F54's streaks: points in a box about the camera, wrapped, 0x2000 wide (x), 0x1000 high (y) and 0x2000 deep
 * (z), each a line from where it was from the view's last position to where it is from the current one. Its generator
 * starts from the same two words every call: the same points every frame */
#define STREAK_SEED_LOW 0x30125400
#define STREAK_SEED_HIGH 0x493583FE
#define RANDOM_BYTE_SHIFT 24
#define RANDOM_BYTE_BITS 8
/* A point's coordinates from the generator's bytes at the top of a word: x in 32nds, y in 16ths (a VXY pair's high
 * half), z in 64ths */
#define STREAK_X_SHIFT 19
#define STREAK_Y_SHIFT 4
#define STREAK_Z_SHIFT 18
/* A VXY pair (x low, y high) with y's lowest bit clear, where a carry out of x lands; the box's wrap of a pair and
 * of z */
#define PAIR_MASK 0xFFFEFFFF
#define STREAK_PAIR_WRAP 0x0FFF1FFF
#define STREAK_Z_WRAP 0x1FFF
/* What the box's corner is from the view, x and z; y's adds the scene's offset */
#define STREAK_X_BIAS 0x1000
#define STREAK_Y_BIAS 0x800
#define STREAK_Z_BIAS 0x1C00
/* The screen: a point past it has x from 512 or y from 217 (a packed SXY less this, the halves' signs clear) */
#define STREAK_SCREEN_END 0x00D90200
#define SXY_X_SIGN_SHIFT 16

/* A shaded line, semi-transparent (LineG2: tag, colour and code, xy, colour, xy), and the closing draw mode (tag, its
 * word, 0); a tag's word count is in its top byte, the next primitive's address below */
#define LINE_G2_CODE 0x52000000
#define LINE_G2_TAG_LENGTH 0x04000000
#define LINE_G2_WORDS 5
#define MODE_TAG_LENGTH 0x02000000
#define MODE_WORDS 3
#define PRIM_ADDRESS_MASK 0xFFFFFF

/* The words of the frame being built func_80044F54 uses: where the next primitive goes, and the ordering table */
typedef struct {
    u8 unk_00[8];
    u32 *next;
    u8 unk_0C[0xC];
    u32 ot[1];
} FrameBuffer;

/* D_80062A18's scene: the view matrix (its translation at 0x14, as a MATRIX), the streaks' VXY pair and z from the
 * view's last position and from the current one, both lines' colours (the first with its code), the closing mode's
 * word, the box's y offset and the streaks' slot in the ordering table */
typedef struct {
    u8 unk_00[0xC];
    s32 *view;
    u8 unk_10[0x44];
    u32 last_pair;
    s32 last_z;
    u32 pair;
    s32 z;
    u32 colour_last;
    u32 colour;
    u32 mode;
    s32 y_offset;
    s32 slot;
} StreakScene;

/* A MATRIX's translation: its words from the sixth, after nine halfwords and a pad */
#define VIEW_TRANSLATION 5

/* The generator's step: both words shift a byte, the low one's lowest into the high one's highest, and the sum of the
 * bytes shifted out (and one more of the low word's) the new byte at the top of the high word, returned there */
static u32 streak_random(u32 *low, u32 *high) {
    u32 carried = *high << RANDOM_BYTE_SHIFT;
    u32 byte;

    *high >>= RANDOM_BYTE_BITS;
    byte = *low + *high;
    *low = (*low >> RANDOM_BYTE_BITS) | carried;
    byte = (byte + (*low >> RANDOM_BYTE_BITS)) << RANDOM_BYTE_SHIFT;
    *high |= byte;
    return byte;
}

/* The most streaks a scene asks for: its camera's property 0x1B5, interpolated, is D_80062A90's only source
 * (func_80020A24), its fourth value 1 to 1,500 on the disc (docs/TARGET.md) */
#define STREAKS_MIN 1
#define STREAKS_MAX 1500

/* Its one caller (Sys_MainLoop) passes D_80062A90, the frame being built and D_80062A18 */
EQUIV_SCRATCH(func_80044F54, SAVE_AREA, SAVE_AREA_BYTES)
EQUIV_RANGE(func_80044F54, 0, STREAKS_MIN, STREAKS_MAX)
EQUIV_NONNULL(func_80044F54, 1)
EQUIV_NONNULL(func_80044F54, 2)

/* Draws COUNT + 1 streaks of the scene's box into the frame being built, all in one slot of its ordering table, then
 * the draw mode they close with; with the rotation the caller set and no translation */
void func_80044F54(s32 count, void *frame, void *scene) {
    FrameBuffer *buffer = frame;
    StreakScene *streaks = scene;
    u32 *line = buffer->next;
    u32 *slot;
    u32 low = STREAK_SEED_LOW;
    u32 high = STREAK_SEED_HIGH;
    u32 code, colour, mode, bias, view_pair, last_pair, pair, pair_span;
    s32 view_z, last_z, z, z_span, left;
    u32 x, y, depth, point, from, to, both, past, flag, head, address;
    s32 z_from, z_to;

    GTE_WRITE_CONTROL(GTE_TRX, 0);
    GTE_WRITE_CONTROL(GTE_TRY, 0);
    GTE_WRITE_CONTROL(GTE_TRZ, 0);
    GTE_WRITE_DATA(GTE_VXY2, 0);
    GTE_WRITE_DATA(GTE_VZ2, 0);
    last_pair = streaks->last_pair & PAIR_MASK;
    last_z = streaks->last_z;
    pair = streaks->pair & PAIR_MASK;
    z = streaks->z;
    code = streaks->colour_last | LINE_G2_CODE;
    colour = streaks->colour;
    mode = streaks->mode;
    bias = ((u32)(streaks->y_offset + STREAK_Y_BIAS) << GTE_HALF_BITS) | STREAK_X_BIAS;
    slot = &buffer->ot[streaks->slot];
    view_pair = (((u32)streaks->view[VIEW_TRANSLATION] & GTE_HALF_MASK) |
                 ((u32)streaks->view[VIEW_TRANSLATION + 1] << GTE_HALF_BITS)) & PAIR_MASK;
    view_z = streaks->view[VIEW_TRANSLATION + 2];
    last_pair = (last_pair - view_pair) & PAIR_MASK;
    last_z -= view_z;
    pair = (pair - view_pair) & PAIR_MASK;
    z -= view_z;
    pair_span = last_pair - pair;
    z_span = last_z - z;

    for (left = count; left >= 0; left--) {
        x = streak_random(&low, &high);
        y = streak_random(&low, &high);
        depth = streak_random(&low, &high);
        point = (x >> STREAK_X_SHIFT) | (y >> STREAK_Y_SHIFT);
        from = (point + last_pair) & STREAK_PAIR_WRAP;
        to = (point + pair) & STREAK_PAIR_WRAP;
        if (from - to != pair_span) {
            /* The box wraps x or y between the two ends */
            continue;
        } else {
            /* One side of the box */
        }
        GTE_WRITE_DATA(GTE_VXY0, (from - bias) & PAIR_MASK);
        GTE_WRITE_DATA(GTE_VXY1, (to - bias) & PAIR_MASK);
        z_from = ((depth >> STREAK_Z_SHIFT) + last_z) & STREAK_Z_WRAP;
        z_to = ((depth >> STREAK_Z_SHIFT) + z) & STREAK_Z_WRAP;
        if (z_from - z_to != z_span) {
            /* The box wraps z between the two ends */
            continue;
        } else {
            /* One side of the box */
        }
        GTE_WRITE_DATA(GTE_VZ0, z_from - STREAK_Z_BIAS);
        GTE_WRITE_DATA(GTE_VZ1, z_to - STREAK_Z_BIAS);
        address = (u32)line & PRIM_ADDRESS_MASK;
        Gte_Rtpt();
        head = *slot;
        line[1] = code;
        line[0] = head | LINE_G2_TAG_LENGTH;
        GTE_READ_CONTROL(GTE_FLAG, flag);
        GTE_READ_DATA(GTE_SXY0, from);
        GTE_READ_DATA(GTE_SXY1, to);
        both = from & to;
        past = ~((from - STREAK_SCREEN_END) | (to - STREAK_SCREEN_END)) | both;
        if (flag & GTE_FLAG_SZ3) {
            /* A depth out of range */
            continue;
        } else if ((s32)past < 0 || (s32)(past << SXY_X_SIGN_SHIFT) < 0) {
            /* Both ends past the same side of the screen */
            continue;
        } else {
            /* On the screen */
        }
        line[3] = colour;
        GTE_STORE_DATA(GTE_SXY0, line[2]);
        GTE_STORE_DATA(GTE_SXY1, line[4]);
        *slot = address;
        line += LINE_G2_WORDS;
    }

    line[1] = mode;
    line[2] = 0;
    head = *slot;
    line[0] = head | MODE_TAG_LENGTH;
    *slot = (u32)line & PRIM_ADDRESS_MASK;
    line += MODE_WORDS;
    buffer->next = line;
}

/* func_80045218's dots: the streaks' generator, its bytes as signed 16ths. A dot's x and z keep 0x70 from the middle
 * (x as a halfword, so only from above), its y is the scene's offset added and shifted */
#define DOT_SHIFT 20
#define DOT_GAP 0x70
/* A dot's grey: the generator's byte, at least 0x40, in each of red, green and blue */
#define DOT_GREY_MIN 0x40
#define DOT_GREY_MASK 0xFF
#define GREEN_SHIFT 8
#define BLUE_SHIFT 16
/* A 1x1 tile, opaque (tag, colour and code, xy), and the closing draw mode's word (a texture page) */
#define TILE_1_CODE 0x68000000
#define TILE_1_TAG_LENGTH 0x02000000
#define TILE_1_WORDS 3
#define DOTS_MODE 0xE1000A60

/* D_80062A18's scene as func_80045218 reads it: the dots' y offset, shift and slot in the ordering table */
typedef struct {
    u8 unk_00[0x7C];
    s32 y_offset;
    s32 y_shift;
    s32 slot;
} DotScene;

/* The most dots a scene asks for: its camera's property 0x2AA, interpolated, is D_80062AA0's only source
 * (func_80020A24), its first value 1,000 on the disc (docs/TARGET.md) */
#define DOTS_MIN 1
#define DOTS_MAX 1000

/* A dot's x or z from the generator, kept 0x70 from 0 */
static s32 dot_coordinate(s32 value) {
    if (value < 0) {
        if (value + DOT_GAP < 0) {
            return value;
        } else {
            return value - DOT_GAP;
        }
    } else if (value - DOT_GAP > 0) {
        return value;
    } else {
        return value + DOT_GAP;
    }
}

/* Its one caller (Sys_MainLoop) passes D_80062AA0, the frame being built and D_80062A18 */
EQUIV_SCRATCH(func_80045218, SAVE_AREA, SAVE_AREA_BYTES)
EQUIV_RANGE(func_80045218, 0, DOTS_MIN, DOTS_MAX)
EQUIV_NONNULL(func_80045218, 1)
EQUIV_NONNULL(func_80045218, 2)

/* Draws COUNT + 1 dots into the frame being built, all in one slot of its ordering table, then the draw mode they
 * close with; with the rotation the caller set and no translation */
void func_80045218(s32 count, void *frame, void *scene) {
    FrameBuffer *buffer = frame;
    DotScene *dots = scene;
    u32 *tile = buffer->next;
    u32 *slot;
    u32 low = STREAK_SEED_LOW;
    u32 high = STREAK_SEED_HIGH;
    s32 y_offset, y_shift, left, x, y, z;
    u32 grey, flag, at, past, head, address;

    GTE_WRITE_CONTROL(GTE_TRX, 0);
    GTE_WRITE_CONTROL(GTE_TRY, 0);
    GTE_WRITE_CONTROL(GTE_TRZ, 0);
    y_offset = dots->y_offset;
    y_shift = dots->y_shift;
    slot = &buffer->ot[dots->slot];

    for (left = count; left >= 0; left--) {
        x = (s32)streak_random(&low, &high) >> DOT_SHIFT;
        y = (s32)streak_random(&low, &high) >> DOT_SHIFT;
        z = (s32)streak_random(&low, &high) >> DOT_SHIFT;
        x = dot_coordinate(x & GTE_HALF_MASK);
        y = (y + y_offset) >> (y_shift & SHIFT_COUNT_MASK);
        z = dot_coordinate(z);
        address = (u32)tile & PRIM_ADDRESS_MASK;
        GTE_WRITE_DATA(GTE_VXY0, ((u32)y << GTE_HALF_BITS) | x);
        GTE_WRITE_DATA(GTE_VZ0, z);
        grey = streak_random(&low, &high);
        Gte_Rtps();
        grey = ((grey >> RANDOM_BYTE_SHIFT) | DOT_GREY_MIN) & DOT_GREY_MASK;
        grey |= (grey << GREEN_SHIFT) | (grey << BLUE_SHIFT);
        GTE_READ_CONTROL(GTE_FLAG, flag);
        GTE_READ_DATA(GTE_SXY2, at);
        past = ~((at - STREAK_SCREEN_END) | at);
        if (flag & GTE_FLAG_SZ3) {
            /* A depth out of range */
            continue;
        } else if ((s32)past < 0 || (s32)(past << SXY_X_SIGN_SHIFT) < 0) {
            /* Past the screen's bottom or right */
            continue;
        } else {
            /* On the screen, or past its top or left, which the GPU clips */
        }
        head = *slot;
        tile[1] = grey | TILE_1_CODE;
        tile[0] = head | TILE_1_TAG_LENGTH;
        tile[2] = at;
        *slot = address;
        tile += TILE_1_WORDS;
    }

    tile[1] = DOTS_MODE;
    tile[2] = 0;
    tile[0] = *slot | MODE_TAG_LENGTH;
    *slot = (u32)tile & PRIM_ADDRESS_MASK;
    tile += MODE_WORDS;
    buffer->next = tile;
}

/* The world renderer: func_80041E5C walks the zone's sort list and draws each polygon with the scene's mode
 * (docs/TARGET.md, "The world renderer") */

/* The light's walk over its grid (func_800461E0), as the retail code keeps it: each value's start of the row, current
 * value, step along a row (toward the polygon's second corner) and down the rows (toward its third). x, y and z are in
 * 8ths; red and green, the distances from Crash along z and x, and the texture's u and v go in pairs (the low half
 * first), bit 15 cleared after each step so that the low half's carry stays out of the high one */
typedef struct {
    s32 x_row;
    s32 x;
    s16 x_along;
    s16 x_down;
    s32 y_row;
    s32 y;
    s16 y_along;
    s16 y_down;
    s32 z_row;
    s32 z;
    s16 z_along;
    s16 z_down;
    u32 rg_row;
    u32 rg;
    u32 rg_along;
    u32 rg_down;
    s16 b_row;
    s16 b;
    s16 b_along;
    s16 b_down;
    u32 near_row;
    u32 near;
    u32 near_along;
    u32 near_down;
    u32 uv_row;
    u32 uv;
    u32 uv_along;
    u32 uv_down;
} LightWalk;

/* The scratchpad as the world renderer keeps it. The mode's entries are copied there, where its setup may change them;
 * its other words are the retail code's state, which the C keeps where it did */
/* The worlds Sys_MainLoop copies into the scratchpad, at most */
#define WORLDS 8
/* A quad's record: two words */
#define QUAD_RECORD_WORDS 2

typedef struct {
    u8 unk_00[8];
    s32 depth_fog_shift;
    u8 unk_0C[4];
    s32 depth_fog_start;
    u8 unk_14[8];
    s32 clock;
    u8 unk_20[SAVE_AREA_BYTES - 0x20];
    u8 unk_40[0x20];
    s32 sway_scale;
    WorldStep draw_triangle;
    WorldStep draw_quad;
    u32 drained;
    u8 unk_70[8];
    /* The next entry, its kind alone in the lit modes */
    u32 upcoming;
    u32 lamp1[2];
    s32 pulse;
    u32 lamp2[2];
    u32 shade;
    s32 light_x;
    s32 light_z;
    u16 *first;
    u8 unk_A0[8];
    WorldStep project_triangle;
    WorldStep project_quad;
    u32 lit_words[4];
    s32 camera_x;
    s32 camera_y;
    /* The lit modes keep a corner's colour there in turn */
    s32 camera_z;
    u32 wave_phase;
    u8 depth_shifts[0x20];
    s32 sway_shift;
    u32 light_texture[3];
    u8 waves[0x10];
    LightWalk walk;
    u8 unk_16C[4];
    u8 *light_heights;
    u8 unk_174[4];
    u32 quad_first_xy;
    u32 quad_first_z;
    u8 unk_180[4];
    u32 rotated;
    u32 lit_depth;
    u8 unk_18C[4];
    u32 light_corners[3][2];
    /* The swaying mode's replacement colour in the first; the light's polygon's corners' colours */
    u32 mode_colours[3];
    u32 quad_first_vxy;
    u32 quad_first_vz;
    u32 light_vectors[2][2];
    u8 *light_shades;
    u32 light_depth;
    u8 *light_grid;
    u32 vertex_mask;
    u32 pair_mask;
    u32 *light_slot;
    u32 light_saved_texture[3];
    u32 lamp_far[AXES];
    u32 tint_far[AXES];
    u32 lamp_full[AXES];
    s32 tint;
    u32 light_material;
    u32 screen_end;
    u32 light_last_xy;
    u32 light_last_z;
    u32 light_first_colour;
    u8 unk_22C[0x54];
    WorldBlock worlds[WORLDS];
} WorldScratch;

/* The fields' offsets the retail code addresses, checked as the build compiles (an array of negative size otherwise) */
#define SCRATCH_AT(field, offset) typedef char check_##field[((u32)&((WorldScratch *)0)->field == (offset)) ? 1 : -1]
SCRATCH_AT(clock, 0x1c);
SCRATCH_AT(draw_triangle, 0x64);
SCRATCH_AT(upcoming, 0x78);
SCRATCH_AT(light_x, 0x94);
SCRATCH_AT(first, 0x9c);
SCRATCH_AT(project_triangle, 0xa8);
SCRATCH_AT(camera_z, 0xc8);
SCRATCH_AT(depth_shifts, 0xd0);
SCRATCH_AT(light_texture, 0xf4);
SCRATCH_AT(walk, 0x110);
SCRATCH_AT(light_heights, 0x170);
SCRATCH_AT(quad_first_xy, 0x178);
SCRATCH_AT(rotated, 0x184);
SCRATCH_AT(light_corners, 0x190);
SCRATCH_AT(mode_colours, 0x1a8);
SCRATCH_AT(quad_first_vxy, 0x1b4);
SCRATCH_AT(light_shades, 0x1cc);
SCRATCH_AT(vertex_mask, 0x1d8);
SCRATCH_AT(light_slot, 0x1e0);
SCRATCH_AT(lamp_far, 0x1f0);
SCRATCH_AT(tint, 0x214);
SCRATCH_AT(screen_end, 0x21c);
SCRATCH_AT(light_first_colour, 0x228);
SCRATCH_AT(worlds, 0x280);

#define WORLD_SCRATCH ((WorldScratch *)SAVE_AREA)

/* A sort list's entry: its world (8 a frame), the polygon's kind and its index there */
#define ENTRY_WORLD_SHIFT 13
#define ENTRY_KIND_MASK 0x1800
#define ENTRY_INDEX_MASK 0x7FF
/* Triangles in the world's triangle table, and three kinds in its quad table: two triangles of a quad's corners (the
 * second with its texture's corners turned) and the quad */
#define KIND_TRIANGLE 0
#define KIND_QUAD_FIRST 0x800
#define KIND_QUAD_LAST 0x1000
#define KIND_QUAD 0x1800
/* The sort list's entries, after its count and a halfword */
#define SORT_LIST_FIRST 2
/* A polygon's vertices: 12-bit indices in its words, from the top */
#define VERTEX_HIGH_SHIFT 20
#define VERTEX_LOW_SHIFT 8
#define VERTEX_HALF_SHIFT 4
#define VERTEX_INDEX_MASK 0xFFF
/* A polygon's material: 0 none, else its texture's index (bit 11 an animation's) from its first word's low byte and
 * its second's low 4 bits */
#define MATERIAL_SHIFT 4
#define MATERIAL_LOW_MASK 0xF
#define MATERIAL_MASK 0xFFF
#define MATERIAL_ANIMATED 0x800
#define MATERIAL_INDEX_MASK 0x7FF
/* A vertex's position and colour bits: its word's x and y less their low 4 bits, its halfword's z less its low 4; the
 * colour's index from the halfword's low 4 bits, the word's low 4 and bits 16 and 17 */
#define VERTEX_MASK 0xFFF0FFF0
#define VERTEX_Z_MASK 0xFFF0
#define COLOUR_HALF_MASK 0xF
#define COLOUR_HALF_SHIFT 2
#define COLOUR_HIGH_SHIFT 6
#define COLOUR_HIGH_MASK 0xC00
#define COLOUR_LOW_SHIFT 6
#define COLOUR_LOW_MASK 0x3C0
/* The ordering table's slots (2,048) and the deepest a polygon goes; its slot from the sum of its last three depths */
#define OT_SLOTS 0x800
#define OT_LAST (OT_SLOTS - 1)
#define OT_END 0xFFFFFF
#define DEPTH_DEEPEST 0x770
#define DEPTH_SUM_SHIFT 5
#define DEPTH_SHIFT_STEP 6
/* A texture: three words (four for a quad, its last halfword the fourth corner), indices from 1; an animation's word:
 * its clock's shift (bits 26 to 31), its phase (from bit 19), its frames less one (bits 12 to 18), and either a
 * texture's index or, bit 11 set, another animation's; a depth level's (bit 31 set): its index and 2 bits a level */
#define TEXTURE_WORDS 3
#define ANIMATION_LEVELS 0x80000000
#define ANIMATION_CLOCK_SHIFT 26
#define ANIMATION_PHASE_SHIFT 19
#define ANIMATION_FRAMES_SHIFT 12
#define ANIMATION_FRAMES_MASK 0x7F
#define LEVEL_MASK 3
/* A texture's second word: its page's blending (bits 21 and 22, 3 none) */
#define TEXTURE_BLEND_MASK 0x600000
#define TEXTURE_OPAQUE TEXTURE_BLEND_MASK
#define HALF_LOW_MASK 0xFFFF
#define HALF_HIGH_MASK 0xFFFF0000
/* The primitives: a shaded triangle (G3) or quad (G4), textured (GT3, GT4) opaque or not; their words, after the tag */
#define POLY_G3_CODE 0x30000000
#define POLY_GT3_CODE 0x34000000
#define POLY_GT3_BLENDED_CODE 0x36000000
#define POLY_G4_CODE 0x38000000
#define POLY_GT4_CODE 0x3C000000
#define POLY_GT4_BLENDED_CODE 0x3E000000
#define POLY_G3_WORDS 7
#define POLY_GT3_WORDS 10
#define POLY_G4_WORDS 9
#define POLY_GT4_WORDS 13
#define TAG_LENGTH_SHIFT 24
/* A triangle's corners and a quad's */
#define TRIANGLE_CORNERS 3
#define QUAD_CORNERS 4
/* Two bits of a vertex's word, which each mode reads its own way */
#define VERTEX_BIT_18 0x40000
#define VERTEX_BIT_19 0x80000
/* The kind of triangle whose texture is turned: the flag the retail code sets, a register's constant */
#define ROTATED_FLAG VERTEX_MASK
/* The camera's position in whole units, from 24.8 */
#define CAMERA_SHIFT 8

extern u8 D_8005C724[];

/* A vertex's colour, from the world's colour table */
static u32 vertex_colour(WorldPass *pass, s32 vertex) {
    u32 word = pass->word[vertex];
    u32 offset = ((pass->half[vertex] & COLOUR_HALF_MASK) << COLOUR_HALF_SHIFT) |
                 ((word >> COLOUR_HIGH_SHIFT) & COLOUR_HIGH_MASK) | ((word << COLOUR_LOW_SHIFT) & COLOUR_LOW_MASK);

    return *(u32 *)(pass->world->colours + offset);
}

/* Off the screen: every corner past the same side of it (both halves of a packed SXY at least the screen's end, or
 * negative) */
static s32 off_screen(u32 *points, s32 count) {
    u32 end = WORLD_SCRATCH->screen_end;
    u32 past = 0;
    u32 all = ~0;
    s32 point;

    for (point = 0; point < count; point++) {
        past |= points[point] - end;
        all &= points[point];
    }
    past = ~past | all;
    return (s32)past < 0 || (s32)(past << GTE_HALF_BITS) < 0;
}

/* The polygon's slot by its depth (SZ1 + SZ2 + SZ3, / 32): nearer, higher */
static void depth_slot(WorldPass *pass) {
    u32 sz1, sz2, sz3;
    s32 slot;

    GTE_READ_DATA(GTE_SZ1, sz1);
    GTE_READ_DATA(GTE_SZ2, sz2);
    GTE_READ_DATA(GTE_SZ3, sz3);
    pass->depth = (sz1 + sz2 + sz3) >> DEPTH_SUM_SHIFT;
    slot = DEPTH_DEEPEST - (s32)pass->depth;
    if (slot < 0) {
        slot = 0;
    } else {
        /* In the table */
    }
    pass->slot = &pass->ot[slot];
}

/* The slots below lowest down to past (left out), each linked to the one above */
static void link_down(u32 *lowest, u32 *past) {
    u32 head = (u32)lowest & PRIM_ADDRESS_MASK;

    for (lowest--; lowest != past; lowest--) {
        *lowest = head;
        head -= sizeof(u32);
    }
}

/* Links the primitive at pass->next into the ordering table and moves past it. The polygons come sorted, far first: one
 * deeper than the lowest slot linked so far goes into that one, else the slots down to its own are linked first, each
 * to the one above */
static void link_primitive(WorldPass *pass, s32 words) {
    u32 *primitive = pass->next;
    u32 *lowest = pass->lowest;
    u32 *slot = pass->slot;
    u32 head = pass->head;

    if (slot < lowest) {
        head = (u32)lowest & PRIM_ADDRESS_MASK;
        for (lowest--; lowest != slot; lowest--) {
            *lowest = head;
            head -= sizeof(u32);
        }
    } else {
        /* Into the lowest slot */
    }
    primitive[0] = head | ((u32)(words - 1) << TAG_LENGTH_SHIFT);
    head = (u32)primitive & PRIM_ADDRESS_MASK;
    *lowest = head;
    pass->lowest = lowest;
    pass->head = head;
    pass->next = primitive + words;
}

/* The texture a material shows now: a fixed one, an animation's frame by the clock, or a depth level's by the polygon's
 * depth (the shift for 2 bits from a byte table the renderer copies to the scratchpad, read past its 32 bytes for the
 * farthest polygons: docs/TARGET.md, "Code addresses read as data") */
static u32 *texture(WorldPass *pass) {
    u32 index = pass->material & MATERIAL_INDEX_MASK;
    u32 word, frame, shift;

    if (pass->material & MATERIAL_ANIMATED) {
        word = pass->world->animations[index];
        shift = *((u8 *)WORLD_SCRATCH->depth_shifts + (pass->depth >> DEPTH_SHIFT_STEP));
        if (!(word & ANIMATION_LEVELS)) {
            frame = (((u32)pass->clock >> ((word >> ANIMATION_CLOCK_SHIFT) & SHIFT_COUNT_MASK)) +
                     (word >> ANIMATION_PHASE_SHIFT)) & ((word >> ANIMATION_FRAMES_SHIFT) & ANIMATION_FRAMES_MASK);
            index = (word + frame) & MATERIAL_INDEX_MASK;
            if (word & MATERIAL_ANIMATED) {
                word = pass->world->animations[index];
            } else {
                return pass->world->textures + (index - 1) * TEXTURE_WORDS;
            }
        } else {
            /* A depth level's */
        }
        index = (word & MATERIAL_INDEX_MASK) + ((word >> (shift & SHIFT_COUNT_MASK)) & LEVEL_MASK);
    } else {
        /* A fixed texture */
    }
    return pass->world->textures + (index - 1) * TEXTURE_WORDS;
}

/* A triangle's texture's three words, its corners turned for the second triangle of a quad's (WorldScratch.rotated) */
static void triangle_texture(WorldPass *pass, u32 *words) {
    u32 rotated = WORLD_SCRATCH->rotated;
    u32 *corners = texture(pass);
    u32 first = corners[0];
    u32 second = corners[1];
    u32 third = corners[2];

    if (rotated) {
        first = (first & HALF_HIGH_MASK) | (second & HALF_LOW_MASK);
        second = (second & HALF_HIGH_MASK) | (third & HALF_LOW_MASK);
        third >>= GTE_HALF_BITS;
    } else {
        /* The texture's corners as they are */
    }
    words[0] = first;
    words[1] = second;
    words[2] = third;
}

/* The default mode's triangle drawer: a shaded triangle, textured unless its material is 0 */
void func_80042548(WorldPass *pass) {
    u32 *primitive = pass->next;
    u32 words[3];
    u32 first, second, third;

    if (!(pass->material & MATERIAL_MASK)) {
        GTE_STORE_DATA(GTE_SXY0, primitive[2]);
        GTE_STORE_DATA(GTE_SXY1, primitive[4]);
        GTE_STORE_DATA(GTE_SXY2, primitive[6]);
        primitive[1] = pass->colour[0] | POLY_G3_CODE;
        primitive[3] = pass->colour[1];
        primitive[5] = pass->colour[2];
        link_primitive(pass, POLY_G3_WORDS);
        return;
    } else {
        triangle_texture(pass, words);
    }
    first = words[0];
    second = words[1];
    third = words[2];
    primitive[3] = first;
    primitive[6] = second;
    primitive[9] = third;
    primitive[4] = pass->colour[1];
    primitive[7] = pass->colour[2];
    if ((second & TEXTURE_BLEND_MASK) == TEXTURE_OPAQUE) {
        primitive[1] = pass->colour[0] | POLY_GT3_CODE;
    } else {
        primitive[1] = pass->colour[0] | POLY_GT3_BLENDED_CODE;
    }
    GTE_STORE_DATA(GTE_SXY0, primitive[2]);
    GTE_STORE_DATA(GTE_SXY1, primitive[5]);
    GTE_STORE_DATA(GTE_SXY2, primitive[8]);
    link_primitive(pass, POLY_GT3_WORDS);
}

/* The default mode's quad drawer: a shaded quad, textured unless its material is 0; its first corner's position the
 * projector wrote */
void func_80042818(WorldPass *pass) {
    u32 *primitive = pass->next;
    u32 *corners;
    u32 first, second, third;

    if (!(pass->material & MATERIAL_MASK)) {
        GTE_STORE_DATA(GTE_SXY0, primitive[4]);
        GTE_STORE_DATA(GTE_SXY1, primitive[6]);
        GTE_STORE_DATA(GTE_SXY2, primitive[8]);
        primitive[1] = pass->colour[0] | POLY_G4_CODE;
        primitive[3] = pass->colour[1];
        primitive[5] = pass->colour[2];
        primitive[7] = pass->colour[3];
        link_primitive(pass, POLY_G4_WORDS);
        return;
    } else {
        corners = texture(pass);
    }
    /* Read before the primitive is written, as the retail code does */
    first = corners[0];
    second = corners[1];
    third = corners[2];
    primitive[3] = first;
    primitive[6] = second;
    primitive[9] = third;
    primitive[12] = third >> GTE_HALF_BITS;
    primitive[4] = pass->colour[1];
    primitive[7] = pass->colour[2];
    primitive[10] = pass->colour[3];
    if ((second & TEXTURE_BLEND_MASK) == TEXTURE_OPAQUE) {
        primitive[1] = pass->colour[0] | POLY_GT4_CODE;
    } else {
        primitive[1] = pass->colour[0] | POLY_GT4_BLENDED_CODE;
    }
    GTE_STORE_DATA(GTE_SXY0, primitive[5]);
    GTE_STORE_DATA(GTE_SXY1, primitive[8]);
    GTE_STORE_DATA(GTE_SXY2, primitive[11]);
    link_primitive(pass, POLY_GT4_WORDS);
}

/* Reads an entry's polygon from its world's tables */
static void read_polygon(WorldRecord *record, u32 entry) {
    WorldBlock *world = &WORLD_SCRATCH->worlds[entry >> ENTRY_WORLD_SHIFT];
    u32 index = entry & ENTRY_INDEX_MASK;

    record->world = world;
    record->kind = entry & ENTRY_KIND_MASK;
    if (record->kind == KIND_TRIANGLE) {
        record->second = *(u16 *)(world->triangles + index * sizeof(u16));
        record->first = *(u32 *)(world->triangles - (index + 1) * sizeof(u32));
    } else {
        record->first = world->quads[index * QUAD_RECORD_WORDS];
        record->second = world->quads[index * QUAD_RECORD_WORDS + 1];
    }
}

/* The default mode's triangle projector: its three corners, culled off the screen, then drawn; the next polygon read
 * after its first corner's colour (WorldPass) */
void func_80042420(WorldPass *pass) {
    u32 points[3];

    Gte_Rtpt();
    pass->colour[0] = vertex_colour(pass, 0);
    read_polygon(pass->upcoming, pass->upcoming_entry);
    GTE_READ_DATA(GTE_SXY0, points[0]);
    GTE_READ_DATA(GTE_SXY1, points[1]);
    GTE_READ_DATA(GTE_SXY2, points[2]);
    if (off_screen(points, TRIANGLE_CORNERS)) {
        return;
    } else {
        /* Drawn */
    }
    pass->colour[1] = vertex_colour(pass, 1);
    pass->colour[2] = vertex_colour(pass, 2);
    depth_slot(pass);
    WORLD_SCRATCH->draw_triangle(pass);
}

/* The default mode's quad projector: its first three corners, then the fourth (RTPS pushing the first out), culled off
 * the screen, then drawn; the first corner's position and depth are kept in the scratchpad, the position also written
 * into the primitive, drawn or not */
void func_800426A0(WorldPass *pass) {
    u32 points[4];

    Gte_Rtpt();
    pass->colour[0] = vertex_colour(pass, 0);
    read_polygon(pass->upcoming, pass->upcoming_entry);
    GTE_STORE_DATA(GTE_SXY0, WORLD_SCRATCH->quad_first_xy);
    GTE_STORE_DATA(GTE_SZ1, WORLD_SCRATCH->quad_first_z);
    GTE_WRITE_DATA(GTE_VXY0, pass->quad_last_xy);
    GTE_WRITE_DATA(GTE_VZ0, pass->quad_last_z);
    GTE_READ_DATA(GTE_SXY0, points[0]);
    GTE_READ_DATA(GTE_SXY1, points[1]);
    GTE_READ_DATA(GTE_SXY2, points[2]);
    pass->next[2] = points[0];
    Gte_Rtps();
    pass->colour[1] = vertex_colour(pass, 1);
    GTE_READ_DATA(GTE_SXY2, points[3]);
    if (off_screen(points, QUAD_CORNERS)) {
        return;
    } else {
        /* Drawn */
    }
    pass->colour[2] = vertex_colour(pass, 2);
    pass->colour[3] = vertex_colour(pass, 3);
    depth_slot(pass);
    WORLD_SCRATCH->draw_quad(pass);
}

/* A vertex of the polygon: its words read, its VXY pair returned and its z kept */
static u32 load_vertex(WorldPass *pass, s32 vertex, u32 index) {
    WorldBlock *world = pass->world;
    u32 word = *(u32 *)(world->vertices - (index + 1) * sizeof(u32));
    u32 half = *(u16 *)(world->vertices + index * sizeof(u16));

    pass->word[vertex] = word;
    pass->half[vertex] = half;
    pass->vertex_z[vertex] = (half & VERTEX_Z_MASK) + world->offset_z;
    return ((word & VERTEX_MASK) + world->offset_xy) & PAIR_MASK;
}

/* Draws a polygon: its corners loaded into the GTE's vectors (a quad's fourth kept for its projector), then the scene's
 * projector run, which reads the next entry's polygon */
static void world_polygon(WorldPass *pass, WorldRecord *polygon, WorldRecord *next, u32 next_entry) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 first = polygon->first;
    u32 second = polygon->second;
    u32 corners[4];
    u32 xy;
    WorldStep project;

    pass->world = polygon->world;
    pass->material = (first << MATERIAL_SHIFT) | (second & MATERIAL_LOW_MASK);
    if (polygon->kind == KIND_TRIANGLE) {
        corners[0] = first >> VERTEX_HIGH_SHIFT;
        corners[1] = (first >> VERTEX_LOW_SHIFT) & VERTEX_INDEX_MASK;
        corners[2] = (second >> VERTEX_HALF_SHIFT) & VERTEX_INDEX_MASK;
    } else if (polygon->kind == KIND_QUAD_LAST) {
        corners[0] = (first >> VERTEX_LOW_SHIFT) & VERTEX_INDEX_MASK;
        corners[1] = second >> VERTEX_HIGH_SHIFT;
        corners[2] = (second >> VERTEX_LOW_SHIFT) & VERTEX_INDEX_MASK;
    } else {
        corners[0] = first >> VERTEX_HIGH_SHIFT;
        corners[1] = (first >> VERTEX_LOW_SHIFT) & VERTEX_INDEX_MASK;
        corners[2] = second >> VERTEX_HIGH_SHIFT;
        corners[3] = (second >> VERTEX_LOW_SHIFT) & VERTEX_INDEX_MASK;
    }
    xy = load_vertex(pass, 0, corners[0]);
    GTE_WRITE_DATA(GTE_VXY0, xy);
    GTE_WRITE_DATA(GTE_VZ0, pass->vertex_z[0]);
    xy = load_vertex(pass, 1, corners[1]);
    GTE_WRITE_DATA(GTE_VXY1, xy);
    GTE_WRITE_DATA(GTE_VZ1, pass->vertex_z[1]);
    xy = load_vertex(pass, 2, corners[2]);
    GTE_WRITE_DATA(GTE_VXY2, xy);
    GTE_WRITE_DATA(GTE_VZ2, pass->vertex_z[2]);
    if (polygon->kind == KIND_QUAD) {
        pass->quad_last_xy = load_vertex(pass, 3, corners[3]);
        pass->quad_last_z = pass->vertex_z[3];
        project = scratch->project_quad;
    } else if (polygon->kind == KIND_QUAD_LAST) {
        scratch->rotated = ROTATED_FLAG;
        project = scratch->project_triangle;
    } else {
        scratch->rotated = 0;
        project = scratch->project_triangle;
    }
    pass->upcoming = next;
    pass->upcoming_entry = next_entry;
    project(pass);
}

/* The fog's far colours: bright where a vertex's word has bit 18 set, else dark */
#define FOG_BRIGHT_VERTEX VERTEX_BIT_18
#define FOG_BRIGHT 0xFF0
#define FOG_DARK 0x200

/* The corners' colours moved toward the fog's far colour by its depth, with DPCS */
static void fog_colours(WorldPass *pass, s32 corners) {
    u32 far;
    s32 corner;

    GTE_LOAD_DATA(GTE_IR0, WORLD_SCRATCH->shade);
    for (corner = 0; corner < corners; corner++) {
        GTE_WRITE_DATA(GTE_RGBC, pass->colour[corner]);
        if (pass->word[corner] & FOG_BRIGHT_VERTEX) {
            far = FOG_BRIGHT;
        } else {
            far = FOG_DARK;
        }
        GTE_WRITE_CONTROL(GTE_RFC, far);
        GTE_WRITE_CONTROL(GTE_GFC, far);
        GTE_WRITE_CONTROL(GTE_BFC, far);
        Gte_Dpcs();
        GTE_READ_DATA(GTE_RGB2, pass->colour[corner]);
    }
}

/* The fogged mode's setup (the scene's flag 0x200000 with D_8006CDC4 set): the fog's depth into the scratchpad */
void func_8004599C(WorldScene *scene) {
    WORLD_SCRATCH->shade = scene->shade;
}

/* The fogged mode's drawers: the default ones, the corners' colours fogged first */
void func_800459A8(WorldPass *pass) {
    fog_colours(pass, TRIANGLE_CORNERS);
    func_80042548(pass);
}

void func_80045AB4(WorldPass *pass) {
    fog_colours(pass, QUAD_CORNERS);
    func_80042818(pass);
}

/* The depth fog's colour: a component in each of bits 4 to 11, 12 to 19 and 20 to 27 */
#define DEPTH_FOG_GREEN_SHIFT 8
#define DEPTH_FOG_RED_SHIFT 16
#define DEPTH_FOG_COMPONENT_MASK 0xFF0
/* A flat triangle (F3) or quad (F4): its code, black, and its words */
#define POLY_F3_CODE 0x20000000
#define POLY_F4_CODE 0x28000000
#define POLY_F3_WORDS 5
#define POLY_F4_WORDS 6

/* The depth fog's mode (the camera's flags 0x20, or 0x200000 without D_8006CDC4): the far colour into the GTE, its
 * shift and starting depth into the scratchpad */
void func_8004399C(WorldScene *scene) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 colour = scene->depth_fog_colour;

    GTE_WRITE_CONTROL(GTE_RFC, (colour >> DEPTH_FOG_RED_SHIFT) & DEPTH_FOG_COMPONENT_MASK);
    GTE_WRITE_CONTROL(GTE_GFC, (colour >> DEPTH_FOG_GREEN_SHIFT) & DEPTH_FOG_COMPONENT_MASK);
    GTE_WRITE_CONTROL(GTE_BFC, colour & DEPTH_FOG_COMPONENT_MASK);
    scratch->depth_fog_shift = scene->depth_fog_shift;
    scratch->depth_fog_start = scene->depth_fog_start;
}

/* A black flat polygon instead of the shaded one: its three last corners from the GTE (a quad's first the projector
 * wrote) */
static void flat_black(WorldPass *pass, s32 corners) {
    if (corners == QUAD_CORNERS) {
        GTE_STORE_DATA(GTE_SXY0, pass->next[3]);
        GTE_STORE_DATA(GTE_SXY1, pass->next[4]);
        GTE_STORE_DATA(GTE_SXY2, pass->next[5]);
        pass->next[1] = POLY_F4_CODE;
        link_primitive(pass, POLY_F4_WORDS);
    } else {
        GTE_STORE_DATA(GTE_SXY0, pass->next[2]);
        GTE_STORE_DATA(GTE_SXY1, pass->next[3]);
        GTE_STORE_DATA(GTE_SXY2, pass->next[4]);
        pass->next[1] = POLY_F3_CODE;
        link_primitive(pass, POLY_F3_WORDS);
    }
}

/* A corner's colour fogged by its depth past the fog's start; whether it was */
static s32 depth_fog(WorldPass *pass, s32 corner, u32 depth) {
    WorldScratch *scratch = WORLD_SCRATCH;
    s32 past = depth - scratch->depth_fog_start;

    if (past < 0) {
        return 0;
    } else {
        GTE_WRITE_DATA(GTE_RGBC, pass->colour[corner]);
        GTE_WRITE_DATA(GTE_IR0, past << (scratch->depth_fog_shift & SHIFT_COUNT_MASK));
        Gte_Dpcs();
        GTE_READ_DATA(GTE_RGB2, pass->colour[corner]);
        return 1;
    }
}

/* The depth fog's drawers: the corners fogged by their depths (the quad's first kept by its projector), then the
 * default drawer, or a black flat polygon where the last corner was fogged and every colour came out 0 */
void func_800439D4(WorldPass *pass) {
    u32 sz1, sz2, sz3;
    s32 fogged;

    GTE_READ_DATA(GTE_SZ1, sz1);
    GTE_READ_DATA(GTE_SZ2, sz2);
    GTE_READ_DATA(GTE_SZ3, sz3);
    depth_fog(pass, 0, sz1);
    depth_fog(pass, 1, sz2);
    fogged = depth_fog(pass, 2, sz3);
    if (fogged && (pass->colour[0] | pass->colour[1] | pass->colour[2]) == 0) {
        flat_black(pass, TRIANGLE_CORNERS);
    } else {
        func_80042548(pass);
    }
}

void func_80043A84(WorldPass *pass) {
    u32 sz1, sz2, sz3;
    s32 fogged;

    GTE_READ_DATA(GTE_SZ1, sz1);
    GTE_READ_DATA(GTE_SZ2, sz2);
    GTE_READ_DATA(GTE_SZ3, sz3);
    depth_fog(pass, 0, WORLD_SCRATCH->quad_first_z);
    depth_fog(pass, 1, sz1);
    depth_fog(pass, 2, sz2);
    fogged = depth_fog(pass, 3, sz3);
    if (fogged && (pass->colour[0] | pass->colour[1] | pass->colour[2] | pass->colour[3]) == 0) {
        flat_black(pass, QUAD_CORNERS);
    } else {
        func_80042818(pass);
    }
}

/* The tinted vertices: bit 19 of their words */
#define TINTED_VERTEX VERTEX_BIT_19

/* The tinted mode (the camera's flag 0x8000): the tint's colour into the GTE's far colour, its depth into the
 * scratchpad */
void func_80044980(WorldScene *scene) {
    u32 colour = scene->tint_colour;

    GTE_WRITE_CONTROL(GTE_RFC, (colour >> DEPTH_FOG_RED_SHIFT) & DEPTH_FOG_COMPONENT_MASK);
    GTE_WRITE_CONTROL(GTE_GFC, (colour >> DEPTH_FOG_GREEN_SHIFT) & DEPTH_FOG_COMPONENT_MASK);
    GTE_WRITE_CONTROL(GTE_BFC, colour & DEPTH_FOG_COMPONENT_MASK);
    WORLD_SCRATCH->tint = scene->tint;
}

/* The corners of tinted vertices moved toward the tint's colour */
static void tint_colours(WorldPass *pass, s32 corners) {
    s32 corner;

    GTE_LOAD_DATA(GTE_IR0, WORLD_SCRATCH->tint);
    for (corner = 0; corner < corners; corner++) {
        GTE_WRITE_DATA(GTE_RGBC, pass->colour[corner]);
        if (pass->word[corner] & TINTED_VERTEX) {
            Gte_Dpcs();
            GTE_READ_DATA(GTE_RGB2, pass->colour[corner]);
        } else {
            /* As it is */
        }
    }
}

/* The tinted mode's drawers: the default ones, the tinted vertices' colours moved first */
void func_800449B0(WorldPass *pass) {
    tint_colours(pass, TRIANGLE_CORNERS);
    func_80042548(pass);
}

void func_80044A04(WorldPass *pass) {
    tint_colours(pass, QUAD_CORNERS);
    func_80042818(pass);
}

/* The pulse: a triangle wave of the clock, 0 to 0x800 and back every 32 ticks */
#define PULSE_SHIFT 7
#define PULSE_MASK 0xFFF
#define PULSE_HALF 0x800
#define PULSE_PERIOD 0x1000
/* The dimmed vertices (bit 18 of the first's word): a corner black nearer than DIM_NEAR, dimmed out to DIM_FAR past it,
 * as it is farther */
#define DIM_NEAR 0x258
#define DIM_SPAN 0x2D8
#define DIM_SHIFT 5
/* The waving mode's dimmed polygons (all its vertices tinted) dim out farther */
#define DIM_SPAN_WAVING 0x3A0

/* The pulsing mode (the camera's flag 0x40): a black far colour, and the pulse into the scratchpad */
void func_80043D54(WorldScene *scene) {
    s32 pulse = ((u32)WORLD_SCRATCH->clock << PULSE_SHIFT) & PULSE_MASK;

    GTE_WRITE_CONTROL(GTE_RFC, 0);
    GTE_WRITE_CONTROL(GTE_GFC, 0);
    GTE_WRITE_CONTROL(GTE_BFC, 0);
    if (pulse - PULSE_HALF > 0) {
        pulse = PULSE_PERIOD - pulse;
    } else {
        /* Rising */
    }
    WORLD_SCRATCH->pulse = pulse;
}

/* The corners dimmed toward black by their depths; whether any colour is left */
static s32 dim_colours(WorldPass *pass, s32 corners, s32 span) {
    u32 depths[4];
    u32 left = 0;
    s32 corner, past, depth;

    if (corners == QUAD_CORNERS) {
        depths[0] = WORLD_SCRATCH->quad_first_z;
        GTE_READ_DATA(GTE_SZ1, depths[1]);
        GTE_READ_DATA(GTE_SZ2, depths[2]);
        GTE_READ_DATA(GTE_SZ3, depths[3]);
    } else {
        GTE_READ_DATA(GTE_SZ1, depths[0]);
        GTE_READ_DATA(GTE_SZ2, depths[1]);
        GTE_READ_DATA(GTE_SZ3, depths[2]);
    }
    for (corner = 0; corner < corners; corner++) {
        depth = depths[corner] - DIM_NEAR;
        past = span - depth;
        if (past < 0) {
            /* Beyond the dimming: as it is */
        } else {
            GTE_WRITE_DATA(GTE_RGBC, pass->colour[corner]);
            if (depth <= 0) {
                pass->colour[corner] = 0;
            } else {
                GTE_WRITE_DATA(GTE_IR0, past << DIM_SHIFT);
                Gte_Dpcs();
                GTE_READ_DATA(GTE_RGB2, pass->colour[corner]);
            }
        }
        left |= pass->colour[corner];
    }
    return left != 0;
}

/* The pulsing mode's drawers: where a corner's vertex has bit 19 set, the tinted ones pulse; else, the first's bit 18
 * set, the corners dim by depth (none drawn left black); then the default drawer */
static void pulse_polygon(WorldPass *pass, s32 corners, WorldStep draw) {
    u32 any = 0;
    s32 corner;

    for (corner = 0; corner < corners; corner++) {
        any |= pass->word[corner];
    }
    if (any & TINTED_VERTEX) {
        GTE_LOAD_DATA(GTE_IR0, WORLD_SCRATCH->pulse);
        for (corner = 0; corner < corners; corner++) {
            GTE_WRITE_DATA(GTE_RGBC, pass->colour[corner]);
            if (pass->word[corner] & TINTED_VERTEX) {
                Gte_Dpcs();
                GTE_READ_DATA(GTE_RGB2, pass->colour[corner]);
            } else {
                /* As it is */
            }
        }
    } else if (!(pass->word[0] & FOG_BRIGHT_VERTEX)) {
        /* As it is */
    } else if (!dim_colours(pass, corners, DIM_SPAN)) {
        /* Black: not drawn */
        return;
    } else {
        /* Dimmed */
    }
    draw(pass);
}

void func_80043D84(WorldPass *pass) {
    pulse_polygon(pass, TRIANGLE_CORNERS, func_80042548);
}

void func_80043EA0(WorldPass *pass) {
    pulse_polygon(pass, QUAD_CORNERS, func_80042818);
}

/* A lamp's light at a vertex: from LAMP_REACH less how far the vertex is along z, y and x (less LAMP_FLAT more), none
 * past it. The lamp and the vertex are VXY pairs, y's difference taking x's borrow */
#define LAMP_REACH 0x1320
#define LAMP_FLAT 0x320
/* The lit vertices' light, the two lamps' summed: none lights a corner with the lamps' far colour, full or more with
 * their full colour, between with their colour moved toward the far one */
#define LAMP_FULL 0x1000
/* The lit vertices: bit 18 set leaves the corner the GTE's last colour */
#define UNLIT_VERTEX VERTEX_BIT_18

/* A component's colour, its bits 4 to 11, from a colour word with red in bits 20 to 27 (the lamps' far colour and the
 * tint's), or with red in the low byte (the lamps' colour) shifted by 4 */
static void colour_components(u32 *components, u32 colour, s32 red_high) {
    if (red_high) {
        components[0] = (colour >> DEPTH_FOG_RED_SHIFT) & DEPTH_FOG_COMPONENT_MASK;
        components[1] = (colour >> DEPTH_FOG_GREEN_SHIFT) & DEPTH_FOG_COMPONENT_MASK;
        components[2] = colour & DEPTH_FOG_COMPONENT_MASK;
    } else {
        colour <<= GTE_COLOUR_OUT_SHIFT;
        components[0] = colour & DEPTH_FOG_COMPONENT_MASK;
        components[1] = (colour >> DEPTH_FOG_GREEN_SHIFT) & DEPTH_FOG_COMPONENT_MASK;
        components[2] = (colour >> DEPTH_FOG_RED_SHIFT) & DEPTH_FOG_COMPONENT_MASK;
    }
}

/* The lit mode (the camera's flag 0x100): the lamps' colours, positions and the tint into the scratchpad */
void func_80042C70(WorldScene *scene) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 lamp_far[AXES], lamp_full[AXES], tint_far[AXES];
    s32 axis;

    colour_components(lamp_far, scene->lamp_far, 1);
    for (axis = 0; axis < AXES; axis++) {
        scratch->lamp_far[axis] = lamp_far[axis];
    }
    scratch->shade = scene->shade;
    colour_components(lamp_full, scene->shade, 0);
    for (axis = 0; axis < AXES; axis++) {
        scratch->lamp_full[axis] = lamp_full[axis];
    }
    scratch->tint = scene->tint;
    colour_components(tint_far, scene->tint_colour, 1);
    for (axis = 0; axis < AXES; axis++) {
        scratch->tint_far[axis] = tint_far[axis];
    }
    scratch->lamp1[0] = scene->lamps[0][0];
    scratch->lamp1[1] = scene->lamps[0][1];
    scratch->lamp2[0] = scene->lamps[1][0];
    scratch->lamp2[1] = scene->lamps[1][1];
}

/* A number's magnitude, as the retail code negates it (the most negative word stays itself) */
static u32 magnitude(u32 value) {
    if ((s32)value < 0) {
        return 0 - value;
    } else {
        return value;
    }
}

static u32 lamp_light(u32 xy, s32 z, u32 *lamp) {
    u32 offset = xy - lamp[0];
    u32 light = LAMP_REACH - magnitude((u32)z - lamp[1]);

    if ((s32)light < 0) {
        return 0;
    } else {
        light -= magnitude((u32)((s32)offset >> GTE_HALF_BITS));
    }
    if ((s32)light < 0) {
        return 0;
    } else {
        light = light - LAMP_FLAT - magnitude((u32)((s32)(offset << GTE_HALF_BITS) >> GTE_HALF_BITS));
    }
    if ((s32)light < 0) {
        return 0;
    } else {
        return light;
    }
}

/* A corner lit (func_80042D10): its vertex's vector and word, its colour in the scratchpad; the result is the GTE's
 * last colour (RGB2), left as it was for an unlit vertex */
static void lamp_corner(u32 xy, s32 z, u32 word) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 light, between;

    if (word & TINTED_VERTEX) {
        GTE_WRITE_CONTROL(GTE_RFC, scratch->tint_far[0]);
        GTE_WRITE_CONTROL(GTE_GFC, scratch->tint_far[1]);
        GTE_WRITE_CONTROL(GTE_BFC, scratch->tint_far[2]);
        GTE_LOAD_DATA(GTE_IR0, scratch->tint);
        GTE_LOAD_DATA(GTE_RGBC, scratch->camera_z);
        Gte_Dpcs();
        return;
    } else if (word & UNLIT_VERTEX) {
        return;
    } else {
        light = lamp_light(xy, z, scratch->lamp1) + lamp_light(xy, z, scratch->lamp2);
    }
    between = LAMP_FULL - light;
    if (light == 0) {
        GTE_LOAD_DATA(GTE_IR1, scratch->lamp_far[0]);
        GTE_LOAD_DATA(GTE_IR2, scratch->lamp_far[1]);
        GTE_LOAD_DATA(GTE_IR3, scratch->lamp_far[2]);
    } else if ((s32)between <= 0) {
        GTE_LOAD_DATA(GTE_IR1, scratch->lamp_full[0]);
        GTE_LOAD_DATA(GTE_IR2, scratch->lamp_full[1]);
        GTE_LOAD_DATA(GTE_IR3, scratch->lamp_full[2]);
    } else {
        GTE_WRITE_CONTROL(GTE_RFC, scratch->lamp_far[0]);
        GTE_WRITE_CONTROL(GTE_GFC, scratch->lamp_far[1]);
        GTE_WRITE_CONTROL(GTE_BFC, scratch->lamp_far[2]);
        GTE_LOAD_DATA(GTE_RGBC, scratch->shade);
        GTE_WRITE_DATA(GTE_IR0, between);
        Gte_Dpcs();
    }
    GTE_WRITE_DATA(GTE_IR0, 0);
    GTE_LOAD_DATA(GTE_RGBC, scratch->camera_z);
    Gte_Dpcl();
}

static void lit_vectors(WorldPass *pass, s32 corners, u32 (*vectors)[2]);

/* The lit mode's corners: each lit by the lamps from its vertex's vector */
static void lamp_colours(WorldPass *pass, s32 corners) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 vectors[4][2];
    s32 corner;

    lit_vectors(pass, corners, vectors);
    for (corner = 0; corner < corners; corner++) {
        scratch->camera_z = pass->colour[corner];
        lamp_corner(vectors[corner][0], vectors[corner][1], scratch->lit_words[corner]);
        GTE_READ_DATA(GTE_RGB2, pass->colour[corner]);
    }
}

/* The lit modes' corners' vectors (the quad's first as its projector kept it); the scratchpad keeps the vertices'
 * words, the next entry's kind and the depth meanwhile, as the retail code did */
static void lit_vectors(WorldPass *pass, s32 corners, u32 (*vectors)[2]) {
    WorldScratch *scratch = WORLD_SCRATCH;
    s32 corner;

    for (corner = 0; corner < corners; corner++) {
        scratch->lit_words[corner] = pass->word[corner];
    }
    scratch->upcoming = pass->upcoming_entry & ENTRY_KIND_MASK;
    scratch->lit_depth = pass->depth;
    if (corners == QUAD_CORNERS) {
        vectors[0][0] = scratch->quad_first_vxy;
        vectors[0][1] = scratch->quad_first_vz;
        GTE_READ_DATA(GTE_VXY1, vectors[1][0]);
        GTE_READ_DATA(GTE_VZ1, vectors[1][1]);
        GTE_READ_DATA(GTE_VXY2, vectors[2][0]);
        GTE_READ_DATA(GTE_VZ2, vectors[2][1]);
        GTE_READ_DATA(GTE_VXY0, vectors[3][0]);
        GTE_READ_DATA(GTE_VZ0, vectors[3][1]);
    } else {
        GTE_READ_DATA(GTE_VXY0, vectors[0][0]);
        GTE_READ_DATA(GTE_VZ0, vectors[0][1]);
        GTE_READ_DATA(GTE_VXY1, vectors[1][0]);
        GTE_READ_DATA(GTE_VZ1, vectors[1][1]);
        GTE_READ_DATA(GTE_VXY2, vectors[2][0]);
        GTE_READ_DATA(GTE_VZ2, vectors[2][1]);
    }
}

/* The lit modes' quad projector: the first corner's vector kept before the default one pushes it out */
void func_80042698(WorldPass *pass) {
    GTE_STORE_DATA(GTE_VXY0, WORLD_SCRATCH->quad_first_vxy);
    GTE_STORE_DATA(GTE_VZ0, WORLD_SCRATCH->quad_first_vz);
    func_800426A0(pass);
}

/* The lit mode's drawers: the default ones, the corners lit first */
void func_80042E84(WorldPass *pass) {
    lamp_colours(pass, TRIANGLE_CORNERS);
    func_80042548(pass);
}

void func_80042F08(WorldPass *pass) {
    lamp_colours(pass, QUAD_CORNERS);
    func_80042818(pass);
}

/* The glowing mode's lamps: what is left of a lamp's reach at a vertex lights it; its first corner tries the lamps
 * farther and takes LAMP_FLAT off what is left, the others take the lamps near enough at their first */
#define GLOW_FIRST_REACH 0x15E0
#define GLOW_REACH 0x12C0
/* A tinted vertex glows more, an unlit one keeps its colour */
#define GLOW_TINTED (0x199 + 0x332)
#define SPECIAL_VERTICES (TINTED_VERTEX | UNLIT_VERTEX)

/* The lamps the glowing mode takes for a polygon's later corners */
enum { GLOW_NONE, GLOW_ONE, GLOW_BOTH };

/* The glowing mode (the camera's flags 4 or 0x800000): a black far colour, the lamps' positions into the scratchpad */
void func_80044A70(WorldScene *scene) {
    WorldScratch *scratch = WORLD_SCRATCH;

    GTE_WRITE_CONTROL(GTE_RFC, 0);
    GTE_WRITE_CONTROL(GTE_GFC, 0);
    GTE_WRITE_CONTROL(GTE_BFC, 0);
    scratch->lamp1[0] = scene->lamps[0][0];
    scratch->lamp1[1] = scene->lamps[0][1];
    scratch->lamp2[0] = scene->lamps[1][0];
    scratch->lamp2[1] = scene->lamps[1][1];
}

/* What is left of a lamp's reach at a vertex, less how far the vertex is along z, y and x: negative once it runs out */
static s32 glow_left(u32 xy, s32 z, u32 *lamp, u32 reach) {
    u32 offset = xy - lamp[0];
    u32 left = reach - magnitude((u32)z - lamp[1]);

    if ((s32)left < 0) {
        return left;
    } else {
        left -= magnitude((u32)((s32)offset >> GTE_HALF_BITS));
    }
    if ((s32)left < 0) {
        return left;
    } else {
        return left - magnitude((u32)((s32)(offset << GTE_HALF_BITS) >> GTE_HALF_BITS));
    }
}

static s32 at_least_zero(s32 value) {
    if (value < 0) {
        return 0;
    } else {
        return value;
    }
}

/* A corner's colour dimmed toward black by how little light it has (into RGB2) */
static void glow_corner(u32 word, s32 light) {
    WorldScratch *scratch = WORLD_SCRATCH;
    s32 between;

    if (word & TINTED_VERTEX) {
        light += GLOW_TINTED;
    } else if (word & UNLIT_VERTEX) {
        GTE_LOAD_DATA(GTE_RGB2, scratch->camera_z);
        return;
    } else {
        /* Lit by the lamps alone */
    }
    if (light == 0) {
        GTE_WRITE_DATA(GTE_RGB2, 0);
        return;
    } else {
        between = LAMP_FULL - light;
    }
    GTE_WRITE_DATA(GTE_IR0, between);
    GTE_LOAD_DATA(GTE_RGBC, scratch->camera_z);
    if (between <= 0) {
        GTE_LOAD_DATA(GTE_RGB2, scratch->camera_z);
    } else {
        Gte_Dpcs();
    }
}

/* The glowing mode's corners: the lamps the first corner is near light it and the later ones; near neither, with no
 * tinted or unlit vertex, the polygon is a black flat one (whether so) */
static s32 glow_colours(WorldPass *pass, s32 corners) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 vectors[4][2];
    u32 *lamp = scratch->lamp1;
    u32 special = 0;
    s32 lamps, corner, first, second, light;

    lit_vectors(pass, corners, vectors);
    /* The first corner's colour is in the scratchpad as the lamps are tried, the polygon flat or not */
    scratch->camera_z = pass->colour[0];
    first = glow_left(vectors[0][0], vectors[0][1], scratch->lamp1, GLOW_FIRST_REACH);
    if (first >= 0) {
        second = glow_left(vectors[0][0], vectors[0][1], scratch->lamp2, GLOW_FIRST_REACH);
    } else {
        second = glow_left(vectors[0][0], vectors[0][1], scratch->lamp2, GLOW_FIRST_REACH);
        lamp = scratch->lamp2;
    }
    if (first >= 0 && second >= 0) {
        lamps = GLOW_BOTH;
        light = at_least_zero(first - LAMP_FLAT);
        if (second - LAMP_FLAT >= 0) {
            light += second - LAMP_FLAT;
        } else {
            /* The second lamp too far once flattened */
        }
    } else if (first >= 0) {
        lamps = GLOW_ONE;
        light = at_least_zero(first - LAMP_FLAT);
    } else if (second >= 0) {
        lamps = GLOW_ONE;
        light = at_least_zero(second - LAMP_FLAT);
    } else {
        lamps = GLOW_NONE;
        light = 0;
        for (corner = 0; corner < corners; corner++) {
            special |= scratch->lit_words[corner];
        }
        if (!(special & SPECIAL_VERTICES)) {
            flat_black(pass, corners);
            return 1;
        } else {
            /* Some corner tinted or unlit */
        }
    }
    for (corner = 0; corner < corners; corner++) {
        if (corner == 0) {
            /* Its light found above */
        } else if (lamps == GLOW_BOTH) {
            light = at_least_zero(glow_left(vectors[corner][0], vectors[corner][1], scratch->lamp1, GLOW_REACH));
            second = glow_left(vectors[corner][0], vectors[corner][1], scratch->lamp2, GLOW_REACH);
            if (second >= 0) {
                light += second;
            } else {
                /* The second lamp too far */
            }
        } else if (lamps == GLOW_ONE) {
            light = at_least_zero(glow_left(vectors[corner][0], vectors[corner][1], lamp, GLOW_REACH));
        } else {
            light = 0;
        }
        scratch->camera_z = pass->colour[corner];
        glow_corner(scratch->lit_words[corner], light);
        GTE_READ_DATA(GTE_RGB2, pass->colour[corner]);
    }
    return 0;
}

/* The glowing mode's drawers: the default ones, the corners lit by the lamps first, or a black flat polygon */
void func_80044E48(WorldPass *pass) {
    if (!glow_colours(pass, TRIANGLE_CORNERS)) {
        func_80042548(pass);
    } else {
        /* Drawn black */
    }
}

void func_80044EC0(WorldPass *pass) {
    if (!glow_colours(pass, QUAD_CORNERS)) {
        func_80042818(pass);
    } else {
        /* Drawn black */
    }
}

/* The waving vertices (bit 18 of their words) move along y by a byte of the scratchpad's wave table, the index from
 * their position and the camera's; the swaying ones (the same bit) by a sine */
#define MOVING_VERTEX VERTEX_BIT_18
#define WAVE_SHIFT 4
#define WAVE_MASK 0xF

/* The waving mode (the camera's flag 0x200): the waves' phase from the camera's position */
void func_80043B5C(WorldScene *scene) {
    WorldScratch *scratch = WORLD_SCRATCH;

    scratch->wave_phase = scratch->camera_x + scratch->camera_y;
}

/* A waving vertex's VXY pair, moved */
static u32 waved(u32 xy, u32 phase) {
    u32 index = (((xy >> GTE_HALF_BITS) + phase + xy) >> WAVE_SHIFT) & WAVE_MASK;

    return xy + ((u32)WORLD_SCRATCH->waves[index] << GTE_HALF_BITS);
}

/* The waving mode's projectors: the waving corners moved in the GTE's vectors (a quad's fourth in the pass), then the
 * default projectors */
void func_80043B74(WorldPass *pass) {
    u32 phase = WORLD_SCRATCH->wave_phase;
    u32 xy;

    if (pass->word[0] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY0, xy);
        GTE_WRITE_DATA(GTE_VXY0, waved(xy, phase));
    } else {
        /* Still */
    }
    if (pass->word[1] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY1, xy);
        GTE_WRITE_DATA(GTE_VXY1, waved(xy, phase));
    } else {
        /* Still */
    }
    if (pass->word[2] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY2, xy);
        GTE_WRITE_DATA(GTE_VXY2, waved(xy, phase));
    } else {
        /* Still */
    }
    func_80042420(pass);
}

void func_80043C48(WorldPass *pass) {
    u32 phase = WORLD_SCRATCH->wave_phase;
    u32 xy;

    if (pass->word[0] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY0, xy);
        GTE_WRITE_DATA(GTE_VXY0, waved(xy, phase));
    } else {
        /* Still */
    }
    if (pass->word[1] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY1, xy);
        GTE_WRITE_DATA(GTE_VXY1, waved(xy, phase));
    } else {
        /* Still */
    }
    if (pass->word[2] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY2, xy);
        GTE_WRITE_DATA(GTE_VXY2, waved(xy, phase));
    } else {
        /* Still */
    }
    if (pass->word[3] & MOVING_VERTEX) {
        pass->quad_last_xy = waved(pass->quad_last_xy, phase);
    } else {
        /* Still */
    }
    func_800426A0(pass);
}

/* The waving mode's drawers: a polygon whose vertices are all tinted dims by depth (none drawn left black), else the
 * default drawer */
static void wave_polygon(WorldPass *pass, s32 corners, WorldStep draw) {
    u32 all = ~0;
    s32 corner;

    for (corner = 0; corner < corners; corner++) {
        all &= pass->word[corner];
    }
    if ((all & TINTED_VERTEX) && !dim_colours(pass, corners, DIM_SPAN_WAVING)) {
        /* Black: not drawn */
        return;
    } else {
        draw(pass);
    }
}

void func_80043C2C(WorldPass *pass) {
    wave_polygon(pass, TRIANGLE_CORNERS, func_80042548);
}

void func_80043D34(WorldPass *pass) {
    wave_polygon(pass, QUAD_CORNERS, func_80042818);
}

/* The swaying vertices (bit 18 of their words) move by a sine of their position, the camera's and the clock: its
 * angle in 32ths of a turn, the table's quarter wave in 16 steps */
#define SWAY_SHIFT 4
#define SWAY_FLIP 0x20
#define SWAY_QUADRANT_SHIFT 4
#define SWAY_QUADRANT_MASK 3
#define SWAY_STEP_MASK 0xF
#define SWAY_STEP_ENTRIES 0x40
#define SWAY_COLOUR_MASK 0xFFFFFF

/* The swaying mode (the camera's flag 8): the sway's phase from the camera's position, its replacement colour, clock
 * shift and size into the scratchpad */
void func_8004299C(WorldScene *scene) {
    WorldScratch *scratch = WORLD_SCRATCH;

    scratch->wave_phase = scratch->camera_x + scratch->camera_z;
    scratch->mode_colours[0] = scene->tint_colour & SWAY_COLOUR_MASK;
    scratch->sway_shift = scene->sway_shift;
    scratch->sway_scale = scene->sway_scale;
}

/* A swaying vertex moved (func_800429D0): its VXY pair and z by the sine and cosine of its angle, shifted by the sway's
 * size, the quadrant (bits 4 and 5 of the angle) choosing which goes where and its sign */
static void swayed(u32 *xy, u32 *z, s32 clock, s32 scale, u32 phase) {
    s32 angle = (s32)(*z + phase + *xy) >> SWAY_SHIFT;
    s32 step, cosine, sine;

    if (angle & SWAY_FLIP) {
        angle = -(angle + clock);
    } else {
        angle += clock;
    }
    step = (angle & SWAY_STEP_MASK) * SWAY_STEP_ENTRIES;
    cosine = (s32)((u32)g_SinTable[ANGLE_QUARTER - step] << GTE_HALF_BITS) >> (scale & SHIFT_COUNT_MASK);
    sine = (s32)((u32)g_SinTable[step] << GTE_HALF_BITS) >> (scale & SHIFT_COUNT_MASK);
    switch ((angle >> SWAY_QUADRANT_SHIFT) & SWAY_QUADRANT_MASK) {
        case 0:
            *z += cosine;
            *xy += sine;
            break;
        case 1:
            *xy += cosine;
            *z -= sine;
            break;
        case 2:
            *z -= cosine;
            *xy -= sine;
            break;
        default:
            *xy -= cosine;
            *z += sine;
            break;
    }
}

/* The swaying mode's projectors: the swaying corners moved (in the GTE's vectors; the triangle's third and the quad's
 * fourth from what the dispatcher loaded, its z whole), then the default projectors; the scratchpad keeps the next
 * entry meanwhile, as the retail code did */
static void sway_corners(WorldPass *pass, s32 corners) {
    WorldScratch *scratch = WORLD_SCRATCH;
    s32 clock = scratch->clock >> (scratch->sway_shift & SHIFT_COUNT_MASK);
    s32 scale = scratch->sway_scale;
    u32 phase = scratch->wave_phase;
    u32 xy, z;

    scratch->upcoming = pass->upcoming_entry;
    if (pass->word[0] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY0, xy);
        GTE_READ_DATA(GTE_VZ0, z);
        swayed(&xy, &z, clock, scale, phase);
        GTE_WRITE_DATA(GTE_VXY0, xy);
        GTE_WRITE_DATA(GTE_VZ0, z);
    } else {
        /* Still */
    }
    if (pass->word[1] & MOVING_VERTEX) {
        GTE_READ_DATA(GTE_VXY1, xy);
        GTE_READ_DATA(GTE_VZ1, z);
        swayed(&xy, &z, clock, scale, phase);
        GTE_WRITE_DATA(GTE_VXY1, xy);
        GTE_WRITE_DATA(GTE_VZ1, z);
    } else {
        /* Still */
    }
    if (!(pass->word[2] & MOVING_VERTEX)) {
        /* Still */
    } else if (corners == QUAD_CORNERS) {
        GTE_READ_DATA(GTE_VXY2, xy);
        GTE_READ_DATA(GTE_VZ2, z);
        swayed(&xy, &z, clock, scale, phase);
        GTE_WRITE_DATA(GTE_VXY2, xy);
        GTE_WRITE_DATA(GTE_VZ2, z);
    } else {
        GTE_READ_DATA(GTE_VXY2, xy);
        z = pass->vertex_z[2];
        swayed(&xy, &z, clock, scale, phase);
        GTE_WRITE_DATA(GTE_VXY2, xy);
        GTE_WRITE_DATA(GTE_VZ2, z);
    }
    if (corners == QUAD_CORNERS && (pass->word[3] & MOVING_VERTEX)) {
        xy = pass->quad_last_xy;
        z = pass->quad_last_z;
        swayed(&xy, &z, clock, scale, phase);
        pass->quad_last_xy = xy;
        pass->quad_last_z = z;
    } else {
        /* Still, or no fourth corner */
    }
}

void func_80042AB8(WorldPass *pass) {
    sway_corners(pass, TRIANGLE_CORNERS);
    func_80042420(pass);
}

void func_80042B7C(WorldPass *pass) {
    sway_corners(pass, QUAD_CORNERS);
    func_800426A0(pass);
}

/* The swaying mode's drawers: the tinted vertices' corners in its replacement colour, when it isn't 0, then the default
 * drawers */
static void sway_colours(WorldPass *pass, s32 corners) {
    u32 colour = WORLD_SCRATCH->mode_colours[0];
    s32 corner;

    if (colour == 0) {
        return;
    } else {
        for (corner = 0; corner < corners; corner++) {
            if (pass->word[corner] & TINTED_VERTEX) {
                pass->colour[corner] = colour;
            } else {
                /* Its own colour */
            }
        }
    }
}

void func_80042B44(WorldPass *pass) {
    sway_colours(pass, TRIANGLE_CORNERS);
    func_80042548(pass);
}

void func_80042C2C(WorldPass *pass) {
    sway_colours(pass, QUAD_CORNERS);
    func_80042818(pass);
}

/* The light around Crash (the game's flag 0x1000000, its light's entry loaded): a polygon with a lit vertex (bit 18)
 * whose corners come within 0x400 of him along z and along x is divided into a grid of 45 points, each shaded and
 * raised from the light's 16 by 16 maps by its distance from him, and drawn as the triangles a strip list makes of
 * them, each in its own slot, with the polygon itself 40 slots farther */
#define LIGHT_CENTRE 0x200
#define LIT_VERTEX VERTEX_BIT_18
/* Where func_800461E0, alone, keeps its return address */
#define LIGHT_RETURN 0x1F80018C
#define LIGHT_RETURN_BYTES 4
#define LIGHT_NEAR_MASK 0xFC00
#define LIGHT_ROWS 8
#define LIGHT_MAP_SHIFT 4
#define LIGHT_MAP_MASK 0xFFF0
#define LIGHT_SHADE_SHIFT 5
#define LIGHT_UNLIT 0xFF
#define LIGHT_NEARER 5
#define LIGHT_BEHIND 40
/* The walk's fixed point: positions, colours and the texture's corners in 8ths; the distances from Crash in 64ths
 * along z (z << 6) and x, a step an 8th of the corners' difference */
#define WALK_SHIFT 3
#define WALK_Z_SHIFT 6
#define WALK_X_SHIFT 22
#define WALK_COLOUR_MASK 0x7FFF
#define WALK_PAIR_MASK 0xFFFF7FFF
#define LIGHT_Y_SHIFT 4
#define LIGHT_Y_PAIR_SHIFT 17
#define LIGHT_V_SHIFT 5
#define LIGHT_V_MASK 0xFF00
#define COLOUR_INVALID 0xFFFFFFFF

/* A point of the light's grid: its colour (all bits set when unlit), screen position, depth and texture corner */
typedef struct {
    u32 colour;
    u32 screen;
    s16 depth;
    u16 uv;
} LightPoint;

/* The triangles of the grid: strips of point indices, each started by one with bit 7 set, the list ended by -1 */
extern s8 D_8005C6D0[];
#define STRIP_START 0x80
#define STRIP_INDEX_MASK 0x7F
#define STRIPS_END (-1)

/* The light's mode: a black far colour and depth, its maps, its grid's buffer and Crash's place into the scratchpad */
void func_80045C18(WorldScene *scene) {
    WorldScratch *scratch = WORLD_SCRATCH;
    s32 x, z;

    GTE_WRITE_CONTROL(GTE_RFC, 0);
    GTE_WRITE_CONTROL(GTE_GFC, 0);
    GTE_WRITE_CONTROL(GTE_BFC, 0);
    GTE_WRITE_DATA(GTE_IR0, 0);
    x = scene->crash_x;
    z = scene->crash_z;
    scratch->light_heights = scene->light_heights;
    scratch->light_shades = scene->light_shades;
    scratch->light_grid = scene->light_grid;
    scratch->light_x = x + LIGHT_CENTRE;
    scratch->light_z = z + LIGHT_CENTRE;
}

/* Whether a polygon is lit: one of its vertices lit, and one of its first three corners near Crash along z, one along x
 */
static s32 light_near(WorldPass *pass, s32 corners) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 words = 0;
    u32 z[3], xy[3];
    s32 corner;

    for (corner = 0; corner < corners; corner++) {
        words |= pass->word[corner];
    }
    if (!(words & LIT_VERTEX)) {
        return 0;
    } else {
        GTE_READ_DATA(GTE_VZ0, z[0]);
        GTE_READ_DATA(GTE_VZ1, z[1]);
        GTE_READ_DATA(GTE_VZ2, z[2]);
    }
    if (((scratch->light_z - z[0]) & LIGHT_NEAR_MASK) && ((scratch->light_z - z[1]) & LIGHT_NEAR_MASK) &&
        ((scratch->light_z - z[2]) & LIGHT_NEAR_MASK)) {
        return 0;
    } else {
        GTE_READ_DATA(GTE_VXY0, xy[0]);
        GTE_READ_DATA(GTE_VXY1, xy[1]);
        GTE_READ_DATA(GTE_VXY2, xy[2]);
    }
    return !(((scratch->light_x - xy[0]) & LIGHT_NEAR_MASK) && ((scratch->light_x - xy[1]) & LIGHT_NEAR_MASK) &&
             ((scratch->light_x - xy[2]) & LIGHT_NEAR_MASK));
}

/* One of the light's triangles (func_80045CAC): shaded, textured with the light's texture words when the polygon is,
 * into its own slot unless that is below the lowest linked so far (own_slot 0: linked as the other polygons) */
static void light_emit(WorldPass *pass, u32 *slot, s32 own_slot, s32 textured, u32 *colours, u32 *screens) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 *primitive = pass->next;
    s32 words;

    if (!textured) {
        primitive[1] = colours[0] | POLY_G3_CODE;
        primitive[3] = colours[1];
        primitive[5] = colours[2];
        primitive[2] = screens[0];
        primitive[4] = screens[1];
        primitive[6] = screens[2];
        words = POLY_G3_WORDS;
    } else {
        primitive[3] = scratch->light_texture[0];
        primitive[6] = scratch->light_texture[1];
        primitive[9] = scratch->light_texture[2];
        primitive[2] = screens[0];
        primitive[5] = screens[1];
        primitive[8] = screens[2];
        primitive[1] = colours[0] | POLY_GT3_CODE;
        primitive[4] = colours[1];
        primitive[7] = colours[2];
        words = POLY_GT3_WORDS;
    }
    if (own_slot && slot >= pass->lowest) {
        primitive[0] = *slot | ((u32)(words - 1) << TAG_LENGTH_SHIFT);
        *slot = (u32)primitive & PRIM_ADDRESS_MASK;
        pass->next = primitive + words;
    } else {
        pass->slot = slot;
        link_primitive(pass, words);
    }
}

/* A triangle of three grid points (func_80045C5C): none if one is unlit, else into the slot of their depth, a little
 * nearer than their polygon's */
static void light_points(WorldPass *pass, s32 textured, LightPoint **points) {
    u32 colours[3], screens[3];
    s32 sum, index, corner;
    u32 *slot;

    for (corner = 0; corner < 3; corner++) {
        colours[corner] = points[corner]->colour;
        screens[corner] = points[corner]->screen;
    }
    if ((s32)(colours[0] | colours[1] | colours[2]) < 0) {
        return;
    } else {
        sum = (s16)(points[0]->depth + points[1]->depth + points[2]->depth);
    }
    index = DEPTH_DEEPEST - (s32)((u32)sum >> DEPTH_SUM_SHIFT) - LIGHT_NEARER;
    if (index - DEPTH_DEEPEST >= 0) {
        index = DEPTH_DEEPEST;
    } else {
        /* In the table, or past its far end */
    }
    if (index >= 0) {
        slot = &pass->ot[index];
    } else {
        slot = pass->ot;
    }
    light_emit(pass, slot, 1, textured, colours, screens);
}

/* A lit triangle (func_800461E0): its corners on the GTE (vectors, screen positions and depths) with their colours; the
 * grid walked, then drawn by the strip list, then the triangle itself */
static void light_triangle(WorldPass *pass, u32 material, u32 first, u32 second, u32 third) {
    WorldScratch *scratch = WORLD_SCRATCH;
    LightWalk *walk = &scratch->walk;
    LightPoint *point, *points[3];
    u32 xy[3], words[3];
    s32 z[3], near[3], past[3];
    u32 colour, shade, height;
    s32 textured, row, column, index, corner, u, v;
    s8 *strip;
    s32 next;

    scratch->mode_colours[0] = first;
    scratch->mode_colours[1] = second;
    scratch->mode_colours[2] = third;
    GTE_STORE_DATA(GTE_SXY0, scratch->light_corners[0][0]);
    GTE_STORE_DATA(GTE_SXY1, scratch->light_corners[1][0]);
    GTE_STORE_DATA(GTE_SXY2, scratch->light_corners[2][0]);
    GTE_STORE_DATA(GTE_SZ1, scratch->light_corners[0][1]);
    GTE_STORE_DATA(GTE_SZ2, scratch->light_corners[1][1]);
    GTE_STORE_DATA(GTE_SZ3, scratch->light_corners[2][1]);
    GTE_READ_DATA(GTE_VXY0, xy[0]);
    GTE_READ_DATA(GTE_VXY1, xy[1]);
    GTE_READ_DATA(GTE_VXY2, xy[2]);
    walk->x_row = walk->x = (s32)(xy[0] << GTE_HALF_BITS) >> (GTE_HALF_BITS - WALK_SHIFT);
    walk->x_along = xy[1] - xy[0];
    walk->x_down = xy[2] - xy[0];
    walk->y_row = walk->y = ((s32)xy[0] >> GTE_HALF_BITS) << WALK_SHIFT;
    walk->y_along = ((s32)xy[1] >> GTE_HALF_BITS) - ((s32)xy[0] >> GTE_HALF_BITS);
    walk->y_down = ((s32)xy[2] >> GTE_HALF_BITS) - ((s32)xy[0] >> GTE_HALF_BITS);
    GTE_READ_DATA(GTE_VZ0, z[0]);
    GTE_READ_DATA(GTE_VZ1, z[1]);
    GTE_READ_DATA(GTE_VZ2, z[2]);
    walk->z_row = walk->z = z[0] << WALK_SHIFT;
    walk->z_along = z[1] - z[0];
    walk->z_down = z[2] - z[0];
    walk->rg_row = walk->rg =
        (u16)((first & GTE_COLOUR_MAX) << WALK_SHIFT) |
        ((u32)(u16)(((first >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX) << WALK_SHIFT) << GTE_HALF_BITS);
    walk->rg_along = (((second & GTE_COLOUR_MAX) - (first & GTE_COLOUR_MAX)) & WALK_COLOUR_MASK) |
                     ((u32)(u16)(((second >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX) -
                                 ((first >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX)) << GTE_HALF_BITS);
    walk->rg_down = (((third & GTE_COLOUR_MAX) - (first & GTE_COLOUR_MAX)) & WALK_COLOUR_MASK) |
                    ((u32)(u16)(((third >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX) -
                                ((first >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX)) << GTE_HALF_BITS);
    walk->b_row = walk->b = (first >> GTE_HALF_BITS) << WALK_SHIFT;
    walk->b_along = (second >> GTE_HALF_BITS) - (first >> GTE_HALF_BITS);
    walk->b_down = (third >> GTE_HALF_BITS) - (first >> GTE_HALF_BITS);
    for (corner = 0; corner < 3; corner++) {
        near[corner] = (s32)(scratch->light_z - z[corner]) >> WALK_Z_SHIFT;
        past[corner] = (s32)(((u32)scratch->light_x << GTE_HALF_BITS) - (xy[corner] << GTE_HALF_BITS)) >> WALK_X_SHIFT;
    }
    walk->near_row = walk->near = (near[0] & WALK_COLOUR_MASK) | ((u32)(u16)past[0] << GTE_HALF_BITS);
    walk->near_along = (((near[1] - near[0]) >> WALK_SHIFT) & WALK_COLOUR_MASK) |
                       ((u32)(u16)((past[1] - past[0]) >> WALK_SHIFT) << GTE_HALF_BITS);
    walk->near_down = (((near[2] - near[0]) >> WALK_SHIFT) & WALK_COLOUR_MASK) |
                      ((u32)(u16)((past[2] - past[0]) >> WALK_SHIFT) << GTE_HALF_BITS);
    textured = (material & MATERIAL_MASK) != 0;
    if (textured) {
        triangle_texture(pass, words);
        for (corner = 0; corner < 3; corner++) {
            scratch->light_texture[corner] = words[corner];
            scratch->light_saved_texture[corner] = words[corner];
        }
        walk->uv_row = walk->uv = (u16)((words[0] & GTE_COLOUR_MAX) << WALK_SHIFT) |
                                  ((u32)(u16)(((words[0] >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX) << WALK_SHIFT)
                                   << GTE_HALF_BITS);
        walk->uv_along = (((words[1] & GTE_COLOUR_MAX) - (words[0] & GTE_COLOUR_MAX)) & WALK_COLOUR_MASK) |
                         ((u32)(u16)(((words[1] >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX) -
                                     ((words[0] >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX)) << GTE_HALF_BITS);
        walk->uv_down = (((words[2] & GTE_COLOUR_MAX) - (words[0] & GTE_COLOUR_MAX)) & WALK_COLOUR_MASK) |
                        ((u32)(u16)(((words[2] >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX) -
                                    ((words[0] >> GTE_COLOUR_BITS) & GTE_COLOUR_MAX)) << GTE_HALF_BITS);
    } else {
        /* The texture's walk left as it was */
    }

    point = (LightPoint *)scratch->light_grid;
    for (row = LIGHT_ROWS; row >= 0; row--) {
        for (column = row; column >= 0; column--) {
            point->colour = COLOUR_INVALID;
            u = (s16)walk->near;
            v = (s16)(walk->near >> GTE_HALF_BITS);
            if (!(u & LIGHT_MAP_MASK) && !(v & LIGHT_MAP_MASK)) {
                index = u + (v << LIGHT_MAP_SHIFT);
                colour = ((u32)(s32)(s16)walk->rg >> WALK_SHIFT) |
                         (((u32)(s32)(s16)(walk->rg >> GTE_HALF_BITS) >> WALK_SHIFT) << GTE_COLOUR_BITS) |
                         (((u32)(s32)walk->b >> WALK_SHIFT) << GTE_HALF_BITS);
                GTE_WRITE_DATA(GTE_RGBC, colour);
                shade = (u32)scratch->light_shades[index] << LIGHT_SHADE_SHIFT;
                GTE_WRITE_DATA(GTE_IR1, shade);
                GTE_WRITE_DATA(GTE_IR2, shade);
                GTE_WRITE_DATA(GTE_IR3, shade);
                height = scratch->light_heights[index];
                Gte_Dpcl();
                if (height != LIGHT_UNLIT) {
                    GTE_WRITE_DATA(GTE_VXY0, ((u32)(walk->x >> WALK_SHIFT) & GTE_HALF_MASK) |
                                                 ((u32)((s32)(walk->y + (height << WALK_SHIFT)) >> LIGHT_Y_SHIFT)
                                                  << LIGHT_Y_PAIR_SHIFT));
                    GTE_WRITE_DATA(GTE_VZ0, walk->z >> WALK_SHIFT);
                    GTE_STORE_DATA(GTE_RGB2, point->colour);
                    u = (s16)walk->uv;
                    v = (s16)(walk->uv >> GTE_HALF_BITS);
                    Gte_Rtps();
                    if (textured) {
                        point->uv = ((u32)u >> WALK_SHIFT) | ((v << LIGHT_V_SHIFT) & LIGHT_V_MASK);
                    } else {
                        /* Untextured: no corner */
                    }
                    GTE_READ_DATA(GTE_SZ3, colour);
                    GTE_STORE_DATA(GTE_SXY2, point->screen);
                    point->depth = colour;
                } else {
                    /* Out of the light */
                }
            } else {
                /* Off the light's maps */
            }
            walk->near = (walk->near + walk->near_along) & WALK_PAIR_MASK;
            walk->uv = (walk->uv + walk->uv_along) & WALK_PAIR_MASK;
            walk->rg = (walk->rg + walk->rg_along) & WALK_PAIR_MASK;
            walk->b += walk->b_along;
            walk->x += walk->x_along;
            walk->y += walk->y_along;
            walk->z += walk->z_along;
            point++;
        }
        walk->near_row = (walk->near_row + walk->near_down) & WALK_PAIR_MASK;
        walk->uv_row = (walk->uv_row + walk->uv_down) & WALK_PAIR_MASK;
        walk->near = walk->near_row;
        walk->uv = walk->uv_row;
        walk->rg_row = (walk->rg_row + walk->rg_down) & WALK_PAIR_MASK;
        walk->b_row += walk->b_down;
        walk->rg = walk->rg_row;
        walk->b = walk->b_row;
        walk->x_row += walk->x_down;
        walk->x = walk->x_row;
        walk->y_row += walk->y_down;
        walk->y = walk->y_row;
        walk->z_row += walk->z_down;
        walk->z = walk->z_row;
    }

    strip = D_8005C6D0;
    next = *strip++;
    while (next != STRIPS_END) {
        points[0] = (LightPoint *)scratch->light_grid + (next & STRIP_INDEX_MASK);
        *(u16 *)&scratch->light_texture[0] = points[0]->uv;
        points[1] = (LightPoint *)scratch->light_grid + strip[0];
        *(u16 *)&scratch->light_texture[1] = points[1]->uv;
        next = strip[1];
        strip += 2;
        for (;;) {
            points[2] = (LightPoint *)scratch->light_grid + next;
            *(u16 *)&scratch->light_texture[2] = points[2]->uv;
            light_points(pass, textured, points);
            next = *strip++;
            if (next < 0) {
                /* The next strip, or the list's end */
                break;
            } else {
                points[0] = points[1];
                points[1] = points[2];
                *(u16 *)&scratch->light_texture[0] = *(u16 *)&scratch->light_texture[1];
                *(u16 *)&scratch->light_texture[1] = *(u16 *)&scratch->light_texture[2];
            }
        }
    }

    for (corner = 0; corner < 3; corner++) {
        words[corner] = scratch->light_corners[corner][0];
        scratch->light_texture[corner] = scratch->light_saved_texture[corner];
    }
    light_emit(pass, scratch->light_slot - LIGHT_BEHIND, 0, textured, scratch->mode_colours, words);
}

/* The light's projectors: an unlit polygon goes to the default ones; a lit one is projected, culled and coloured here,
 * its next polygon read after its first colour, and drawn by the light (a quad as its last three corners' triangle,
 * then its first three's), not by the scene's drawers */
void func_80045D88(WorldPass *pass) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 points[3];

    if (!light_near(pass, TRIANGLE_CORNERS)) {
        func_80042420(pass);
        return;
    } else {
        Gte_Rtpt();
    }
    pass->colour[0] = vertex_colour(pass, 0);
    read_polygon(pass->upcoming, pass->upcoming_entry);
    GTE_READ_DATA(GTE_SXY0, points[0]);
    GTE_READ_DATA(GTE_SXY1, points[1]);
    GTE_READ_DATA(GTE_SXY2, points[2]);
    if (off_screen(points, TRIANGLE_CORNERS)) {
        return;
    } else {
        pass->colour[1] = vertex_colour(pass, 1);
        pass->colour[2] = vertex_colour(pass, 2);
    }
    depth_slot(pass);
    scratch->light_depth = pass->depth;
    scratch->light_slot = pass->slot;
    light_triangle(pass, pass->material, pass->colour[0], pass->colour[1], pass->colour[2]);
}

void func_80045F40(WorldPass *pass) {
    WorldScratch *scratch = WORLD_SCRATCH;
    u32 points[4];

    if (!light_near(pass, QUAD_CORNERS)) {
        func_800426A0(pass);
        return;
    } else {
        Gte_Rtpt();
    }
    pass->colour[0] = vertex_colour(pass, 0);
    read_polygon(pass->upcoming, pass->upcoming_entry);
    GTE_STORE_DATA(GTE_SXY0, scratch->quad_first_xy);
    GTE_STORE_DATA(GTE_SZ1, scratch->quad_first_z);
    GTE_STORE_DATA(GTE_VXY0, scratch->quad_first_vxy);
    GTE_STORE_DATA(GTE_VZ0, scratch->quad_first_vz);
    GTE_WRITE_DATA(GTE_VXY0, pass->quad_last_xy);
    GTE_WRITE_DATA(GTE_VZ0, pass->quad_last_z);
    scratch->light_last_xy = pass->quad_last_xy;
    scratch->light_last_z = pass->quad_last_z;
    GTE_READ_DATA(GTE_SXY0, points[0]);
    GTE_READ_DATA(GTE_SXY1, points[1]);
    GTE_READ_DATA(GTE_SXY2, points[2]);
    Gte_Rtps();
    pass->colour[1] = vertex_colour(pass, 1);
    GTE_READ_DATA(GTE_SXY2, points[3]);
    if (off_screen(points, QUAD_CORNERS)) {
        return;
    } else {
        pass->colour[2] = vertex_colour(pass, 2);
        pass->colour[3] = vertex_colour(pass, 3);
    }
    depth_slot(pass);
    scratch->light_slot = pass->slot;
    scratch->light_depth = pass->depth;
    scratch->rotated = ROTATED_FLAG;
    scratch->light_material = pass->material;
    GTE_STORE_DATA(GTE_VXY1, scratch->light_vectors[0][0]);
    GTE_STORE_DATA(GTE_VZ1, scratch->light_vectors[0][1]);
    GTE_LOAD_DATA(GTE_VXY0, scratch->light_vectors[0][0]);
    GTE_LOAD_DATA(GTE_VZ0, scratch->light_vectors[0][1]);
    GTE_STORE_DATA(GTE_VXY2, scratch->light_vectors[1][0]);
    GTE_STORE_DATA(GTE_VZ2, scratch->light_vectors[1][1]);
    GTE_LOAD_DATA(GTE_VXY1, scratch->light_vectors[1][0]);
    GTE_LOAD_DATA(GTE_VZ1, scratch->light_vectors[1][1]);
    GTE_LOAD_DATA(GTE_VXY2, scratch->light_last_xy);
    GTE_LOAD_DATA(GTE_VZ2, scratch->light_last_z);
    scratch->light_first_colour = pass->colour[0];
    light_triangle(pass, pass->material, pass->colour[1], pass->colour[2], pass->colour[3]);

    GTE_LOAD_DATA(GTE_SXY1, scratch->light_corners[0][0]);
    GTE_LOAD_DATA(GTE_SXY2, scratch->light_corners[1][0]);
    GTE_LOAD_DATA(GTE_SXY0, scratch->quad_first_xy);
    GTE_LOAD_DATA(GTE_VXY1, scratch->light_vectors[0][0]);
    GTE_LOAD_DATA(GTE_VZ1, scratch->light_vectors[0][1]);
    GTE_LOAD_DATA(GTE_VXY2, scratch->light_vectors[1][0]);
    GTE_LOAD_DATA(GTE_VZ2, scratch->light_vectors[1][1]);
    GTE_LOAD_DATA(GTE_VXY0, scratch->quad_first_vxy);
    GTE_LOAD_DATA(GTE_VZ0, scratch->quad_first_vz);
    scratch->rotated = 0;
    light_triangle(pass, scratch->light_material, scratch->light_first_colour, scratch->mode_colours[0],
                   scratch->mode_colours[1]);
}

/* What the world renderer reads of its arguments: the frame up to its ordering table's end, the scene up to its light's
 * grid (checked as the build compiles) */
#define FRAME_BYTES 0x2018
#define WORLD_SCENE_BYTES 0xB4
typedef char check_world_scene[(sizeof(WorldScene) == WORLD_SCENE_BYTES) ? 1 : -1];
typedef char check_frame[((u32)&((FrameBuffer *)0)->ot[OT_SLOTS] == FRAME_BYTES) ? 1 : -1];

/* Every caller (Sys_MainLoop, func_80018434) passes the zone's sort list, the frame being built, D_80062A18 and
 * D_80060944, the textures' clock: a heap buffer of Gfx_AllocSortLists's, one of Gfx_AllocFrameBuffers's and a global,
 * apart. A sort list holds 1 to SORT_LIST_ENTRIES entries: the disc's whole lists hold 1 to 1,026 but one of 0
 * (S2/S0000022.NSF, an SLST's last item, 4 bytes after an empty delta), which the retail code can't draw (it reads on
 * below the list); the deltas build into Gfx_AllocSortLists's buffers. The scene's five words from 0x10 are one of the
 * 10 modes func_80011E24 and func_80020A24 store. The default mode's setup is func_8003DD0C, an empty function: a call
 * to it leaves $v1, the scratchpad's base the retail code keeps across it. The group's parts go with it; the other
 * functions between them (the object renderers' routines and their helpers) stay, never reached */
EQUIV_SCRATCH(func_80041E5C, SAVE_AREA, SAVE_AREA_BYTES)
EQUIV_SCRATCH(func_80041E5C, LIGHT_RETURN, LIGHT_RETURN_BYTES)
EQUIV_NONNULL(func_80041E5C, 0)
EQUIV_NONNULL(func_80041E5C, 1)
EQUIV_NONNULL(func_80041E5C, 2)
EQUIV_BUFFER(func_80041E5C, 0, SORT_LIST_BYTES)
EQUIV_BUFFER(func_80041E5C, 1, FRAME_BYTES)
EQUIV_BUFFER(func_80041E5C, 2, WORLD_SCENE_BYTES)
EQUIV_HALF(func_80041E5C, 0, 0, 1, SORT_LIST_ENTRIES)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_8003DD0C, func_80042420, func_800426A0, func_80042548, func_80042818)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_8004599C, func_80042420, func_800426A0, func_800459A8, func_80045AB4)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_8004399C, func_80042420, func_800426A0, func_800439D4, func_80043A84)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_80044980, func_80042420, func_800426A0, func_800449B0, func_80044A04)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_80043D54, func_80042420, func_800426A0, func_80043D84, func_80043EA0)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_80042C70, func_80042420, func_80042698, func_80042E84, func_80042F08)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_80044A70, func_80042420, func_80042698, func_80044E48, func_80044EC0)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_80043B5C, func_80043B74, func_80043C48, func_80043C2C, func_80043D34)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_8004299C, func_80042AB8, func_80042B7C, func_80042B44, func_80042C2C)
EQUIV_WORDS(func_80041E5C, 2, 0x10, func_80045C18, func_80045D88, func_80045F40, func_80042548, func_80042818)
EQUIV_KEEPS(func_80041E5C, func_8003DD0C, v1)
HANDWRITTEN_PARTS(func_80041E5C, func_80042420, func_80042548, func_800426A0, func_80042818)
HANDWRITTEN_PARTS(func_80041E5C, func_8004599C, func_800459A8, func_80045AB4)
HANDWRITTEN_PARTS(func_80041E5C, func_8004399C, func_800439D4, func_80043A84)
HANDWRITTEN_PARTS(func_80041E5C, func_80044980, func_800449B0, func_80044A04)
HANDWRITTEN_PARTS(func_80041E5C, func_80043D54, func_80043D84, func_80043EA0)
HANDWRITTEN_PARTS(func_80041E5C, func_80042698, func_80042C70, func_80042D10, func_80042E84, func_80042F08)
HANDWRITTEN_PARTS(func_80041E5C, func_80044A70, func_80044BB4, func_80044E48, func_80044EC0)
HANDWRITTEN_PARTS(func_80041E5C, func_80043B5C, func_80043B74, func_80043C2C, func_80043C48, func_80043D34)
HANDWRITTEN_PARTS(func_80041E5C, func_8004299C, func_800429D0, func_80042AB8, func_80042B44, func_80042B7C)
HANDWRITTEN_PARTS(func_80041E5C, func_80042C2C, func_80045C18, func_80045C5C, func_80045CAC, func_80045D88)
HANDWRITTEN_PARTS(func_80041E5C, func_80045F40, func_800461E0)
EQUIV_RUNS(func_80041E5C, func_80042FA8, func_80042FD0, func_80043074, func_80043090, func_80043110)
EQUIV_RUNS(func_80041E5C, func_80043118, func_80043270, func_800433A4, func_800435F4, func_80043658)
EQUIV_RUNS(func_80041E5C, func_800436C8, func_800436F4, func_80043750, func_80043764, func_80043778)
EQUIV_RUNS(func_80041E5C, func_8004378C, func_8004379C, func_800437BC, func_800437D8, func_800437F4)
EQUIV_RUNS(func_80041E5C, func_8004400C, func_80044410, func_800444BC, func_800445B8, func_80044F54)
EQUIV_RUNS(func_80041E5C, func_80045218, func_800454A4, func_80045878)
EQUIV_RUNS(func_80041E5C, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_80041E5C, func_80043504, func_8004351C, func_80043554, func_80043594, func_80044178)
EQUIV_RUNS(func_80041E5C, func_8004427C, func_80044334, func_800443EC, func_80044434, func_80044468)
EQUIV_RUNS(func_80041E5C, func_800444E0, func_80044534, func_80044558, func_80044594, func_800446F0)
EQUIV_RUNS(func_80041E5C, func_800447DC, func_80044830)

/* Draws the zone's world polygons in the sort list's order (its last entry first) into the frame's ordering table,
 * through the scene's mode; a list of one entry draws nothing and leaves the table empty */
void func_80041E5C(u16 *list, FrameBuffer *frame, WorldScene *scene, s32 clock) {
    WorldScratch *scratch = WORLD_SCRATCH;
    WorldPass pass;
    WorldRecord polygon, next;
    u16 *first = list + SORT_LIST_FIRST;
    u16 *entry = list + 1 + (s16)list[0];
    s32 word;

    GTE_WRITE_CONTROL(GTE_TRX, 0);
    GTE_WRITE_CONTROL(GTE_TRY, 0);
    GTE_WRITE_CONTROL(GTE_TRZ, 0);
    scratch->clock = clock;
    scratch->first = first;
    if (entry == first) {
        Gfx_ResetOt((u8 *)frame->ot, OT_SLOTS);
        return;
    } else {
        /* Polygons to draw */
    }
    scratch->camera_x = scene->camera[0] >> CAMERA_SHIFT;
    scratch->camera_y = scene->camera[1] >> CAMERA_SHIFT;
    scratch->camera_z = scene->camera[2] >> CAMERA_SHIFT;
    scratch->project_triangle = scene->project_triangle;
    scratch->project_quad = scene->project_quad;
    scratch->draw_triangle = scene->draw_triangle;
    scratch->draw_quad = scene->draw_quad;
    scene->setup(scene);
    for (word = 0; word < (s32)(sizeof(scratch->depth_shifts) / sizeof(u32)); word++) {
        ((u32 *)scratch->depth_shifts)[word] = ((u32 *)D_8005C724)[word];
    }
    pass.next = frame->next;
    pass.ot = frame->ot;
    pass.clock = clock;
    scratch->vertex_mask = VERTEX_MASK;
    scratch->pair_mask = PAIR_MASK;
    scratch->screen_end = STREAK_SCREEN_END;
    pass.lowest = &frame->ot[OT_LAST];
    *pass.lowest = OT_END;
    read_polygon(&next, *entry);
    scratch->drained = 0;
    for (;; entry--) {
        polygon = next;
        if (entry == first) {
            /* The retail code marks the list's end reached before it draws the first entry, and reads it again */
            scratch->drained = (u32)scratch;
            world_polygon(&pass, &polygon, &next, *entry);
            break;
        } else {
            world_polygon(&pass, &polygon, &next, entry[-1]);
        }
    }
    if (pass.lowest != pass.ot) {
        link_down(pass.lowest, pass.ot - 1);
    } else {
        /* Every slot linked */
    }
    frame->next = pass.next;
}
