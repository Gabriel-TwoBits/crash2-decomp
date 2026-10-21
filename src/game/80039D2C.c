/* The handwritten code's equivalent C (docs/WORKFLOW.md, "The handwritten code"). Only the equivalent build compiles
 * it: configure.py leaves each function defined here out of that build's copy of asm/game/80039D2C.s, and links this
 * file after it. The default build assembles the handwritten code as it is. */
#include "common.h"
#include "game.h"
#define GTE_MVMVA
#define GTE_DPCS
#define GTE_NCLIP
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
EQUIV_RUNS(func_8004400C, func_8004380C)
EQUIV_RUNS(func_80044178, func_80042FD0, func_80043074, func_80043090, func_80043110, func_80043118)
EQUIV_RUNS(func_80044178, func_80043270, func_800433A4, func_800435F4, func_80043658, func_800436C8)
EQUIV_RUNS(func_80044178, func_800436F4, func_80043750, func_80043764, func_80043778, func_8004378C)
EQUIV_RUNS(func_80044178, func_8004379C, func_800437BC, func_800437D8, func_800437F4, func_8004399C)
EQUIV_RUNS(func_80044178, func_800439D4, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C)
EQUIV_RUNS(func_80044178, func_80043C48, func_80043D34, func_80043D54, func_80043D84, func_80043EA0)
EQUIV_RUNS(func_80044178, func_8004400C)
EQUIV_RUNS(func_80044178, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_80044178, func_80043504, func_8004351C, func_80043554, func_80043594)
EQUIV_RUNS(func_80044178, func_8004380C)
EQUIV_RUNS(func_8004427C, func_80042FD0, func_80043074, func_80043090, func_80043110, func_80043118)
EQUIV_RUNS(func_8004427C, func_80043270, func_800433A4, func_800435F4, func_80043658, func_800436C8)
EQUIV_RUNS(func_8004427C, func_800436F4, func_80043750, func_80043764, func_80043778, func_8004378C)
EQUIV_RUNS(func_8004427C, func_8004379C, func_800437BC, func_800437D8, func_800437F4, func_8004399C)
EQUIV_RUNS(func_8004427C, func_800439D4, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C)
EQUIV_RUNS(func_8004427C, func_80043C48, func_80043D34, func_80043D54, func_80043D84, func_80043EA0)
EQUIV_RUNS(func_8004427C, func_8004400C, func_80044178)
EQUIV_RUNS(func_8004427C, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_8004427C, func_80043504, func_8004351C, func_80043554, func_80043594)
EQUIV_RUNS(func_8004427C, func_8004380C)
EQUIV_RUNS(func_80044334, func_80042FD0, func_80043074, func_80043090, func_80043110, func_80043118)
EQUIV_RUNS(func_80044334, func_80043270, func_800433A4, func_800435F4, func_80043658, func_800436C8)
EQUIV_RUNS(func_80044334, func_800436F4, func_80043750, func_80043764, func_80043778, func_8004378C)
EQUIV_RUNS(func_80044334, func_8004379C, func_800437BC, func_800437D8, func_800437F4, func_8004399C)
EQUIV_RUNS(func_80044334, func_800439D4, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C)
EQUIV_RUNS(func_80044334, func_80043C48, func_80043D34, func_80043D54, func_80043D84, func_80043EA0)
EQUIV_RUNS(func_80044334, func_8004400C, func_80044178, func_8004427C)
EQUIV_RUNS(func_80044334, func_800432F4, func_80043310, func_80043324, func_80043374, func_800434EC)
EQUIV_RUNS(func_80044334, func_80043504, func_8004351C, func_80043554, func_80043594)
EQUIV_RUNS(func_80044334, func_8004380C)

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
EQUIV_RUNS(func_80041E5C, func_8004380C)

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

/* ---- The object renderers (docs/TARGET.md, "The object renderers") ---- */

/* A draw packet, as func_8001ABF8 builds them in the frame's packet list: the next packet, its three steps (the
 * GTE's rotation and translation for the object, a light or scene setup, the drawer), the object, its matrix (a
 * rotation's five words, then the scale), and what the steps read; a model's packet (kind 1) holds them all, a sprite's
 * (kind 2) to 0x2C, kind 5's to 0x30 */
struct DrawPass;
typedef void (*DrawStep)(struct DrawPass *);

typedef s32 (*DrawTest)(struct DrawPass *);

typedef struct DrawPacket {
    struct DrawPacket *next;
    DrawStep place;
    DrawStep prepare;
    DrawStep draw;
    u8 *object;
    u32 *matrix;
    u32 clock;
    union {
        /* A sprite's record (its colour and code, then a corner's texture words; kind 5's list of them), fade, size,
         * far colour, and kind 5's frame */
        struct {
            u32 *record;
            s16 fade;
            s16 unk_22;
            u32 size_shift;
            u32 far_colour;
            u32 frame;
        } sprite;
        /* A model's visibility test, far colour step, colour routine, and its frame (two to blend) */
        struct {
            DrawTest visible;
            DrawStep far_setup;
            DrawStep shade;
            u32 *frames[2];
        } model;
    } kind;
    u32 *strips;
    u32 *colours;
    u8 *textures;
    u32 *animations;
    s16 model_fade;
    s16 model_fade_high;
    u32 model_far_colour;
    u32 far_depths[2];
    s32 offset_y;
    s32 level;
    u32 colour_shift;
    u32 colour_mask;
    u32 *widths;
} DrawPacket;

/* The registers the retail code keeps across a packet's steps: the packet ($sp), its object ($gp), the frame being
 * built ($fp), the scene ($a2), the object's rotation words (t0 to t4) and the depth the placing step leaves (t7) */
typedef struct DrawPass {
    DrawPacket *packet;
    u8 *object;
    FrameBuffer *frame;
    WorldScene *scene;
    u32 rotation[MATRIX_WORDS];
    s32 depth;
    /* A model's: its strips' commands (s2), colours (s6), textures (s1) and their animations (s0), its vertices'
     * field widths (gp) and stream (s4) with its bit reader (t3, t4), the command and its corners' colours (t0, t5 to
     * t7), the texture's offset (a2), the colour routine (s3) and the frame's next primitive (s7) */
    u32 *strip;
    u32 *colours;
    u8 *textures;
    u32 *animations;
    u32 *widths;
    u8 *stream;
    u32 bits;
    s32 bit_count;
    /* A morphing model's second frame's stream and bits ($s5, $sp), read in step with the first's */
    s32 morph;
    u8 *stream2;
    u32 bits2;
    /* A lit model's vertices turned by the light matrix (the object's rotation) as they are read */
    s32 lit;
    u32 command;
    u32 corner[3];
    u32 texture;
    DrawStep shade;
    u32 *next;
} DrawPass;

/* A model's vertex as the clipping drawers keep it in the scratchpad: its VXY pair and z, its colour, its texture's
 * corner (u and v) */
typedef struct {
    s16 x;
    s16 y;
    s32 z;
    u8 colour[4];
    u8 u;
    u8 v;
    s16 unk_0E;
} DrawVertex;

/* The clipping drawers' records: a triangle's three corners and its two cut points; the GTE's three screen points
 * kept, each a VXY pair and z */
#define DRAW_RECORDS 5
#define SCREEN_WORDS 6

/* The scratchpad as the object renderers keep it (the retail code's words): the fog's far colour, shift and start (the
 * scene's words, the first two the same one), the packet list's next packet, the frame's next primitive, the camera,
 * the packet's clock, the camera's matrix and position, the scene, the object's far depth and its y */
typedef struct {
    u32 saved_sp;
    u32 fog_far;
    s32 fog_shift;
    DrawPacket *packet;
    s32 fog_start;
    u32 *next;
    s32 *camera;
    u32 clock;
    u32 camera_matrix[MATRIX_WORDS];
    s32 camera_position[3];
    u8 unk_40[0x20];
    WorldScene *scene;
    u8 unk_64[0xC];
    u8 *texture_record;
    s32 sides[3];
    DrawVertex vertices[DRAW_RECORDS];
    u32 screen[SCREEN_WORDS];
    u8 unk_E8[4];
    u32 fade;
    u32 far_depth;
    u32 far_depths[2];
    s32 object_y;
    u32 texture_words[3];
    u8 *clipped_record;
    u8 deltas[4];
    u8 deltas2[4];
    u32 box_xy;
    s32 box_z;
    u32 first_command;
    s32 level;
    s32 clip_y;
    u32 colour_shift;
    u32 colour_mask;
    u8 unk_134[0xC];
    u32 copied[1];
} DrawScratch;

#define DRAW_AT(field, offset) typedef char check_draw_##field[((u32)&((DrawScratch *)0)->field == (offset)) ? 1 : -1]
DRAW_AT(fog_far, 0x4);
DRAW_AT(packet, 0xc);
DRAW_AT(clock, 0x1c);
DRAW_AT(camera_matrix, 0x20);
DRAW_AT(camera_position, 0x34);
DRAW_AT(scene, 0x60);
DRAW_AT(far_depth, 0xf0);
DRAW_AT(object_y, 0xfc);
DRAW_AT(texture_record, 0x70);
DRAW_AT(sides, 0x74);
DRAW_AT(vertices, 0x80);
DRAW_AT(screen, 0xd0);
DRAW_AT(fade, 0xec);
DRAW_AT(box_xy, 0x118);
DRAW_AT(deltas, 0x110);
DRAW_AT(clip_y, 0x128);
DRAW_AT(copied, 0x140);
DRAW_AT(level, 0x124);
DRAW_AT(colour_mask, 0x130);

#define PACKET_AT(field, offset) \
    typedef char check_packet_##field[((u32)&((DrawPacket *)0)->field == (offset)) ? 1 : -1]
PACKET_AT(object, 0x10);
PACKET_AT(kind, 0x1c);
PACKET_AT(strips, 0x30);
PACKET_AT(model_fade, 0x40);
PACKET_AT(offset_y, 0x50);
PACKET_AT(far_depths, 0x48);
PACKET_AT(widths, 0x60);

#define DRAW_SCRATCH ((DrawScratch *)SAVE_AREA)

/* An object's fields the placing steps read: its position (24.8) and angles; its depth bias */
#define OBJECT_X 0x60
#define OBJECT_Y 0x64
#define OBJECT_Z 0x68
#define OBJECT_ANGLE_X 0x6C
#define OBJECT_ANGLE_Y 0x70
#define OBJECT_ANGLE_Z 0x74
#define OBJECT_DEPTH_BIAS 0x118
#define OBJECT_FIELD(object, offset) (*(s32 *)((object) + (offset)))
/* The ordering table's slots a packet's depth bias counts from */
#define FAR_DEPTH_BIAS 0x770
#define POSITION_SHIFT 8

/* The lowest word: the retail code's limits are addi instructions, which trap when the value is within their
 * immediate of it; the game's depths and fades are nowhere near */
#define WORD_MIN ((s32)0x80000000)

/* The camera's rotation times the object's position from the camera, into the GTE's translation (func_80043118, after
 * func_80043110's loads): each product's bits 12 to 43; the object's y kept in the scratchpad, and its depth */
static void place_translation(DrawPass *pass, s32 x, s32 y, s32 z) {
    DrawScratch *scratch = DRAW_SCRATCH;
    s16 *camera = (s16 *)scratch->camera_matrix;
    s32 row[3];
    s32 index;

    scratch->object_y = y;
    x = (x >> POSITION_SHIFT) - scratch->camera_position[0];
    y = (y >> POSITION_SHIFT) - scratch->camera_position[1];
    z = (z >> POSITION_SHIFT) - scratch->camera_position[2];
    for (index = 0; index < 3; index++) {
        row[index] = (s32)(((s64)x * camera[index * 3]) >> GTE_FRACTION_BITS)
                     + (s32)(((s64)y * camera[index * 3 + 1]) >> GTE_FRACTION_BITS)
                     + (s32)(((s64)z * camera[index * 3 + 2]) >> GTE_FRACTION_BITS);
    }
    GTE_WRITE_CONTROL(GTE_TRX, row[0]);
    GTE_WRITE_CONTROL(GTE_TRY, row[1]);
    GTE_WRITE_CONTROL(GTE_TRZ, row[2]);
    pass->depth = row[2];
}

static void place_at_object(DrawPass *pass) {
    u8 *object = pass->object;

    place_translation(pass, OBJECT_FIELD(object, OBJECT_X), OBJECT_FIELD(object, OBJECT_Y),
                      OBJECT_FIELD(object, OBJECT_Z));
}

/* The camera's rotation times the object's (func_80042FA8): the rotation words the GTE then holds */
static void camera_times_object(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;

    rotation_load(scratch->camera_matrix);
    rotation_multiply(pass->rotation);
}

/* The object's rotation with its second row scaled by -0.625 (0xA0 / 256) and its third negated, for the screen's
 * axes (func_80043090): the first row's words as they were */
#define SCREEN_ROW_SCALE 0xA0
#define SCREEN_ROW_SHIFT 8

static s32 screen_scaled(s32 half) {
    return -((SCREEN_ROW_SCALE * half) >> SCREEN_ROW_SHIFT);
}

static void screen_rotation(DrawPass *pass) {
    u32 *words = pass->rotation;
    u32 rt13_21 = words[1];
    u32 rt22_23 = words[2];
    u32 rt31_32 = words[3];

    /* The retail code's negations are sub instructions, which trap on the lowest word: RT31 shifted up and RT33's word,
     * which the game's rotations (within 1.0) never make */
    EQUIV_ASSUME((s32)(rt31_32 << GTE_HALF_BITS) != WORD_MIN && (s32)words[4] != WORD_MIN);
    words[1] = (rt13_21 & GTE_HALF_MASK) | ((u32)screen_scaled((s32)rt13_21 >> GTE_HALF_BITS) << GTE_HALF_BITS);
    words[2] = ((u32)screen_scaled((s16)rt22_23) & GTE_HALF_MASK)
               | ((u32)screen_scaled((s32)rt22_23 >> GTE_HALF_BITS) << GTE_HALF_BITS);
    words[3] = (((u32)-(rt31_32 << GTE_HALF_BITS)) >> GTE_HALF_BITS)
               | ((u32)-((s32)rt31_32 >> GTE_HALF_BITS) << GTE_HALF_BITS);
    words[4] = -words[4];
    GTE_WRITE_CONTROL(GTE_RT13RT21, words[1]);
    GTE_WRITE_CONTROL(GTE_RT22RT23, words[2]);
    GTE_WRITE_CONTROL(GTE_RT31RT32, words[3]);
    GTE_WRITE_CONTROL(GTE_RT33, words[4]);
}

/* The translation of an object drawn on the screen's plane: its x and y (24.8, y down) and the projection's distance */
static void place_on_screen(DrawPass *pass) {
    u8 *object = pass->object;
    s32 x = OBJECT_FIELD(object, OBJECT_X) >> POSITION_SHIFT;
    s32 y = -(OBJECT_FIELD(object, OBJECT_Y) >> POSITION_SHIFT);
    s32 distance;

    GTE_READ_CONTROL(GTE_H, distance);
    GTE_WRITE_CONTROL(GTE_TRX, x);
    GTE_WRITE_CONTROL(GTE_TRY, y);
    GTE_WRITE_CONTROL(GTE_TRZ, distance);
    pass->depth = distance;
}

/* A fade by depth past the scene's fog start, shifted by its fog shift and added to the packet's, at most 1.0 */
static s32 faded(s32 fade, s32 depth_past) {
    DrawScratch *scratch = DRAW_SCRATCH;
    s32 value = (depth_past << (scratch->fog_shift & SHIFT_COUNT_MASK)) + fade;

    EQUIV_ASSUME(value >= WORD_MIN + FIXED_ONE);
    if (value - FIXED_ONE > 0) {
        return FIXED_ONE;
    } else {
        return value;
    }
}

static s32 depth_past_fog(void) {
    s32 depth;

    GTE_READ_CONTROL(GTE_TRZ, depth);
    return depth - DRAW_SCRATCH->fog_start;
}

/* The packets' placing steps (the table at 0x8005B690, func_8001AA80): the GTE's rotation and translation */

/* func_8001AA80 picks an object's first step by its kind, as func_8001ABF8 its packet's drawer: a sprite's steps (its
 * fade at 0x20) go with the sprites' drawers, a model's (its fade at 0x40, its vectors from 0x50) with the models';
 * the trials' rows pair them freely, holding the drawers' retail addresses (only the trials compare them) */
#define SPRITE_DRAWER_ADDRESS 0x80044830
#define SPRITE_LIST_DRAWER_ADDRESS 0x8004380C
#define SPRITE_DRAWN(packet) \
    ((u32)(packet)->draw == SPRITE_DRAWER_ADDRESS || (u32)(packet)->draw == SPRITE_LIST_DRAWER_ADDRESS)

/* In the camera's view */
void func_80044410(DrawPass *pass) {
    camera_times_object(pass);
    place_at_object(pass);
}

/* Raised by the packet's offset */
void func_80044434(DrawPass *pass) {
    u8 *object = pass->object;

    EQUIV_ASSUME(!SPRITE_DRAWN(pass->packet));
    camera_times_object(pass);
    place_translation(pass, OBJECT_FIELD(object, OBJECT_X), OBJECT_FIELD(object, OBJECT_Y) + pass->packet->offset_y,
                      OBJECT_FIELD(object, OBJECT_Z));
}

/* In the camera's view, fading past the fog's start toward its far colour */
void func_80044468(DrawPass *pass) {
    DrawPacket *packet = pass->packet;
    s32 past;
    s32 fade;

    EQUIV_ASSUME(!SPRITE_DRAWN(pass->packet));
    func_80044410(pass);
    past = depth_past_fog();
    if (past >= 0) {
        fade = faded(packet->model_fade, past);
        packet->model_far_colour = DRAW_SCRATCH->fog_far;
        packet->model_fade = fade;
    } else {
        /* Nearer than the fog */
    }
}

/* Facing the screen */
void func_800443EC(DrawPass *pass) {
    screen_rotation(pass);
    place_at_object(pass);
}

void func_800444BC(DrawPass *pass) {
    screen_rotation(pass);
    place_at_object(pass);
}

/* Facing the screen, a sprite fading past the fog's start */
void func_800444E0(DrawPass *pass) {
    DrawPacket *packet = pass->packet;
    s32 past;
    s32 fade;

    EQUIV_ASSUME(SPRITE_DRAWN(pass->packet));
    func_800444BC(pass);
    past = depth_past_fog();
    if (past >= 0) {
        fade = faded(packet->kind.sprite.fade, past);
        packet->kind.sprite.far_colour = DRAW_SCRATCH->fog_far;
        packet->kind.sprite.fade = fade;
    } else {
        /* Nearer than the fog */
    }
}

void func_80044534(DrawPass *pass) {
    camera_times_object(pass);
    place_at_object(pass);
}

/* On the screen's plane */
void func_80044558(DrawPass *pass) {
    screen_rotation(pass);
    place_on_screen(pass);
}

void func_80044594(DrawPass *pass) {
    screen_rotation(pass);
    place_at_object(pass);
}

/* The GTE's light matrix's first and third rows from the GTE's rotation times the packet's two light vectors (each a
 * VXY pair and z); its second row's words as they were but L21, which takes the first product's z's high half */
static void light_rows(DrawPacket *packet) {
    u32 *vectors = (u32 *)&packet->offset_y;
    s32 x, y, z;

    GTE_LOAD_DATA(GTE_VXY0, vectors[0]);
    GTE_LOAD_DATA(GTE_VZ0, vectors[1]);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, 0);
    GTE_READ_DATA(GTE_IR1, x);
    GTE_READ_DATA(GTE_IR2, y);
    GTE_READ_DATA(GTE_IR3, z);
    GTE_WRITE_CONTROL(GTE_L11L12, ((u32)x & GTE_HALF_MASK) | ((u32)y << GTE_HALF_BITS));
    GTE_WRITE_CONTROL(GTE_L13L21, z);
    GTE_LOAD_DATA(GTE_VXY0, vectors[2]);
    GTE_LOAD_DATA(GTE_VZ0, vectors[3]);
    Gte_MultiplyMatrix(GTE_MATRIX_ROTATION, 0);
    GTE_READ_DATA(GTE_IR1, x);
    GTE_READ_DATA(GTE_IR2, y);
    GTE_READ_DATA(GTE_IR3, z);
    GTE_WRITE_CONTROL(GTE_L31L32, ((u32)x & GTE_HALF_MASK) | ((u32)y << GTE_HALF_BITS));
    GTE_WRITE_CONTROL(GTE_L33, z);
}

/* The packet's matrix's rotation into the GTE and the pass, as the root first loaded it */
static void reload_rotation(DrawPass *pass) {
    u32 *matrix = pass->packet->matrix;
    s32 word;

    for (word = 0; word < MATRIX_WORDS; word++) {
        pass->rotation[word] = matrix[word];
    }
    rotation_load(pass->rotation);
}

/* Lit: the light's rows turned by the object's angles (y, x, then y again), then in the camera's view */
void func_800445B8(DrawPass *pass) {
    u8 *object = pass->object;
    u32 words[MATRIX_WORDS];
    s32 z = OBJECT_FIELD(object, OBJECT_ANGLE_Z);
    s32 y = OBJECT_FIELD(object, OBJECT_ANGLE_Y);
    s32 x = OBJECT_FIELD(object, OBJECT_ANGLE_X);

    EQUIV_ASSUME(!SPRITE_DRAWN(pass->packet));
    rotation_y(words, -z);
    rotation_load(words);
    if (x != 0) {
        rotation_x(words, -x);
        rotation_multiply(words);
    } else {
        /* No turn about x */
    }
    if (z - y != 0) {
        rotation_y(words, z - y);
        rotation_multiply(words);
    } else {
        /* No further turn about y */
    }
    light_rows(pass->packet);
    reload_rotation(pass);
    camera_times_object(pass);
    place_at_object(pass);
}

/* Lit and on the screen's plane: the light's rows turned about y alone */
void func_800446F0(DrawPass *pass) {
    u32 words[MATRIX_WORDS];

    EQUIV_ASSUME(!SPRITE_DRAWN(pass->packet));
    rotation_y(words, -OBJECT_FIELD(pass->object, OBJECT_ANGLE_Y));
    rotation_load(words);
    light_rows(pass->packet);
    reload_rotation(pass);
    screen_rotation(pass);
    place_on_screen(pass);
}

/* Lit, fading past the fog's start (its far colour left as the packet has it) */
void func_800447DC(DrawPass *pass) {
    DrawPacket *packet = pass->packet;
    s32 past;

    EQUIV_ASSUME(!SPRITE_DRAWN(pass->packet));
    func_800445B8(pass);
    past = depth_past_fog();
    if (past >= 0) {
        packet->model_fade = faded(packet->model_fade, past);
    } else {
        /* Nearer than the fog */
    }
}

/* The packets' preparing steps (D_8005B75C): the level and the colours' shift and mask the drawers read */
void func_800432F4(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    s32 level = packet->level;
    u32 shift = packet->colour_shift;
    u32 mask = packet->colour_mask;

    scratch->level = level;
    scratch->colour_shift = shift;
    scratch->colour_mask = mask;
}

void func_80043310(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    u32 shift = packet->colour_shift;
    s32 level = packet->level;

    scratch->colour_shift = shift;
    scratch->level = level;
}

/* The level at which models are clipped: the packet's offset word, its low bit cleared, as a VXY pair's y */
void func_800434EC(DrawPass *pass) {
    DRAW_SCRATCH->sides[0] = ((u32)pass->packet->offset_y & ~1 & GTE_HALF_MASK) << GTE_HALF_BITS;
}

/* How much the scene's two lamps light the object (func_800433A4): 1.0 less each lamp's light, which falls from
 * LAMP_REACH by the distance along each axis (and LAMP_FALLOFF past the first two), none at 0; the far colour black
 * and the preparing words as func_800432F4 has them */
#define LAMP_REACH 0x2260
#define LAMP_FALLOFF 0x320
#define PLAYER_FADE_MAX 0xC00

extern s32 g_Player;

static s32 magnitude_of(s32 value) {
    if (value < 0) {
        return -value;
    } else {
        return value;
    }
}

static s32 lamp_left(u32 pair, s32 z, u32 *lamp) {
    u32 difference;
    s32 left = LAMP_REACH - magnitude_of(z - (s32)lamp[1]);

    difference = pair - lamp[0];
    if (left < 0) {
        return 0;
    } else {
        /* Within reach along z */
    }
    left -= magnitude_of((s32)difference >> GTE_HALF_BITS);
    if (left < 0) {
        return 0;
    } else {
        /* Within reach along y */
    }
    left -= LAMP_FALLOFF + magnitude_of((s32)(difference << GTE_HALF_BITS) >> GTE_HALF_BITS);
    if (left < 0) {
        return 0;
    } else {
        return left;
    }
}

static s32 lamps_fade(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    u8 *object = pass->object;
    u32 *lamps = (u32 *)pass->scene->lamps;
    s32 x, y, z, fade;
    u32 pair;

    func_800432F4(pass);
    scratch->sides[0] = ((u32)packet->offset_y & ~1 & GTE_HALF_MASK) << GTE_HALF_BITS;
    GTE_WRITE_CONTROL(GTE_RFC, 0);
    GTE_WRITE_CONTROL(GTE_GFC, 0);
    GTE_WRITE_CONTROL(GTE_BFC, 0);
    x = (OBJECT_FIELD(object, OBJECT_X) >> POSITION_SHIFT) - scratch->camera_position[0];
    y = (OBJECT_FIELD(object, OBJECT_Y) >> POSITION_SHIFT) - scratch->camera_position[1];
    z = (OBJECT_FIELD(object, OBJECT_Z) >> POSITION_SHIFT) - scratch->camera_position[2];
    pair = ((u32)y << GTE_HALF_BITS) | ((u32)x & GTE_HALF_MASK);
    fade = FIXED_ONE - (lamp_left(pair, z, lamps) + lamp_left(pair, z, lamps + 2));
    if (fade < 0) {
        return 0;
    } else {
        return fade;
    }
}

/* A fade added to the packet's, at most 1.0 */
static s32 added_fade(s32 fade, s32 packet_fade) {
    fade += packet_fade;
    if (fade - FIXED_ONE > 0) {
        return FIXED_ONE;
    } else {
        return fade;
    }
}

/* Lit by the lamps, the player at most PLAYER_FADE_MAX dark */
void func_80043324(DrawPass *pass) {
    DrawPacket *packet = pass->packet;
    s32 fade;

    packet->model_far_colour = 0;
    fade = added_fade(lamps_fade(pass), packet->model_fade);
    if ((s32)pass->object == g_Player && fade - PLAYER_FADE_MAX > 0) {
        fade = PLAYER_FADE_MAX;
    } else {
        /* Another object, or light enough */
    }
    packet->model_fade = fade;
}

/* A sprite lit by the lamps */
void func_80043374(DrawPass *pass) {
    DrawPacket *packet = pass->packet;

    packet->kind.sprite.far_colour = 0;
    packet->kind.sprite.fade = added_fade(lamps_fade(pass), packet->kind.sprite.fade);
}

/* A far colour word's components as the GTE takes them (4 fraction bits) */
#define FAR_COLOUR_SHIFT_RED 16
#define FAR_COLOUR_SHIFT_GREEN 8
#define FAR_COLOUR_MASK 0xFF0

/* The drawers' far colour steps (the packet's word 0x20, D_8005B6D0): the far colour the colour routines fade toward
 * (DPCT) and the fade they read in the scratchpad; RGBC cleared */
#define FADE_CLOCK_SHIFT 7
#define FADE_CLOCK_MASK 0xFFF
#define FADE_CLOCK_HALF 0x800

static void far_colour(u32 colour) {
    GTE_WRITE_CONTROL(GTE_RFC, (colour >> FAR_COLOUR_SHIFT_RED) & FAR_COLOUR_MASK);
    GTE_WRITE_CONTROL(GTE_GFC, (colour >> FAR_COLOUR_SHIFT_GREEN) & FAR_COLOUR_MASK);
    GTE_WRITE_CONTROL(GTE_BFC, colour & FAR_COLOUR_MASK);
}

/* None: black, unfaded */
void func_80043504(DrawPass *pass) {
    GTE_WRITE_CONTROL(GTE_RFC, 0);
    GTE_WRITE_CONTROL(GTE_GFC, 0);
    GTE_WRITE_CONTROL(GTE_BFC, 0);
    DRAW_SCRATCH->fade = 0;
}

/* The packet's */
void func_8004351C(DrawPass *pass) {
    DrawPacket *packet = pass->packet;
    u32 fade = *(u32 *)&packet->model_fade;
    u32 colour = packet->model_far_colour;

    DRAW_SCRATCH->fade = fade;
    GTE_WRITE_DATA(GTE_RGBC, 0);
    far_colour(colour);
}

/* The packet's, when it fades */
void func_80043554(DrawPass *pass) {
    DrawPacket *packet = pass->packet;
    u32 fade = *(u32 *)&packet->model_fade;
    u32 colour = packet->model_far_colour;

    DRAW_SCRATCH->fade = fade;
    GTE_WRITE_DATA(GTE_RGBC, 0);
    if (fade & GTE_HALF_MASK) {
        far_colour(colour);
    } else {
        /* Unfaded */
    }
}

/* The packet's, its fade a triangle wave of the clock */
void func_80043594(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    s32 wave = ((scratch->clock << FADE_CLOCK_SHIFT) & FADE_CLOCK_MASK) - FADE_CLOCK_HALF;
    u32 fade = *(u32 *)&packet->model_fade;
    u32 colour = packet->model_far_colour;

    if (wave > 0) {
        wave = FIXED_ONE - wave;
    } else {
        /* The wave's rising half */
    }
    scratch->fade = fade;
    *(s16 *)&scratch->fade = wave;
    GTE_WRITE_DATA(GTE_RGBC, 0);
    if (fade & GTE_HALF_MASK) {
        far_colour(colour);
    } else {
        /* Unfaded */
    }
}

/* The drawers' visibility tests (the packet's word 0x1C): whether to draw the model, 0 to draw */
#define DRAW_IT 0
#define SKIP_IT (-1)
/* The model's frame's box: its corner (a VXY pair and z, scratchpad 0x118) and its size, 0x3FC along each axis; the
 * screen it must reach */
#define BOX_SIZE 0x3FC
#define BOX_SIZE_Y (BOX_SIZE << GTE_HALF_BITS)
#define PAIR_X_CARRY_MASK 0xFFFEFFFF
#define SCREEN_HEIGHT 0xD8
#define SCREEN_WIDTH 0x200

/* Whether a projected corner is on the screen (func_80041E20) */
static s32 corner_on_screen(u32 xy, s32 z) {
    if ((s32)xy < 0 || (s32)(xy << GTE_HALF_BITS) < 0 || z < 0) {
        return 0;
    } else if ((s32)(xy >> GTE_HALF_BITS) - SCREEN_HEIGHT >= 0 || (s32)(xy & GTE_HALF_MASK) - SCREEN_WIDTH >= 0) {
        return 0;
    } else {
        return 1;
    }
}

/* SXY0 to SXY2 from FIRST on, against SZ1 to SZ3 */
static s32 corners_on_screen(s32 first) {
    u32 xy;
    s32 z;

    if (first <= 0) {
        GTE_READ_DATA(GTE_SXY0, xy);
        GTE_READ_DATA(GTE_SZ1, z);
        if (corner_on_screen(xy, z)) {
            return 1;
        } else {
            /* The next corner */
        }
    } else {
        /* Not this one */
    }
    GTE_READ_DATA(GTE_SXY1, xy);
    GTE_READ_DATA(GTE_SZ2, z);
    if (corner_on_screen(xy, z)) {
        return 1;
    } else {
        GTE_READ_DATA(GTE_SXY2, xy);
        GTE_READ_DATA(GTE_SZ3, z);
        return corner_on_screen(xy, z);
    }
}

/* Drawn when a corner of the frame's box is on the screen: the corners projected three at a time, the first on
 * screen ending the test */
s32 func_80041D14(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 corner = scratch->box_xy;
    s32 z = scratch->box_z;
    u32 along_x = (corner + BOX_SIZE) & PAIR_X_CARRY_MASK;

    GTE_WRITE_DATA(GTE_VXY0, corner);
    GTE_WRITE_DATA(GTE_VZ0, z);
    GTE_WRITE_DATA(GTE_VXY1, along_x);
    GTE_WRITE_DATA(GTE_VZ1, z);
    GTE_WRITE_DATA(GTE_VXY2, along_x + BOX_SIZE_Y);
    GTE_WRITE_DATA(GTE_VZ2, z);
    Gte_Rtpt();
    if (corners_on_screen(0)) {
        return DRAW_IT;
    } else {
        /* The far face */
    }
    GTE_WRITE_DATA(GTE_VXY0, corner + BOX_SIZE_Y);
    GTE_WRITE_DATA(GTE_VXY1, corner + BOX_SIZE_Y);
    GTE_WRITE_DATA(GTE_VZ1, z + BOX_SIZE);
    GTE_WRITE_DATA(GTE_VXY2, corner);
    GTE_WRITE_DATA(GTE_VZ2, z + BOX_SIZE);
    Gte_Rtpt();
    if (corners_on_screen(0)) {
        return DRAW_IT;
    } else {
        /* The last two */
    }
    GTE_WRITE_DATA(GTE_VXY1, along_x);
    GTE_WRITE_DATA(GTE_VXY2, along_x + BOX_SIZE_Y);
    Gte_Rtpt();
    if (corners_on_screen(1)) {
        return DRAW_IT;
    } else {
        return SKIP_IT;
    }
}

/* Always drawn */
s32 func_80041E50(DrawPass *pass) {
    return DRAW_IT;
}

/* A primitive's ordering table slot: the object's far depth less its corners' depths' sum / 32, kept to the table */
#define DEPTH_SUM_SHIFT 5
#define OT_LAST_OFFSET 0x1FFC
#define PRIMITIVE_WORDS_SHIFT 24

static u32 *depth_slot_of_depth(DrawPass *pass, s32 depth) {
    s32 offset = (u32)depth * sizeof(u32);

    if (depth < 0) {
        offset = 0;
    } else {
        /* In front of the table's first slot */
    }
    EQUIV_ASSUME(offset >= WORD_MIN + OT_LAST_OFFSET);
    if (offset - OT_LAST_OFFSET > 0) {
        offset = OT_LAST_OFFSET;
    } else {
        /* Within the table */
    }
    return (u32 *)((u8 *)pass->frame->ot + offset);
}

static u32 *depth_slot_of(DrawPass *pass, u32 depth_sum) {
    return depth_slot_of_depth(pass, DRAW_SCRATCH->far_depth - (depth_sum >> DEPTH_SUM_SHIFT));
}

/* A sprite (kind 2): a textured quad of the packet's record's four words (colour and code, then a corner's texture
 * words), its corners 200 << the packet's size shift about the object's centre, faded toward the packet's far colour
 * by its fade (DPCS), linked into its depth's slot; none behind the camera */
#define SPRITE_HALF 0xC8
#define SPRITE_WORDS 10
#define POLY_FT4_TAG_LENGTH 0x09000000
#define CLUT_SHIFT 16

/* A sprite's quad after the GTE's projection of its first three corners (RTPT): its record's texture words, the
 * fourth corner projected (RTPS), linked into its depth's slot, its colour (the record's first word) faded toward the
 * packet's far colour by its fade (DPCS) */
static u32 *sprite_quad(DrawPass *pass, u32 *record, u32 fourth, u32 *primitive) {
    DrawPacket *packet = pass->packet;
    u32 *slot;
    u32 uv0, uv1, uv2, colour, depth_sum, depth, fade;

    uv0 = record[1];
    uv1 = record[2];
    uv2 = record[3];
    GTE_STORE_DATA(GTE_SXY0, primitive[2]);
    GTE_STORE_DATA(GTE_SXY1, primitive[4]);
    GTE_STORE_DATA(GTE_SXY2, primitive[6]);
    GTE_READ_DATA(GTE_SZ1, depth_sum);
    GTE_READ_DATA(GTE_SZ2, depth);
    depth_sum += depth;
    GTE_READ_DATA(GTE_SZ3, depth);
    depth_sum += depth;
    GTE_WRITE_DATA(GTE_VXY0, fourth);
    Gte_Rtps();
    primitive[3] = uv0;
    slot = depth_slot_of(pass, depth_sum);
    primitive[0] = POLY_FT4_TAG_LENGTH | *slot;
    primitive[5] = uv1;
    *slot = (u32)primitive & PRIM_ADDRESS_MASK;
    primitive[7] = uv2;
    primitive[9] = uv2 >> CLUT_SHIFT;
    fade = packet->kind.sprite.fade & GTE_HALF_MASK;
    GTE_STORE_DATA(GTE_SXY2, primitive[8]);
    colour = record[0];
    if (fade != 0) {
        GTE_WRITE_DATA(GTE_RGBC, colour);
        GTE_WRITE_DATA(GTE_IR0, fade);
        far_colour(packet->kind.sprite.far_colour);
        Gte_Dpcs();
        GTE_READ_DATA(GTE_RGB2, colour);
    } else {
        /* Unfaded */
    }
    primitive[1] = colour;
    return primitive + SPRITE_WORDS;
}

void func_80044830(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    u32 near, far;
    s32 shift;

    GTE_WRITE_DATA(GTE_VZ0, 0);
    if (pass->depth <= 0) {
        return;
    } else {
        /* In front of the camera */
    }
    GTE_WRITE_DATA(GTE_VZ1, 0);
    GTE_WRITE_DATA(GTE_VZ2, 0);
    shift = packet->kind.sprite.size_shift & SHIFT_COUNT_MASK;
    far = SPRITE_HALF << shift;
    near = (u32)(-SPRITE_HALF << shift) & GTE_HALF_MASK;
    GTE_WRITE_DATA(GTE_VXY0, (far << GTE_HALF_BITS) | near);
    GTE_WRITE_DATA(GTE_VXY1, (far << GTE_HALF_BITS) | far);
    GTE_WRITE_DATA(GTE_VXY2, (near << GTE_HALF_BITS) | near);
    Gte_Rtpt();
    scratch->next = sprite_quad(pass, packet->kind.sprite.record, (near << GTE_HALF_BITS) | far, scratch->next);
}

/* Kind 5's sprites (func_8004380C): the packet's frame's records (0x18 bytes, after the count at word 2 and three
 * words), each its own corners' halves (x0 y0 x1 y1 from 0x10); at least one */
#define SPRITE_RECORD_WORDS 6
#define SPRITE_LIST_COUNT 2
#define SPRITE_LIST_HEADER_WORDS 3
#define SPRITE_CORNERS 4

void func_8004380C(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    u32 *list;
    u32 *record;
    u32 *primitive;
    u16 *corners;
    u32 x0, y0, x1, y1;
    s32 count;

    GTE_WRITE_DATA(GTE_VZ0, 0);
    if (pass->depth <= 0) {
        return;
    } else {
        list = packet->kind.sprite.record;
    }
    GTE_WRITE_DATA(GTE_VZ1, 0);
    count = list[SPRITE_LIST_COUNT];
    GTE_WRITE_DATA(GTE_VZ2, 0);
    primitive = scratch->next;
    record = list + SPRITE_LIST_HEADER_WORDS
             + (packet->kind.sprite.frame * (SPRITE_RECORD_WORDS * sizeof(u32)) * count) / sizeof(u32);
    do {
        corners = (u16 *)(record + SPRITE_CORNERS);
        x0 = corners[0];
        y0 = (u32)corners[1] << GTE_HALF_BITS;
        x1 = corners[2];
        y1 = (u32)corners[3] << GTE_HALF_BITS;
        GTE_WRITE_DATA(GTE_VXY0, y1 | x0);
        GTE_WRITE_DATA(GTE_VXY1, x1 | y1);
        GTE_WRITE_DATA(GTE_VXY2, x0 | y0);
        Gte_Rtpt();
        primitive = sprite_quad(pass, record, x1 | y0, primitive);
        record += SPRITE_RECORD_WORDS;
    } while (--count > 0);
    scratch->next = primitive;
}

/* Sys_MainLoop passes the frame's packet list, the frame and D_80062A18. The retail code keeps its caller's $sp and
 * its steps' return addresses in the scratchpad, which the C doesn't */
#define DRAW_SAVED_SP_BYTES 4
#define DRAW_SAVED_RA 0x1F800064
#define DRAW_SAVED_RA_BYTES 8
#define DRAW_PACKET_BYTES 0x64
EQUIV_SCRATCH(func_8003DB94, SAVE_AREA, DRAW_SAVED_SP_BYTES)
EQUIV_SCRATCH(func_8003DB94, DRAW_SAVED_RA, DRAW_SAVED_RA_BYTES)
/* func_8003E118 and func_8003E128 keep their triangles' routine in word 0x130, the C's at its own address */
#define DRAW_SAVED_TRIANGLE 0x1F800130
#define DRAW_SAVED_TRIANGLE_BYTES 4
EQUIV_SCRATCH(func_8003DB94, DRAW_SAVED_TRIANGLE, DRAW_SAVED_TRIANGLE_BYTES)
EQUIV_NONNULL(func_8003DB94, 0)
EQUIV_NONNULL(func_8003DB94, 1)
EQUIV_NONNULL(func_8003DB94, 2)
EQUIV_BUFFER(func_8003DB94, 0, DRAW_PACKET_BYTES)
EQUIV_BUFFER(func_8003DB94, 1, FRAME_BYTES)
EQUIV_BUFFER(func_8003DB94, 2, WORLD_SCENE_BYTES)
/* A packet's words as func_8001ABF8 builds them: the last packet (one a trial), its first step (D_8005B690), its
 * second and drawer by mode (D_8005B75C and D_8005B708 by the same index), a model's visibility test, and its far
 * colour step and colour routine by sub-kind (D_8005B6D0 and D_8005B6EC); a sprite's words there are its own */
EQUIV_WORDS(func_8003DB94, 0, 0, 0)
EQUIV_WORDS(func_8003DB94, 0, 4, func_800443EC)
EQUIV_WORDS(func_8003DB94, 0, 4, func_80044410)
EQUIV_WORDS(func_8003DB94, 0, 4, func_80044434)
EQUIV_WORDS(func_8003DB94, 0, 4, func_80044468)
EQUIV_WORDS(func_8003DB94, 0, 4, func_800444BC)
EQUIV_WORDS(func_8003DB94, 0, 4, func_800444E0)
EQUIV_WORDS(func_8003DB94, 0, 4, func_80044534)
EQUIV_WORDS(func_8003DB94, 0, 4, func_80044558)
EQUIV_WORDS(func_8003DB94, 0, 4, func_80044594)
EQUIV_WORDS(func_8003DB94, 0, 4, func_800445B8)
EQUIV_WORDS(func_8003DB94, 0, 4, func_800446F0)
EQUIV_WORDS(func_8003DB94, 0, 4, func_800447DC)
EQUIV_WORDS(func_8003DB94, 0, 8, func_8003DD0C, func_8003E360)
EQUIV_WORDS(func_8003DB94, 0, 8, func_8003DD0C, func_8003F410)
EQUIV_WORDS(func_8003DB94, 0, 8, func_8003DD0C, func_80044830)
EQUIV_WORDS(func_8003DB94, 0, 8, func_8003DD0C, func_8004380C)
EQUIV_WORDS(func_8003DB94, 0, 8, func_800432F4, func_8003FF14)
EQUIV_WORDS(func_8003DB94, 0, 8, func_800432F4, func_80040FC4)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043310, func_8003E128)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043310, func_8003F1D8)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043324, func_8003E360)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043324, func_8003F410)
EQUIV_WORDS(func_8003DB94, 0, 8, func_800434EC, func_80040168)
EQUIV_WORDS(func_8003DB94, 0, 8, func_800434EC, func_800417B0)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043324, func_80040168)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043324, func_800417B0)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043324, func_8003FF14)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043324, func_80040FC4)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043374, func_80044830)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043310, func_8003E118)
EQUIV_WORDS(func_8003DB94, 0, 8, func_80043310, func_8003F1C8)
EQUIV_WORDS(func_8003DB94, 0, 0x1C, func_80041D14)
EQUIV_WORDS(func_8003DB94, 0, 0x1C, func_80041E50)
EQUIV_WORDS(func_8003DB94, 0, 0x20, func_80043554, func_8003EBC4)
EQUIV_WORDS(func_8003DB94, 0, 0x20, func_80043504, func_80040B44)
EQUIV_WORDS(func_8003DB94, 0, 0x20, func_80043554, func_8003FB84)
EQUIV_WORDS(func_8003DB94, 0, 0x20, func_80043554, func_80041B80)
EQUIV_WORDS(func_8003DB94, 0, 0x20, func_8004351C, func_8004075C)
EQUIV_WORDS(func_8003DB94, 0, 0x20, func_8004351C, func_80040948)
EQUIV_WORDS(func_8003DB94, 0, 0x20, func_80043594, func_8003EBC4)
HANDWRITTEN_PARTS(func_8003DB94, func_8003DD14, func_8003DED8, func_8003E118, func_8003E128, func_8003E360)
HANDWRITTEN_PARTS(func_8003DB94, func_8003E4FC, func_8003E8DC, func_8003E940, func_8003EA1C, func_8003EBC4)
HANDWRITTEN_PARTS(func_8003DB94, func_8003ED14, func_8003F1C8, func_8003F1D8, func_8003F410, func_8003F59C)
HANDWRITTEN_PARTS(func_8003DB94, func_8003F988, func_8003F9EC, func_8003FB24, func_8003FB84, func_8003FD14)
HANDWRITTEN_PARTS(func_8003DB94, func_8003FF14, func_80040168, func_8004075C, func_80040948, func_80040B44)
HANDWRITTEN_PARTS(func_8003DB94, func_80040D14, func_80040FC4, func_80041218, func_80041600, func_8004164C)
HANDWRITTEN_PARTS(func_8003DB94, func_80041784, func_800417B0, func_80041B80, func_80041D14, func_80041E20)
HANDWRITTEN_PARTS(func_8003DB94, func_80041E50, func_80042FA8, func_80042FD0, func_80043074, func_80043090)
HANDWRITTEN_PARTS(func_8003DB94, func_80043110, func_80043118, func_80043270, func_800432F4, func_80043310)
HANDWRITTEN_PARTS(func_8003DB94, func_80043324, func_80043374, func_800433A4, func_800434EC, func_80043504)
HANDWRITTEN_PARTS(func_8003DB94, func_8004351C, func_80043554, func_80043594, func_800435F4, func_80043658)
HANDWRITTEN_PARTS(func_8003DB94, func_800436C8, func_800436F4, func_80043750, func_80043764, func_80043778)
HANDWRITTEN_PARTS(func_8003DB94, func_8004378C, func_8004379C, func_800437BC, func_800437D8, func_800437F4)
HANDWRITTEN_PARTS(func_8003DB94, func_8004380C, func_800443EC, func_80044410, func_80044434, func_80044468)
HANDWRITTEN_PARTS(func_8003DB94, func_800444BC, func_800444E0, func_80044534, func_80044558, func_80044594)
HANDWRITTEN_PARTS(func_8003DB94, func_800445B8, func_800446F0, func_800447DC, func_80044830, func_800454A4)
HANDWRITTEN_PARTS(func_8003DB94, func_80045878)
HANDWRITTEN_PARTS(func_8003DB94, func_8003DD0C)
EQUIV_RUNS(func_8003DB94, func_8003EAD4, func_80041A78, func_80041E5C, func_80042420, func_80042548)
EQUIV_RUNS(func_8003DB94, func_80042698, func_800426A0, func_80042818, func_8004299C, func_800429D0)
EQUIV_RUNS(func_8003DB94, func_80042AB8, func_80042B44, func_80042B7C, func_80042C2C, func_80042C70)
EQUIV_RUNS(func_8003DB94, func_80042D10, func_80042E84, func_80042F08, func_8004399C, func_800439D4)
EQUIV_RUNS(func_8003DB94, func_80043A84, func_80043B5C, func_80043B74, func_80043C2C, func_80043C48)
EQUIV_RUNS(func_8003DB94, func_80043D34, func_80043D54, func_80043D84, func_80043EA0, func_8004400C)
EQUIV_RUNS(func_8003DB94, func_80044178, func_8004427C, func_80044334, func_80044980, func_800449B0)
EQUIV_RUNS(func_8003DB94, func_80044A04, func_80044A70, func_80044BB4, func_80044E48, func_80044EC0)
EQUIV_RUNS(func_8003DB94, func_80044F54, func_80045218)

/* ---- Models (kind 1): triangle strips of a frame's vertices ---- */

/* A strip command: its first is negative (-1 ends the model), a vertex's command has bits 16 to 31 not all clear (bit
 * 30: the triangle turns about the strip's first vertex instead of moving on), a colour command has them clear (its
 * second and third corners' colours, bits 7 and 0 choosing the scratchpad's copy). A vertex's command: bit 26 takes it
 * from the scratchpad's cache (its index in bits 16 to 23), bit 27 its colour from the copy (index in bits 7 to 13),
 * bits 0 to 8 the triangle's texture (bit 8 an animation's index) */
#define STRIP_END ((u32)-1)
#define STRIP_FAN (1 << 30)
#define COLOUR_COMMAND_SHIFT 16
#define COLOUR_SECOND_FROM_COPY 1
#define COLOUR_THIRD_FROM_COPY 2
#define VERTEX_CACHED (1 << 26)
#define VERTEX_COLOUR_FROM_COPY (1 << 27)
#define VERTEX_CACHE_SHIFT 13
#define VERTEX_CACHE_MASK 0x7F8
#define COLOUR_INDEX_SHIFT 7
#define COLOUR_INDEX_MASK 0x1FC
#define TEXTURE_FIELD_SHIFT 23
#define TEXTURE_INDEX_MASK 0xFF
/* A texture's record: three words */
#define TEXTURE_RECORD_BYTES 12
/* An animation's entry: its frame's shift of the clock (bits 26 to 31), its first frame (19 on), its frames' mask (12
 * to 18), bit 11 another entry's index, its index (0 to 10) */
#define ANIMATION_SHIFT_SHIFT 26
#define ANIMATION_START_SHIFT 19
#define ANIMATION_MASK_SHIFT 12
#define ANIMATION_MASK_MASK 0x7F
#define ANIMATION_CHAINED (1 << 11)
#define ANIMATION_INDEX_MASK 0x7FF
/* The scratchpad's copy of the model's colours, which its vertex cache (8 bytes an entry) shares */
#define DRAW_COPY 0x140
/* A frame's word 0x14: its vertices' offset from it */
#define FRAME_VERTICES 5

/* The model's strips and the scratchpad's copy of its colours (func_800435F4) */
static void model_setup(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    u32 *strip = packet->strips;
    u32 *from = packet->colours;
    u32 *to = scratch->copied;
    s32 count = strip[0];

    pass->colours = from;
    for (; count > 0; count--) {
        *to++ = *from++;
    }
    strip++;
    pass->textures = packet->textures;
    pass->animations = packet->animations;
    pass->widths = packet->widths;
    pass->command = *strip++;
    pass->strip = strip;
    if (pass->morph) {
        pass->stream2 += ((u32 *)pass->stream2)[FRAME_VERTICES];
    } else {
        /* One frame */
    }
    pass->stream += ((u32 *)pass->stream)[FRAME_VERTICES];
    pass->bit_count = 0;
    pass->bits = 0;
    pass->bits2 = 0;
    *(u32 *)scratch->deltas = 0;
}

/* One of a vertex's three fields from the stream's bits, most significant first: CODE + 1 bits, signed; narrower than
 * 8, a delta from the field's last value and BASE, which the field's word gives */
#define FIELD_FULL 8
#define WORD_BITS 32
#define FIELD_SHIFT_FROM_CODE 0x1F

static void vertex_field(DrawPass *pass, u32 code, s32 base, u8 *field) {
    s32 width = code + 1;
    s32 shift = code ^ FIELD_SHIFT_FROM_CODE;
    s32 used = width;
    s32 value;
    u32 word;

    pass->bit_count += width;
    if (pass->bit_count <= 0) {
        value = (s32)pass->bits >> shift;
    } else {
        word = *(u32 *)pass->stream;
        pass->stream += sizeof(u32);
        value = (s32)(pass->bits | (word >> ((width - pass->bit_count) & SHIFT_COUNT_MASK))) >> shift;
        used = pass->bit_count;
        pass->bit_count -= WORD_BITS;
        pass->bits = word;
    }
    pass->bits <<= used & SHIFT_COUNT_MASK;
    if (width != FIELD_FULL) {
        value += base + (s8)*field;
    } else {
        /* The value itself */
    }
    *field = value;
}

/* A vertex's word of fields (as the stream's widths give it): the widths' codes and the bases of x (bits 6 to 8, base
 * bits 25 to 31 doubled), y (3 to 5, base 17 to 24) and z (0 to 2, base 9 to 16) */
#define WIDTH_X_SHIFT 6
#define WIDTH_Y_SHIFT 3
#define WIDTH_CODE_MASK 7
#define BASE_X_SHIFT 25
#define BASE_Y_UP 7
#define BASE_Z_UP 15
#define BASE_DOWN 24
/* The vertex's bytes in the frame's box, 4 units each: x and y as a VXY pair (x's carry into y dropped), z */
#define VERTEX_UNIT_SHIFT 2
#define VERTEX_PAIR_MASK 0x03FC03FC
#define VERTEX_Z_SHIFT 8
#define VERTEX_Z_MASK 0x3FC
#define RAW_VERTEX_BYTES 3
#define BYTE_BITS 8

/* A strip vertex (func_8003DD14): its VXY pair and z, from the cache or the stream (fields, or three bytes when the
 * model has no widths) and cached; its colour, the return value */
static void morph_vertex(DrawPass *pass, u32 command, u32 *xy, u32 *z);

/* A vertex's three bytes as the retail code's unaligned loads read them: the word holding the first (lwr), and the
 * next when the first's address has bit 1 set (lwl); only the low 24 bits count */
#define STREAM_CROSSES 2
#define WORD_ALIGN_MASK 3

static u32 stream_bytes(u8 *at, u32 crosses) {
    u32 *word = (u32 *)((u32)at & ~WORD_ALIGN_MASK);
    u32 shift = ((u32)at & WORD_ALIGN_MASK) * BYTE_BITS;
    u32 value = word[0] >> shift;

    if (crosses) {
        value |= word[1] << (WORD_BITS - shift);
    } else {
        /* Within the first word */
    }
    return value;
}

/* A vertex turned by the light matrix (func_8003FD14), a row at a time from three copies of it: y, x, then z */
static void lit_turn(u32 *xy, u32 *z) {
    u32 x, y;

    GTE_WRITE_DATA(GTE_VXY0, *xy);
    GTE_WRITE_DATA(GTE_VZ0, *z);
    GTE_WRITE_DATA(GTE_VXY1, *xy);
    GTE_WRITE_DATA(GTE_VZ1, *z);
    GTE_WRITE_DATA(GTE_VXY2, *xy);
    GTE_WRITE_DATA(GTE_VZ2, *z);
    Gte_MultiplyMatrix(GTE_MATRIX_LIGHT, 1);
    GTE_READ_DATA(GTE_IR2, y);
    Gte_MultiplyMatrix(GTE_MATRIX_LIGHT, 0);
    GTE_READ_DATA(GTE_IR1, x);
    *xy = (y << GTE_HALF_BITS) | (x & GTE_HALF_MASK);
    Gte_MultiplyMatrix(GTE_MATRIX_LIGHT, 2);
    GTE_READ_DATA(GTE_IR3, *z);
}

static u32 strip_vertex(DrawPass *pass, u32 command, u32 *xy, u32 *z) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 *cache = (u32 *)((u8 *)scratch + DRAW_COPY + ((command >> VERTEX_CACHE_SHIFT) & VERTEX_CACHE_MASK));
    u32 index = (command >> COLOUR_INDEX_SHIFT) & COLOUR_INDEX_MASK;
    u32 packed, at;
    u8 *bytes;

    if (pass->morph) {
        morph_vertex(pass, command, xy, z);
    } else if (command & VERTEX_CACHED) {
        *xy = cache[0];
        *z = cache[1];
    } else {
        if (pass->widths != 0) {
            at = *pass->widths++;
            vertex_field(pass, (at >> WIDTH_X_SHIFT) & WIDTH_CODE_MASK, ((s32)at >> BASE_X_SHIFT) * 2,
                         &scratch->deltas[0]);
            vertex_field(pass, (at >> WIDTH_Y_SHIFT) & WIDTH_CODE_MASK, (s32)(at << BASE_Y_UP) >> BASE_DOWN,
                         &scratch->deltas[2]);
            vertex_field(pass, at & WIDTH_CODE_MASK, (s32)(at << BASE_Z_UP) >> BASE_DOWN, &scratch->deltas[1]);
            packed = *(u32 *)scratch->deltas;
        } else {
            bytes = pass->stream;
            pass->stream += RAW_VERTEX_BYTES;
            packed = stream_bytes(bytes, (u32)bytes & STREAM_CROSSES);
        }
        packed <<= VERTEX_UNIT_SHIFT;
        *xy = ((packed & VERTEX_PAIR_MASK) + scratch->box_xy) & PAIR_X_CARRY_MASK;
        *z = ((packed >> VERTEX_Z_SHIFT) & VERTEX_Z_MASK) + scratch->box_z;
        if (pass->lit) {
            lit_turn(xy, z);
        } else {
            /* In the model's space */
        }
        cache[0] = *xy;
        cache[1] = *z;
    }
    if (command & VERTEX_COLOUR_FROM_COPY) {
        return scratch->copied[index / sizeof(u32)];
    } else {
        return pass->colours[index / sizeof(u32)];
    }
}

/* A morphing vertex's field from both streams in step, the second's bits in BITS2 (func_8003ED14) */
static void morph_field(DrawPass *pass, u32 code, s32 base, u8 *field, u8 *field2) {
    s32 width = code + 1;
    s32 shift = code ^ FIELD_SHIFT_FROM_CODE;
    s32 used = width;
    s32 value, value2;
    u32 word, word2;

    pass->bit_count += width;
    if (pass->bit_count <= 0) {
        value = (s32)pass->bits >> shift;
        value2 = (s32)pass->bits2 >> shift;
    } else {
        word = *(u32 *)pass->stream;
        word2 = *(u32 *)pass->stream2;
        pass->stream += sizeof(u32);
        pass->stream2 += sizeof(u32);
        value = (s32)(pass->bits | (word >> ((width - pass->bit_count) & SHIFT_COUNT_MASK))) >> shift;
        value2 = (s32)(pass->bits2 | (word2 >> ((width - pass->bit_count) & SHIFT_COUNT_MASK))) >> shift;
        used = pass->bit_count;
        pass->bit_count -= WORD_BITS;
        pass->bits = word;
        pass->bits2 = word2;
    }
    pass->bits <<= used & SHIFT_COUNT_MASK;
    pass->bits2 <<= used & SHIFT_COUNT_MASK;
    if (width != FIELD_FULL) {
        value += base + (s8)*field;
        value2 += base + (s8)*field2;
    } else {
        /* The values themselves */
    }
    *field = value;
    *field2 = value2;
}

/* The two frames' vertex halved between them: x and y as a VXY pair, z, each 2 units a frame */
#define MORPH_PAIR_MASK 0x00FF00FF
#define MORPH_Z_MASK 0xFF00
#define MORPH_Z_SHIFT 7

static void morph_vertex(DrawPass *pass, u32 command, u32 *xy, u32 *z) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 *cache = (u32 *)((u8 *)scratch + DRAW_COPY + ((command >> VERTEX_CACHE_SHIFT) & VERTEX_CACHE_MASK));
    u32 packed, packed2, at;
    u8 *bytes, *bytes2;

    if (command & VERTEX_CACHED) {
        *xy = cache[0];
        *z = cache[1];
        return;
    } else if (pass->widths != 0) {
        at = *pass->widths++;
        morph_field(pass, (at >> WIDTH_X_SHIFT) & WIDTH_CODE_MASK, ((s32)at >> BASE_X_SHIFT) * 2, &scratch->deltas[0],
                    &scratch->deltas2[0]);
        morph_field(pass, (at >> WIDTH_Y_SHIFT) & WIDTH_CODE_MASK, (s32)(at << BASE_Y_UP) >> BASE_DOWN,
                    &scratch->deltas[2], &scratch->deltas2[2]);
        morph_field(pass, at & WIDTH_CODE_MASK, (s32)(at << BASE_Z_UP) >> BASE_DOWN, &scratch->deltas[1],
                    &scratch->deltas2[1]);
        packed = *(u32 *)scratch->deltas;
        packed2 = *(u32 *)scratch->deltas2;
    } else {
        /* The retail code reads the second frame's three bytes as the first's alignment says; the game's frames
         * align alike */
        EQUIV_ASSUME((((u32)pass->stream ^ (u32)pass->stream2) & 2) == 0);
        bytes = pass->stream;
        bytes2 = pass->stream2;
        pass->stream += RAW_VERTEX_BYTES;
        pass->stream2 += RAW_VERTEX_BYTES;
        packed = stream_bytes(bytes, (u32)bytes & STREAM_CROSSES);
        packed2 = stream_bytes(bytes2, (u32)bytes & STREAM_CROSSES);
    }
    *xy = ((((packed & MORPH_PAIR_MASK) + (packed2 & MORPH_PAIR_MASK)) << 1) + scratch->box_xy) & PAIR_X_CARRY_MASK;
    *z = (((packed & MORPH_Z_MASK) + (packed2 & MORPH_Z_MASK)) >> MORPH_Z_SHIFT) + scratch->box_z;
    if (pass->lit) {
        lit_turn(xy, z);
    } else {
        /* In the model's space */
    }
    cache[0] = *xy;
    cache[1] = *z;
}

/* A colour command's colour: the scratchpad's copy's or the model's */
static u32 strip_colour(DrawPass *pass, u32 index, s32 from_copy) {
    if (from_copy) {
        return DRAW_SCRATCH->copied[index / sizeof(u32)];
    } else {
        return pass->colours[index / sizeof(u32)];
    }
}

/* The triangle's texture's record's offset in the model's textures, 0 for none: an animation's frame by the clock */
static u32 strip_texture(DrawPass *pass, u32 command) {
    u32 index = command & TEXTURE_INDEX_MASK;
    u32 entry, step, chained;

    if ((command << TEXTURE_FIELD_SHIFT) == 0) {
        return 0;
    } else if ((s32)(command << TEXTURE_FIELD_SHIFT) > 0) {
        /* A texture's index */
    } else {
        do {
            entry = pass->animations[index];
            step = (DRAW_SCRATCH->clock >> ((entry >> ANIMATION_SHIFT_SHIFT) & SHIFT_COUNT_MASK))
                   + (entry >> ANIMATION_START_SHIFT);
            step &= (entry >> ANIMATION_MASK_SHIFT) & ANIMATION_MASK_MASK;
            chained = entry & ANIMATION_CHAINED;
            index = (entry + step) & ANIMATION_INDEX_MASK;
        } while (chained);
    }
    return index * TEXTURE_RECORD_BYTES;
}

/* A strip's next command after its triangle: the colour commands' colours taken, the next vertex's command (or the
 * next strip's first, negative) the return value */
static u32 strip_next(DrawPass *pass) {
    u32 command;

    for (;;) {
        command = *pass->strip++;
        if ((s32)command < 0 || (command >> COLOUR_COMMAND_SHIFT) != 0) {
            return command;
        } else {
            pass->corner[1] = strip_colour(pass, (command >> COLOUR_INDEX_SHIFT) & COLOUR_INDEX_MASK,
                                           command & COLOUR_SECOND_FROM_COPY);
            pass->corner[2] = strip_colour(pass, command & COLOUR_INDEX_MASK, command & COLOUR_THIRD_FROM_COPY);
            pass->corner[0] = pass->corner[1];
        }
    }
}

/* The model drawers' start (func_800436C8): the frame, the frame's box, the far colour step */
static void model_start(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 *frame = pass->packet->kind.model.frames[0];
    u32 *second = pass->packet->kind.model.frames[1];
    u32 pair;

    pass->stream = (u8 *)frame;
    pass->next = scratch->next;
    if (pass->morph) {
        /* The boxes' corners' halves added as words and halved, x's carry kept in y (func_800436F4) */
        pass->stream2 = (u8 *)second;
        pair = frame[0] + second[0];
        scratch->box_xy = (((s32)pair >> (GTE_HALF_BITS + 1)) << GTE_HALF_BITS)
                          | (((s32)(pair << GTE_HALF_BITS) >> (GTE_HALF_BITS + 1)) & GTE_HALF_MASK);
        scratch->box_z = (s32)(frame[1] + second[1]) >> 1;
    } else {
        scratch->box_xy = frame[0];
        scratch->box_z = frame[1];
    }
    pass->packet->kind.model.far_setup(pass);
}

/* The object's flags (word 0xA8): set when a model is drawn clipped at a level, cleared otherwise */
#define OBJECT_FLAGS 0xA8
#define OBJECT_CLIPPED 0x800000

/* A model's strips with each triangle drawn by the colour routine, or by DRAW when given, on the GTE's three screen
 * points; RECORDS keeps the vertices and their height above the clip level in the scratchpad, as the clipping
 * drawers read them */
static void model_strips(DrawPass *pass, s32 records, void (*draw)(DrawPass *)) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;
    u32 command = pass->command;
    u32 xy, z;

    while (command != STRIP_END) {
        scratch->first_command = command;
        pass->corner[0] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY0, xy);
        GTE_WRITE_DATA(GTE_VZ0, z);
        if (records) {
            *(u32 *)&vertices[0] = xy;
            vertices[0].z = z;
            scratch->sides[0] = scratch->clip_y - ((s32)xy >> GTE_HALF_BITS);
        } else {
            /* No records */
        }
        command = pass->strip[0];
        pass->corner[1] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY1, xy);
        GTE_WRITE_DATA(GTE_VZ1, z);
        if (records) {
            *(u32 *)&vertices[1] = xy;
            vertices[1].z = z;
            scratch->sides[1] = scratch->clip_y - ((s32)xy >> GTE_HALF_BITS);
        } else {
            /* No records */
        }
        command = pass->strip[1];
        pass->corner[2] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY2, xy);
        GTE_WRITE_DATA(GTE_VZ2, z);
        command = scratch->first_command;
        Gte_Rtpt();
        if (records) {
            *(u32 *)&vertices[2] = xy;
            vertices[2].z = z;
            scratch->sides[2] = scratch->clip_y - ((s32)xy >> GTE_HALF_BITS);
        } else {
            /* No records */
        }
        pass->strip += 2;
        for (;;) {
            pass->command = command;
            pass->texture = strip_texture(pass, command);
            draw(pass);
            command = strip_next(pass);
            if ((s32)command < 0) {
                break;
            } else if (command & STRIP_FAN) {
                GTE_READ_DATA(GTE_SXY0, xy);
                GTE_READ_DATA(GTE_SZ1, z);
                GTE_WRITE_DATA(GTE_SXY1, xy);
                GTE_WRITE_DATA(GTE_SZ2, z);
                pass->corner[1] = pass->corner[0];
            } else if (records) {
                *(u32 *)&vertices[0] = *(u32 *)&vertices[1];
                vertices[0].z = vertices[1].z;
                scratch->sides[0] = scratch->sides[1];
            } else {
                /* The strip moves on: the GTE's points shift as it projects */
            }
            if (records) {
                *(u32 *)&vertices[1] = *(u32 *)&vertices[2];
                vertices[1].z = vertices[2].z;
                scratch->sides[1] = scratch->sides[2];
            } else {
                /* No records */
            }
            pass->corner[0] = pass->corner[1];
            pass->corner[1] = pass->corner[2];
            pass->corner[2] = strip_vertex(pass, command, &xy, &z);
            GTE_WRITE_DATA(GTE_VXY0, xy);
            GTE_WRITE_DATA(GTE_VZ0, z);
            Gte_Rtps();
            if (records) {
                *(u32 *)&vertices[2] = xy;
                vertices[2].z = z;
                scratch->sides[2] = scratch->clip_y - ((s32)xy >> GTE_HALF_BITS);
            } else {
                /* No records */
            }
        }
    }
}

/* A triangle by the colour routine, the texture's record in the scratchpad (D_8003E360's) */
static void shade_triangle(DrawPass *pass) {
    DRAW_SCRATCH->texture_record = pass->textures + pass->texture;
    pass->shade(pass);
}

/* A started model's strips, flagged or not as clipped at a level, when its test draws it */
static void model_drawn(DrawPass *pass, s32 clipped, void (*triangle)(DrawPass *)) {
    u32 *flags = (u32 *)(pass->object + OBJECT_FLAGS);

    if (clipped) {
        *flags |= OBJECT_CLIPPED;
    } else {
        *flags &= ~OBJECT_CLIPPED;
    }
    pass->shade = pass->packet->kind.model.shade;
    if (pass->packet->kind.model.visible(pass) == DRAW_IT) {
        model_setup(pass);
        model_strips(pass, clipped, triangle);
    } else {
        /* Off the screen */
    }
    DRAW_SCRATCH->next = pass->next;
}

/* A model drawn whole, clipped by nothing (morphing between its two frames: func_8003F410) */
static void model_whole(DrawPass *pass, s32 morph) {
    pass->morph = morph;
    pass->lit = 0;
    model_start(pass);
    model_drawn(pass, 0, shade_triangle);
}

void func_8003E360(DrawPass *pass) {
    model_whole(pass, 0);
}

void func_8003F410(DrawPass *pass) {
    model_whole(pass, 1);
}

static s32 model_level(DrawPass *pass, s32 object_y);
static void model_far_depths(DrawPass *pass);
static void clip_triangle(DrawPass *pass);

/* A model clipped at the level when its frame's box reaches it, its parts below and above at the packet's two far
 * depths; drawn whole otherwise (at the first) */
static void model_at_level(DrawPass *pass, s32 morph) {
    DrawScratch *scratch = DRAW_SCRATCH;
    s32 clip_y = model_level(pass, scratch->object_y);

    pass->morph = morph;
    pass->lit = 0;
    model_far_depths(pass);
    model_start(pass);
    scratch->clip_y = clip_y;
    if (clip_y - ((s32)scratch->box_xy >> GTE_HALF_BITS) < 0) {
        model_drawn(pass, 0, shade_triangle);
    } else {
        model_drawn(pass, 1, clip_triangle);
    }
}

void func_8003DED8(DrawPass *pass) {
    model_at_level(pass, 0);
}

/* Each triangle by the routine the scratchpad holds (word 0x130) */
static void scratch_triangle(DrawPass *pass) {
    ((DrawStep)DRAW_SCRATCH->colour_mask)(pass);
}

void func_800454A4(DrawPass *pass);
void func_8003E4FC(DrawPass *pass);

static void model_clipped_by(DrawPass *pass, DrawStep triangle, s32 morph) {
    DrawScratch *scratch = DRAW_SCRATCH;
    s32 clip_y;

    pass->morph = morph;
    pass->lit = 0;
    scratch->colour_mask = (u32)triangle;
    clip_y = model_level(pass, OBJECT_FIELD(pass->object, OBJECT_Y));
    model_start(pass);
    scratch->clip_y = clip_y;
    model_drawn(pass, 1, scratch_triangle);
}

/* A model clipped at the level by func_800454A4's triangles, or func_8003E4FC's */
void func_8003E118(DrawPass *pass) {
    model_clipped_by(pass, func_800454A4, 0);
}

void func_8003E128(DrawPass *pass) {
    model_clipped_by(pass, func_8003E4FC, 0);
}

/* The same, morphing */
void func_8003F1C8(DrawPass *pass) {
    model_clipped_by(pass, func_800454A4, 1);
}

void func_8003F1D8(DrawPass *pass) {
    model_clipped_by(pass, func_8003E4FC, 1);
}

/* ---- Models clipped at a level: the parts below and above it drawn at the packet's two far depths ---- */

/* The level in the model's space (func_800437BC, func_800437D8): the scratchpad's level less the object's y (24.8
 * then 16ths), over the matrix's y scale */
#define MATRIX_SCALE_Y (MATRIX_SCALE + 1)
#define LEVEL_Y_SHIFT 4

static s32 model_level(DrawPass *pass, s32 object_y) {
    s32 scale_y = pass->packet->matrix[MATRIX_SCALE_Y];
    s32 above = DRAW_SCRATCH->level - (object_y << LEVEL_Y_SHIFT);

    /* A zero scale, or the one quotient that overflows, never comes of the game's objects */
    EQUIV_ASSUME(scale_y != 0 && !(above == WORD_MIN && scale_y == -1));
    return above / scale_y;
}

/* The far depths (func_8004379C): the root's and the part below the level's from the packet's first, the part
 * above's from its second */
static void model_far_depths(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPacket *packet = pass->packet;
    u32 below = packet->far_depths[0] + FAR_DEPTH_BIAS;
    u32 above = packet->far_depths[1] + FAR_DEPTH_BIAS;

    scratch->far_depth = below;
    scratch->far_depths[0] = below;
    scratch->far_depths[1] = above;
}

/* A triangle's state before it is cut (func_8003F988, func_8003E8DC): the GTE's screen points, the corners' colours
 * and texture corners in their records, the texture's words copied to be given the cut points' corners */
#define CLIP_RECORD_OFFSET 0x10C

static void clip_record(DrawPass *pass);

static void clip_save(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;

    GTE_STORE_DATA(GTE_SXY0, scratch->screen[0]);
    GTE_STORE_DATA(GTE_SZ1, scratch->screen[1]);
    GTE_STORE_DATA(GTE_SXY1, scratch->screen[2]);
    GTE_STORE_DATA(GTE_SZ2, scratch->screen[3]);
    GTE_STORE_DATA(GTE_SXY2, scratch->screen[4]);
    GTE_STORE_DATA(GTE_SZ3, scratch->screen[5]);
    clip_record(pass);
}

/* The corners' colours and texture corners kept, the texture's words copied (func_80041600, func_8003F988's rest) */
static void clip_record(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;
    u32 *record;

    *(u32 *)vertices[0].colour = pass->corner[0];
    *(u32 *)vertices[1].colour = pass->corner[1];
    *(u32 *)vertices[2].colour = pass->corner[2];
    *(u32 *)vertices[3].colour = 0;
    *(u32 *)vertices[4].colour = 0;
    if (pass->texture != 0) {
        record = (u32 *)(pass->textures + pass->texture);
        scratch->texture_words[0] = record[-3];
        *(u16 *)&vertices[0].u = record[-3];
        scratch->texture_words[1] = record[-2];
        *(u16 *)&vertices[1].u = record[-2];
        scratch->texture_words[2] = record[-1];
        *(u16 *)&vertices[2].u = record[-1];
        scratch->texture_record = (u8 *)scratch + CLIP_RECORD_OFFSET;
    } else {
        scratch->texture_record = 0;
    }
    /* The pieces' colour routines take the copy's address (or 0) where the strips give the texture's offset */
    pass->texture = (u32)scratch->texture_record;
}

/* FROM's record moved toward TO's to the level, into OUT (func_8003F9EC): x, z, the colour (white instead for a
 * reflection, func_8003E940) and (textured) the texture's corner by the fraction (16.16) of y between them the level is
 * at; OUT's y the level */
#define WHITE_COMPONENT 0xFF
#define FRACTION_SHIFT 16

static s32 toward(s32 from, s32 to, s32 fraction) {
    return (s32)((u32)(to - from) * (u32)fraction) >> FRACTION_SHIFT;
}

static void clip_point(DrawVertex *from, DrawVertex *to, DrawVertex *out, s32 white) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 from_y = *(u32 *)from & ~GTE_HALF_MASK;
    s32 span = (s32)(*(u32 *)to - from_y) >> GTE_HALF_BITS;
    s32 level = scratch->clip_y;
    s32 fraction;
    s32 z;

    out->y = level;
    fraction = (s32)(((u32)level << GTE_HALF_BITS) - from_y) / span;
    if (scratch->texture_record != 0) {
        out->u = toward(from->u, to->u, fraction) + from->u;
        out->v = toward(from->v, to->v, fraction) + from->v;
    } else {
        /* Untextured */
    }
    out->x = toward(from->x, to->x, fraction) + from->x;
    z = toward(from->z, to->z, fraction);
    /* The retail code's add traps on an overflow, which the game's coordinates never make */
    EQUIV_ASSUME(!((z ^ from->z) >= 0 && (z + from->z ^ z) < 0));
    out->z = z + from->z;
    if (white) {
        out->colour[0] = WHITE_COMPONENT;
        out->colour[1] = WHITE_COMPONENT;
        out->colour[2] = WHITE_COMPONENT;
    } else {
        out->colour[0] = toward(from->colour[0], to->colour[0], fraction) + from->colour[0];
        out->colour[1] = toward(from->colour[1], to->colour[1], fraction) + from->colour[1];
        out->colour[2] = toward(from->colour[2], to->colour[2], fraction) + from->colour[2];
    }
}

/* A piece by the colour routine (func_8003FB24): its colours shifted and masked when its corner given is at or above
 * the level */
static void clip_piece(DrawPass *pass, s32 side) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 shift = scratch->colour_shift & SHIFT_COUNT_MASK;
    u32 mask = scratch->colour_mask;

    if (side >= 0) {
        pass->corner[0] = (pass->corner[0] >> shift) & mask;
        pass->corner[1] = (pass->corner[1] >> shift) & mask;
        pass->corner[2] = (pass->corner[2] >> shift) & mask;
    } else {
        /* Below the level: its colours as they are */
    }
    pass->shade(pass);
}

/* A clipped triangle's points: its corners (their screen points as saved) and the two cut points (their records' first
 * words, which the cut's projection rewrites) */
#define CLIP_FIRST_CUT 3
#define CLIP_SECOND_CUT 4

static u32 *clip_screen(s32 point) {
    DrawScratch *scratch = DRAW_SCRATCH;

    if (point < CLIP_FIRST_CUT) {
        return &scratch->screen[point * 2];
    } else {
        return (u32 *)&scratch->vertices[point];
    }
}

/* A reflection's piece (func_8003EA1C): its colours at 3/4 and its screen points 3 to the right when its corner
 * given is on the side the scratchpad's word 0x12C's sign picks */
#define REFLECTION_HALF_MASK 0x7F7F7F
#define REFLECTION_SHIFT_X 3

static u32 three_quarters(u32 colour) {
    u32 half = (colour >> 1) & REFLECTION_HALF_MASK;

    return ((half >> 1) & REFLECTION_HALF_MASK) + half;
}

static u32 shifted_right(u32 xy) {
    return (xy & ~GTE_HALF_MASK) | ((xy + REFLECTION_SHIFT_X) & GTE_HALF_MASK);
}

static void reflection_piece(DrawPass *pass, s32 side) {
    u32 xy;

    if ((s32)(side ^ DRAW_SCRATCH->colour_shift) >= 0) {
        pass->corner[0] = three_quarters(pass->corner[0]);
        pass->corner[1] = three_quarters(pass->corner[1]);
        pass->corner[2] = three_quarters(pass->corner[2]);
        GTE_READ_DATA(GTE_SXY0, xy);
        GTE_WRITE_DATA(GTE_SXY0, shifted_right(xy));
        GTE_READ_DATA(GTE_SXY1, xy);
        GTE_WRITE_DATA(GTE_SXY1, shifted_right(xy));
        GTE_READ_DATA(GTE_SXY2, xy);
        GTE_WRITE_DATA(GTE_SXY2, shifted_right(xy));
    } else {
        /* The other side: as it is */
    }
    pass->shade(pass);
}

/* One side's piece (func_80045878): drawn only when its corner given is on the side the scratchpad's word 0x12C's sign
 * picks */
static void one_side_piece(DrawPass *pass, s32 side) {
    if ((s32)(side ^ DRAW_SCRATCH->colour_shift) >= 0) {
        pass->shade(pass);
    } else {
        /* The other side: not drawn */
    }
}

/* How a triangle is cut: at the level (func_8003F59C's), as a reflection (func_8003E4FC's) or keeping one side
 * (func_800454A4's); the last two make the cut points white and keep the far depth */
#define CLIP_AT_LEVEL 0
#define CLIP_REFLECTED 1
#define CLIP_ONE_SIDE 2

static void clip_any_piece(DrawPass *pass, s32 side, s32 variant) {
    if (variant == CLIP_REFLECTED) {
        reflection_piece(pass, side);
    } else if (variant == CLIP_ONE_SIDE) {
        one_side_piece(pass, side);
    } else {
        clip_piece(pass, side);
    }
}

/* A piece of a cut triangle: FIRST and SECOND, then the first cut point (the GTE's third screen point, left by the
 * cut's projection, put back for the other variants' later pieces), the copied texture words given their corners (the
 * third only for the first piece), and the corners' colours (func_800454A4's first piece cut from the first corner
 * keeping the triangle's third colour, which it doesn't reload) */
static void clip_triangle_piece(DrawPass *pass, s32 first, s32 second, s32 first_piece, s32 side, s32 variant) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;

    GTE_LOAD_DATA(GTE_SXY0, clip_screen(first)[0]);
    GTE_LOAD_DATA(GTE_SZ1, clip_screen(first)[1]);
    GTE_LOAD_DATA(GTE_SXY1, clip_screen(second)[0]);
    GTE_LOAD_DATA(GTE_SZ2, clip_screen(second)[1]);
    if (variant != CLIP_AT_LEVEL && !first_piece) {
        GTE_LOAD_DATA(GTE_SXY2, clip_screen(CLIP_FIRST_CUT)[0]);
        GTE_LOAD_DATA(GTE_SZ3, clip_screen(CLIP_FIRST_CUT)[1]);
    } else {
        /* The cut's projection's */
    }
    if (scratch->texture_record != 0) {
        *(u16 *)&scratch->texture_words[0] = *(u16 *)&vertices[first].u;
        *(u16 *)&scratch->texture_words[1] = *(u16 *)&vertices[second].u;
        if (first_piece) {
            *(u16 *)&scratch->texture_words[2] = *(u16 *)&vertices[CLIP_FIRST_CUT].u;
        } else {
            /* The first cut point's, from the first piece */
        }
    } else {
        /* Untextured */
    }
    pass->texture = (u32)scratch->texture_record;
    pass->corner[0] = *(u32 *)vertices[first].colour;
    pass->corner[1] = *(u32 *)vertices[second].colour;
    if (variant == CLIP_ONE_SIDE && first_piece && first == 1) {
        /* The triangle's third corner's, as the retail code leaves it */
    } else {
        pass->corner[2] = *(u32 *)vertices[CLIP_FIRST_CUT].colour;
    }
    clip_any_piece(pass, side, variant);
}

/* A triangle cut where its two edges from the odd corner cross the level, into three pieces: the two on the pair's
 * side, then (at the odd side's far depth, cut at the level) the odd corner's */
static void clip_cut(DrawPass *pass, s32 odd, u32 odd_depth, s32 variant) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;
    s32 *sides = scratch->sides;

    clip_save(pass);
    if (odd == 0) {
        clip_point(&vertices[0], &vertices[1], &vertices[CLIP_FIRST_CUT], variant != CLIP_AT_LEVEL);
        clip_point(&vertices[0], &vertices[2], &vertices[CLIP_SECOND_CUT], variant != CLIP_AT_LEVEL);
    } else if (odd == 1) {
        clip_point(&vertices[0], &vertices[1], &vertices[CLIP_FIRST_CUT], variant != CLIP_AT_LEVEL);
        clip_point(&vertices[2], &vertices[1], &vertices[CLIP_SECOND_CUT], variant != CLIP_AT_LEVEL);
    } else {
        clip_point(&vertices[0], &vertices[2], &vertices[CLIP_FIRST_CUT], variant != CLIP_AT_LEVEL);
        clip_point(&vertices[1], &vertices[2], &vertices[CLIP_SECOND_CUT], variant != CLIP_AT_LEVEL);
    }
    GTE_LOAD_DATA(GTE_VXY1, *(u32 *)&vertices[CLIP_SECOND_CUT]);
    GTE_LOAD_DATA(GTE_VZ1, vertices[CLIP_SECOND_CUT].z);
    GTE_LOAD_DATA(GTE_VXY2, *(u32 *)&vertices[CLIP_FIRST_CUT]);
    GTE_LOAD_DATA(GTE_VZ2, vertices[CLIP_FIRST_CUT].z);
    Gte_Rtpt();
    GTE_STORE_DATA(GTE_SXY1, *(u32 *)&vertices[CLIP_SECOND_CUT]);
    GTE_STORE_DATA(GTE_SZ2, vertices[CLIP_SECOND_CUT].z);
    GTE_STORE_DATA(GTE_SXY2, *(u32 *)&vertices[CLIP_FIRST_CUT]);
    GTE_STORE_DATA(GTE_SZ3, vertices[CLIP_FIRST_CUT].z);
    if (odd == 0) {
        clip_triangle_piece(pass, 1, 2, 1, sides[1], variant);
        clip_triangle_piece(pass, 2, CLIP_SECOND_CUT, 0, sides[2], variant);
    } else if (odd == 1) {
        clip_triangle_piece(pass, 2, 0, 1, sides[0], variant);
        clip_triangle_piece(pass, CLIP_SECOND_CUT, 2, 0, sides[2], variant);
    } else {
        clip_triangle_piece(pass, 0, 1, 1, sides[0], variant);
        clip_triangle_piece(pass, 1, CLIP_SECOND_CUT, 0, sides[1], variant);
    }
    if (variant == CLIP_AT_LEVEL) {
        scratch->far_depth = odd_depth;
    } else {
        /* The far depth kept */
    }
    if (odd == 0) {
        clip_triangle_piece(pass, CLIP_SECOND_CUT, 0, 0, sides[0], variant);
    } else if (odd == 1) {
        clip_triangle_piece(pass, 1, CLIP_SECOND_CUT, 0, sides[1], variant);
    } else {
        clip_triangle_piece(pass, CLIP_SECOND_CUT, 2, 0, sides[2], variant);
    }
}

/* The saved screen points back into the GTE, and the corners' colours, as the strip goes on from them */
static void clip_restore(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;

    GTE_LOAD_DATA(GTE_SXY0, scratch->screen[0]);
    GTE_LOAD_DATA(GTE_SZ1, scratch->screen[1]);
    GTE_LOAD_DATA(GTE_SXY1, scratch->screen[2]);
    GTE_LOAD_DATA(GTE_SZ2, scratch->screen[3]);
    GTE_LOAD_DATA(GTE_SXY2, scratch->screen[4]);
    GTE_LOAD_DATA(GTE_SZ3, scratch->screen[5]);
    pass->corner[0] = *(u32 *)vertices[0].colour;
    pass->corner[1] = *(u32 *)vertices[1].colour;
    pass->corner[2] = *(u32 *)vertices[2].colour;
}

static void clip_colours_back(DrawPass *pass) {
    DrawVertex *vertices = DRAW_SCRATCH->vertices;

    pass->corner[0] = *(u32 *)vertices[0].colour;
    pass->corner[1] = *(u32 *)vertices[1].colour;
    pass->corner[2] = *(u32 *)vertices[2].colour;
}

/* The corner its two edges are cut from: the one alone on its side of the level */
static s32 clip_odd(s32 *sides) {
    s32 in0 = sides[0] >= 0;
    s32 in1 = sides[1] >= 0;
    s32 in2 = sides[2] >= 0;

    if (in1 == in2) {
        return 0;
    } else if (in0 == in2) {
        return 1;
    } else {
        return 2;
    }
}

/* A triangle cut at the level (func_8003F59C): drawn whole when its corners are all on one side (below: at the first
 * far depth, as it is; above: the second, its colours shifted and masked), else cut, the pair's side at its far depth
 * first; the GTE's screen points and the corners' colours put back after a cut */
static void clip_triangle(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    s32 *sides = scratch->sides;
    u32 below = scratch->far_depths[0];
    u32 above = scratch->far_depths[1];
    s32 in0 = sides[0] >= 0;
    s32 in1 = sides[1] >= 0;
    s32 in2 = sides[2] >= 0;
    s32 odd;

    if (!in0 && !in1 && !in2) {
        scratch->far_depth = below;
        scratch->texture_record = pass->textures + pass->texture;
        pass->shade(pass);
    } else if (in0 && in1 && in2) {
        scratch->far_depth = above;
        clip_save(pass);
        clip_piece(pass, sides[0]);
        clip_colours_back(pass);
    } else {
        odd = clip_odd(sides);
        if (sides[(odd + 1) % 3] >= 0) {
            scratch->far_depth = above;
            clip_cut(pass, odd, below, CLIP_AT_LEVEL);
        } else {
            scratch->far_depth = below;
            clip_cut(pass, odd, above, CLIP_AT_LEVEL);
        }
        clip_restore(pass);
    }
}

/* A triangle cut by the other variants: as one piece when its corners are all on one side */
static void clip_other(DrawPass *pass, s32 variant) {
    s32 *sides = DRAW_SCRATCH->sides;
    s32 in0 = sides[0] >= 0;
    s32 in1 = sides[1] >= 0;
    s32 in2 = sides[2] >= 0;

    if (in0 == in1 && in1 == in2) {
        clip_save(pass);
        clip_any_piece(pass, sides[0], variant);
    } else {
        clip_cut(pass, clip_odd(sides), 0, variant);
    }
    clip_restore(pass);
}

/* A reflection's triangle (func_8003E4FC, func_8003E128's): cut at the level as func_8003F59C cuts, the cut points
 * white, each piece darkened and moved on its reflected side */
void func_8003E4FC(DrawPass *pass) {
    clip_other(pass, CLIP_REFLECTED);
}

/* A triangle cut at the level, the pieces on the other side of it than word 0x12C's sign picks left out
 * (func_800454A4, func_8003E118's) */
void func_800454A4(DrawPass *pass) {
    clip_other(pass, CLIP_ONE_SIDE);
}

/* ---- Mirrored models: each triangle drawn again reflected in a level ---- */

/* The GTE's vertices reflected in the level (word 0x74, its y as a VXY pair's): y the level's less its own */
static u32 mirrored(u32 xy, u32 level) {
    return (level - (xy & ~GTE_HALF_MASK)) | (xy & GTE_HALF_MASK);
}

/* The fade word's bit 31, which turns the facing test, flipped for the reflection */
#define FADE_FACING_FLIP 0x8000

static void mirror_flip(void) {
    u16 *high = (u16 *)&DRAW_SCRATCH->fade + 1;

    *high ^= FADE_FACING_FLIP;
}

/* A triangle twice: reflected (its screen points the GTE's) at the second far depth, then as it is (the records')
 * at the first, the reflection's screen points kept */
static void mirror_triangle(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;

    scratch->texture_record = pass->textures + pass->texture;
    mirror_flip();
    scratch->far_depth = scratch->far_depths[1];
    pass->shade(pass);
    GTE_STORE_DATA(GTE_SXY0, scratch->screen[0]);
    GTE_STORE_DATA(GTE_SZ1, scratch->screen[1]);
    GTE_STORE_DATA(GTE_SXY1, scratch->screen[2]);
    GTE_STORE_DATA(GTE_SZ2, scratch->screen[3]);
    GTE_STORE_DATA(GTE_SXY2, scratch->screen[4]);
    GTE_STORE_DATA(GTE_SZ3, scratch->screen[5]);
    GTE_LOAD_DATA(GTE_SXY0, *(u32 *)&vertices[0]);
    GTE_LOAD_DATA(GTE_SZ1, vertices[0].z);
    GTE_LOAD_DATA(GTE_SXY1, *(u32 *)&vertices[1]);
    GTE_LOAD_DATA(GTE_SZ2, vertices[1].z);
    GTE_LOAD_DATA(GTE_SXY2, *(u32 *)&vertices[2]);
    mirror_flip();
    GTE_LOAD_DATA(GTE_SZ3, vertices[2].z);
    scratch->far_depth = scratch->far_depths[0];
    pass->shade(pass);
}

/* The GTE's screen points as they are, kept in the records */
static void mirror_keep(void) {
    DrawVertex *vertices = DRAW_SCRATCH->vertices;

    GTE_STORE_DATA(GTE_SXY0, *(u32 *)&vertices[0]);
    GTE_STORE_DATA(GTE_SZ1, vertices[0].z);
    GTE_STORE_DATA(GTE_SXY1, *(u32 *)&vertices[1]);
    GTE_STORE_DATA(GTE_SZ2, vertices[1].z);
    GTE_STORE_DATA(GTE_SXY2, *(u32 *)&vertices[2]);
    GTE_STORE_DATA(GTE_SZ3, vertices[2].z);
}

static void mirror_strips(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 command = pass->command;
    u32 xy, z, level, first, second, third;

    while (command != STRIP_END) {
        scratch->first_command = command;
        pass->corner[0] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY0, xy);
        GTE_WRITE_DATA(GTE_VZ0, z);
        command = pass->strip[0];
        pass->corner[1] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY1, xy);
        GTE_WRITE_DATA(GTE_VZ1, z);
        command = pass->strip[1];
        pass->corner[2] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY2, xy);
        GTE_WRITE_DATA(GTE_VZ2, z);
        command = scratch->first_command;
        Gte_Rtpt();
        pass->strip += 2;
        mirror_keep();
        level = scratch->sides[0];
        GTE_READ_DATA(GTE_VXY0, first);
        GTE_READ_DATA(GTE_VXY1, second);
        GTE_READ_DATA(GTE_VXY2, third);
        GTE_WRITE_DATA(GTE_VXY0, mirrored(first, level));
        GTE_WRITE_DATA(GTE_VXY1, mirrored(second, level));
        GTE_WRITE_DATA(GTE_VXY2, mirrored(third, level));
        Gte_Rtpt();
        for (;;) {
            pass->command = command;
            pass->texture = strip_texture(pass, command);
            mirror_triangle(pass);
            command = strip_next(pass);
            if ((s32)command < 0) {
                break;
            } else if (command & STRIP_FAN) {
                GTE_READ_DATA(GTE_SXY0, xy);
                GTE_READ_DATA(GTE_SZ1, z);
                GTE_WRITE_DATA(GTE_SXY1, xy);
                GTE_WRITE_DATA(GTE_SZ2, z);
                pass->corner[1] = pass->corner[0];
                scratch->screen[2] = scratch->screen[0];
                scratch->screen[3] = scratch->screen[1];
            } else {
                /* The strip moves on */
            }
            pass->corner[0] = pass->corner[1];
            pass->corner[1] = pass->corner[2];
            pass->corner[2] = strip_vertex(pass, command, &xy, &z);
            GTE_WRITE_DATA(GTE_VXY0, xy);
            GTE_WRITE_DATA(GTE_VZ0, z);
            Gte_Rtps();
            mirror_keep();
            level = scratch->sides[0];
            GTE_READ_DATA(GTE_VXY0, first);
            GTE_WRITE_DATA(GTE_VXY0, mirrored(first, level));
            GTE_LOAD_DATA(GTE_SXY1, scratch->screen[2]);
            GTE_LOAD_DATA(GTE_SZ2, scratch->screen[3]);
            GTE_LOAD_DATA(GTE_SXY2, scratch->screen[4]);
            GTE_LOAD_DATA(GTE_SZ3, scratch->screen[5]);
            Gte_Rtps();
        }
    }
}

/* A model with its reflection in the level (func_80040168; morphing, func_800417B0): unflagged */
static void model_mirrored(DrawPass *pass, s32 morph) {
    u32 *flags = (u32 *)(pass->object + OBJECT_FLAGS);

    pass->morph = morph;
    pass->lit = 0;
    model_far_depths(pass);
    model_start(pass);
    *flags &= ~OBJECT_CLIPPED;
    pass->shade = pass->packet->kind.model.shade;
    if (pass->packet->kind.model.visible(pass) == DRAW_IT) {
        model_setup(pass);
        mirror_strips(pass);
    } else {
        /* Off the screen */
    }
    DRAW_SCRATCH->next = pass->next;
}

void func_80040168(DrawPass *pass) {
    model_mirrored(pass, 0);
}

void func_800417B0(DrawPass *pass) {
    model_mirrored(pass, 1);
}

/* ---- Lit models: their vertices turned to the world's axes, cut at a level there ---- */

/* The GTE's three vertices from the records, projected */
static void world_project(s32 first, s32 second, s32 third) {
    DrawVertex *vertices = DRAW_SCRATCH->vertices;

    GTE_LOAD_DATA(GTE_VXY0, *(u32 *)&vertices[first]);
    GTE_LOAD_DATA(GTE_VZ0, vertices[first].z);
    GTE_LOAD_DATA(GTE_VXY1, *(u32 *)&vertices[second]);
    GTE_LOAD_DATA(GTE_VZ1, vertices[second].z);
    if (third >= 0) {
        GTE_LOAD_DATA(GTE_VXY2, *(u32 *)&vertices[third]);
        GTE_LOAD_DATA(GTE_VZ2, vertices[third].z);
    } else {
        /* The first cut point's, from the first piece */
    }
    Gte_Rtpt();
}

/* A piece of a triangle cut in the world: FIRST, SECOND and the first cut point projected, as func_8003F59C's pieces */
static void world_piece(DrawPass *pass, s32 first, s32 second, s32 first_piece, s32 side) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;

    if (first_piece) {
        world_project(first, second, CLIP_FIRST_CUT);
    } else {
        world_project(first, second, -1);
    }
    if (scratch->texture_record != 0) {
        *(u16 *)&scratch->texture_words[0] = *(u16 *)&vertices[first].u;
        *(u16 *)&scratch->texture_words[1] = *(u16 *)&vertices[second].u;
        if (first_piece) {
            *(u16 *)&scratch->texture_words[2] = *(u16 *)&vertices[CLIP_FIRST_CUT].u;
        } else {
            /* The first cut point's, from the first piece */
        }
    } else {
        /* Untextured */
    }
    pass->texture = (u32)scratch->texture_record;
    pass->corner[0] = *(u32 *)vertices[first].colour;
    pass->corner[1] = *(u32 *)vertices[second].colour;
    pass->corner[2] = *(u32 *)vertices[CLIP_FIRST_CUT].colour;
    clip_piece(pass, side);
}

/* A lit triangle cut at the level (func_80041218): func_8003F59C's cut on the turned vertices, each piece projected
 * itself; projected whole when not cut */
static void world_triangle(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;
    s32 *sides = scratch->sides;
    u32 below = scratch->far_depths[0];
    u32 above = scratch->far_depths[1];
    s32 in0 = sides[0] >= 0;
    s32 in1 = sides[1] >= 0;
    s32 in2 = sides[2] >= 0;
    s32 odd;
    u32 odd_depth;

    if (!in0 && !in1 && !in2) {
        scratch->far_depth = below;
        world_project(0, 1, 2);
        scratch->texture_record = pass->textures + pass->texture;
        pass->shade(pass);
        return;
    } else if (in0 && in1 && in2) {
        scratch->far_depth = above;
        clip_record(pass);
        world_project(0, 1, 2);
        clip_piece(pass, sides[0]);
        clip_colours_back(pass);
        return;
    } else {
        odd = clip_odd(sides);
    }
    if (sides[(odd + 1) % 3] >= 0) {
        scratch->far_depth = above;
        odd_depth = below;
    } else {
        scratch->far_depth = below;
        odd_depth = above;
    }
    clip_record(pass);
    if (odd == 0) {
        clip_point(&vertices[0], &vertices[1], &vertices[CLIP_FIRST_CUT], 0);
        clip_point(&vertices[0], &vertices[2], &vertices[CLIP_SECOND_CUT], 0);
        world_piece(pass, 1, 2, 1, sides[1]);
        world_piece(pass, 2, CLIP_SECOND_CUT, 0, sides[2]);
        scratch->far_depth = odd_depth;
        world_piece(pass, CLIP_SECOND_CUT, 0, 0, sides[0]);
    } else if (odd == 1) {
        clip_point(&vertices[0], &vertices[1], &vertices[CLIP_FIRST_CUT], 0);
        clip_point(&vertices[2], &vertices[1], &vertices[CLIP_SECOND_CUT], 0);
        world_piece(pass, 2, 0, 1, sides[0]);
        world_piece(pass, CLIP_SECOND_CUT, 2, 0, sides[2]);
        scratch->far_depth = odd_depth;
        world_piece(pass, 1, CLIP_SECOND_CUT, 0, sides[1]);
    } else {
        clip_point(&vertices[0], &vertices[2], &vertices[CLIP_FIRST_CUT], 0);
        clip_point(&vertices[1], &vertices[2], &vertices[CLIP_SECOND_CUT], 0);
        world_piece(pass, 0, 1, 1, sides[0]);
        world_piece(pass, 1, CLIP_SECOND_CUT, 0, sides[1]);
        scratch->far_depth = odd_depth;
        world_piece(pass, CLIP_SECOND_CUT, 2, 0, sides[2]);
    }
    clip_colours_back(pass);
}

/* A lit model's strips (func_8003FF14's): each vertex turned and kept in the records with its height above the level,
 * each triangle (its texture the last vertex's command's) cut by world_triangle */
static void lit_strips(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawVertex *vertices = scratch->vertices;
    u32 command = pass->command;
    u32 xy, z;

    while (command != STRIP_END) {
        pass->corner[0] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY0, xy);
        GTE_WRITE_DATA(GTE_VZ0, z);
        *(u32 *)&vertices[0] = xy;
        vertices[0].z = z;
        scratch->sides[0] = scratch->clip_y - ((s32)xy >> GTE_HALF_BITS);
        command = pass->strip[0];
        pass->corner[1] = strip_vertex(pass, command, &xy, &z);
        GTE_WRITE_DATA(GTE_VXY1, xy);
        GTE_WRITE_DATA(GTE_VZ1, z);
        *(u32 *)&vertices[1] = xy;
        vertices[1].z = z;
        scratch->sides[1] = scratch->clip_y - ((s32)xy >> GTE_HALF_BITS);
        command = pass->strip[1];
        pass->strip += 2;
        for (;;) {
            pass->corner[2] = strip_vertex(pass, command, &xy, &z);
            GTE_WRITE_DATA(GTE_VXY2, xy);
            GTE_WRITE_DATA(GTE_VZ2, z);
            *(u32 *)&vertices[2] = xy;
            vertices[2].z = z;
            scratch->sides[2] = scratch->clip_y - ((s32)xy >> GTE_HALF_BITS);
            pass->command = command;
            pass->texture = strip_texture(pass, command);
            world_triangle(pass);
            command = strip_next(pass);
            if ((s32)command < 0) {
                break;
            } else if (command & STRIP_FAN) {
                pass->corner[1] = pass->corner[0];
            } else {
                *(u32 *)&vertices[0] = *(u32 *)&vertices[1];
                vertices[0].z = vertices[1].z;
                scratch->sides[0] = scratch->sides[1];
            }
            *(u32 *)&vertices[1] = *(u32 *)&vertices[2];
            vertices[1].z = vertices[2].z;
            scratch->sides[1] = scratch->sides[2];
            pass->corner[0] = pass->corner[1];
            pass->corner[1] = pass->corner[2];
        }
    }
}

/* The level's height above the object (func_800437F4): the scratchpad's level less its y (24.8, then 16ths), in 1.0s;
 * the model reaches it from LEVEL_REACH below */
#define LEVEL_REACH 0x16A

static s32 level_above(s32 object_y) {
    return (DRAW_SCRATCH->level - (object_y << LEVEL_Y_SHIFT)) >> GTE_FRACTION_BITS;
}

/* A lit model (func_8003FF14; morphing, func_80040FC4): the drawer clipped at a level for an object not turned about
 * x; otherwise its vertices turned by its rotation (the light matrix) and cut at the level in the world, projected by
 * the camera's rotation and the object's translation; drawn whole when the level is below its reach */
static void lit_model(DrawPass *pass, s32 morph) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u8 *object = pass->object;
    u32 *matrix;
    s32 object_y;

    if (OBJECT_FIELD(object, OBJECT_ANGLE_X) == 0) {
        model_at_level(pass, morph);
        return;
    } else {
        pass->morph = morph;
        pass->lit = 0;
        model_far_depths(pass);
        model_start(pass);
    }
    object_y = OBJECT_FIELD(object, OBJECT_Y);
    scratch->clip_y = level_above(object_y);
    if (scratch->clip_y + LEVEL_REACH < 0) {
        model_drawn(pass, 0, shade_triangle);
        return;
    } else {
        *(u32 *)(object + OBJECT_FLAGS) |= OBJECT_CLIPPED;
        pass->shade = pass->packet->kind.model.shade;
    }
    if (pass->packet->kind.model.visible(pass) == DRAW_IT) {
        matrix = pass->packet->matrix;
        GTE_WRITE_CONTROL(GTE_L11L12, matrix[0]);
        GTE_WRITE_CONTROL(GTE_L13L21, matrix[1]);
        GTE_WRITE_CONTROL(GTE_L22L23, matrix[2]);
        GTE_WRITE_CONTROL(GTE_L31L32, matrix[3]);
        GTE_WRITE_CONTROL(GTE_L33, matrix[4]);
        rotation_load(scratch->camera_matrix);
        place_at_object(pass);
        model_setup(pass);
        pass->lit = 1;
        lit_strips(pass);
    } else {
        /* Off the screen */
    }
    scratch->next = pass->next;
}

void func_8003FF14(DrawPass *pass) {
    lit_model(pass, 0);
}

void func_80040FC4(DrawPass *pass) {
    lit_model(pass, 1);
}

/* The colour routines (the packet's word 0x24, D_8005B6EC), after the GTE has the triangle's screen points: a shaded
 * triangle (G3) or a textured one (GT3, its texture's record in the scratchpad), faded toward the far colour (DPCT)
 * unless the command's bit 24 says not or the fade is 0, linked into its depth's slot; none at a depth of 0, or facing
 * away (NCLIP, its sign turned by bit 29 and the fade word's bit 31) when bit 28 makes it one-sided */
#define TRIANGLE_ONE_SIDED (1 << 28)
#define TRIANGLE_FACING_SHIFT 2
#define TRIANGLE_UNFADED (1 << 24)
#define POLY_G3_CODE 0x30000000
#define POLY_G3_TRANSLUCENT_CODE 0x32000000
#define POLY_GT3_CODE 0x34000000
#define POLY_GT3_BLENDED_CODE 0x36000000
#define POLY_G3_WORDS 7
#define POLY_GT3_WORDS 10
#define POLY_G3_TAG_LENGTH 0x06000000
#define POLY_GT3_TAG_LENGTH 0x09000000
#define TEXTURE_BLEND_BITS 0x600000
#define TEXTURE_BLEND_ADDITIVE 0x200000
/* A draw mode primitive (texture page, translucency) before a translucent G3 */
#define DRAW_MODE_WORD 0xE1000A20
#define DRAW_MODE_WORDS 3
#define DRAW_MODE_TAG_LENGTH 0x02000000

/* The triangle's depths' sum, 0 when culled at a depth of 0 (the GTE's normal then in MAC0) */
static u32 shade_depths(void) {
    u32 first, second, third;

    GTE_READ_DATA(GTE_SZ1, first);
    GTE_READ_DATA(GTE_SZ2, second);
    GTE_READ_DATA(GTE_SZ3, third);
    if ((s32)first <= 0 || (s32)second <= 0) {
        return 0;
    } else {
        Gte_Nclip();
    }
    if ((s32)third <= 0) {
        return 0;
    } else {
        return first + second + third;
    }
}

/* The facing's sign word: the normal's, turned by the fade word's bit 31 and the command's bit 29; 0 for no normal */
static s32 shade_facing(DrawPass *pass, u32 fade, s32 *facing) {
    s32 normal;

    GTE_READ_DATA(GTE_MAC0, normal);
    *facing = normal ^ fade ^ (pass->command << TRIANGLE_FACING_SHIFT);
    return normal != 0;
}

/* The corners' colours faded toward the far colour, unless unfaded */
static void shade_fade(DrawPass *pass, u32 fade) {
    GTE_WRITE_DATA(GTE_RGB0, pass->corner[0]);
    GTE_WRITE_DATA(GTE_RGB1, pass->corner[1]);
    GTE_WRITE_DATA(GTE_RGB2, pass->corner[2]);
    GTE_WRITE_DATA(GTE_IR0, fade & GTE_HALF_MASK);
    if ((pass->command & TRIANGLE_UNFADED) == 0 && (fade & GTE_HALF_MASK) != 0) {
        Gte_Dpct();
    } else {
        /* Its colours as they are */
    }
}

/* The primitive linked into its slot, the frame's next primitive past it */
static void shade_link(DrawPass *pass, u32 *slot, u32 tag, u32 words) {
    u32 *primitive = pass->next;

    primitive[0] = *slot | tag;
    *slot = (u32)primitive & PRIM_ADDRESS_MASK;
    pass->next = primitive + words;
}

/* A G3 or GT3 of the GTE's colours, translucent ones: the G3 code 0x32 after a draw mode primitive, the GT3's page
 * made additive */
static void shade_emit(DrawPass *pass, u32 *slot, s32 translucent) {
    u32 *primitive = pass->next;
    u32 *mode;
    u32 *record;
    u32 first, second, third, colour, code;

    if (pass->texture == 0) {
        GTE_STORE_DATA(GTE_SXY0, primitive[2]);
        GTE_STORE_DATA(GTE_SXY1, primitive[4]);
        GTE_STORE_DATA(GTE_SXY2, primitive[6]);
        GTE_READ_DATA(GTE_RGB0, colour);
        if (translucent) {
            code = POLY_G3_TRANSLUCENT_CODE;
        } else {
            code = POLY_G3_CODE;
        }
        primitive[1] = code | colour;
        GTE_STORE_DATA(GTE_RGB1, primitive[3]);
        GTE_STORE_DATA(GTE_RGB2, primitive[5]);
        if (translucent) {
            /* The draw mode primitive takes the slot, linked to the G3 */
            primitive[0] = *slot | POLY_G3_TAG_LENGTH;
            mode = primitive + POLY_G3_WORDS;
            mode[1] = DRAW_MODE_WORD;
            mode[2] = 0;
            mode[0] = DRAW_MODE_TAG_LENGTH | ((u32)primitive & PRIM_ADDRESS_MASK);
            *slot = (u32)mode & PRIM_ADDRESS_MASK;
            pass->next = mode + DRAW_MODE_WORDS;
        } else {
            shade_link(pass, slot, POLY_G3_TAG_LENGTH, POLY_G3_WORDS);
        }
    } else {
        record = (u32 *)DRAW_SCRATCH->texture_record;
        second = record[-2];
        if (translucent) {
            second = (second & ~TEXTURE_BLEND_BITS) | TEXTURE_BLEND_ADDITIVE;
        } else {
            /* The page's own blending */
        }
        first = record[-3];
        third = record[-1];
        GTE_STORE_DATA(GTE_SXY0, primitive[2]);
        GTE_STORE_DATA(GTE_SXY1, primitive[5]);
        GTE_STORE_DATA(GTE_SXY2, primitive[8]);
        primitive[3] = first;
        primitive[6] = second;
        primitive[9] = third;
        GTE_READ_DATA(GTE_RGB0, colour);
        if (!translucent && (second & TEXTURE_BLEND_BITS) == TEXTURE_BLEND_BITS) {
            code = POLY_GT3_CODE;
        } else {
            code = POLY_GT3_BLENDED_CODE;
        }
        primitive[1] = code | colour;
        GTE_STORE_DATA(GTE_RGB1, primitive[4]);
        GTE_STORE_DATA(GTE_RGB2, primitive[7]);
        shade_link(pass, slot, POLY_GT3_TAG_LENGTH, POLY_GT3_WORDS);
    }
}

/* The screen points' halves when all three are off the same edge (x 0 to 511, y 0 to 216) */
#define SCREEN_EDGES 0x00D90200
#define HALF_SIGN_UP 16

/* The screen points and-ed together are left where the texture's offset was (the retail code's $a2) */
static s32 off_one_edge(DrawPass *pass) {
    u32 first, second, third, beyond, before;

    GTE_READ_DATA(GTE_SXY0, first);
    GTE_READ_DATA(GTE_SXY1, second);
    GTE_READ_DATA(GTE_SXY2, third);
    before = first & second & third;
    beyond = ~((first - SCREEN_EDGES) | (second - SCREEN_EDGES) | (third - SCREEN_EDGES)) | before;
    if ((s32)beyond < 0 || (s32)(beyond << HALF_SIGN_UP) < 0) {
        pass->texture = before;
        return 1;
    } else {
        return 0;
    }
}

#define SHADE_PLAIN 0
#define SHADE_TRANSLUCENT 1
#define SHADE_ON_SCREEN 2

static void shade_as(DrawPass *pass, s32 variant) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 depth_sum = shade_depths();
    u32 fade, *slot;
    s32 facing;

    if (depth_sum == 0) {
        return;
    } else {
        fade = scratch->fade;
    }
    if (!shade_facing(pass, fade, &facing)) {
        return;
    } else if (variant == SHADE_ON_SCREEN) {
        scratch->clipped_record = (u8 *)pass->texture;
    } else {
        /* No word kept */
    }
    if ((pass->command & TRIANGLE_ONE_SIDED) && facing <= 0) {
        return;
    } else if (variant == SHADE_ON_SCREEN && off_one_edge(pass)) {
        return;
    } else {
        shade_fade(pass, fade);
        slot = depth_slot_of(pass, depth_sum);
        shade_emit(pass, slot, variant == SHADE_TRANSLUCENT);
    }
}

/* The default (func_8003EBC4), translucent (func_8003FB84), and culled off the screen's edges (func_80041B80) */
void func_8003EBC4(DrawPass *pass) {
    shade_as(pass, SHADE_PLAIN);
}

void func_8003FB84(DrawPass *pass) {
    shade_as(pass, SHADE_TRANSLUCENT);
}

void func_80041B80(DrawPass *pass) {
    shade_as(pass, SHADE_ON_SCREEN);
}

/* Glowing by depth (func_80040B44, the command's bit 24; the default otherwise): a GT3 whose corners each fade from
 * black at GLOW_NEAR to their colour GLOW_SPAN further (DPCS toward the far colour), none when all three are black; its
 * colours kept in the scratchpad's words 0x124 to 0x12C, the first before the facing test */
#define TRIANGLE_GLOWS TRIANGLE_UNFADED
#define GLOW_NEAR 0x258
#define GLOW_SPAN 0x2D8
#define GLOW_SHIFT 5
#define COLOUR_MASK 0xFFFFFF

static void depth_glow_corner(u32 *kept, u32 colour, s32 depth, s32 first) {
    s32 past = depth - GLOW_NEAR;
    s32 left;

    if (past <= 0) {
        *kept = 0;
        return;
    } else if (first) {
        GTE_WRITE_DATA(GTE_RGB2, colour);
    } else {
        /* The FIFO as it is */
    }
    left = GLOW_SPAN - past;
    if (left >= 0) {
        GTE_WRITE_DATA(GTE_RGBC, colour);
        GTE_WRITE_DATA(GTE_IR0, left << GLOW_SHIFT);
        Gte_Dpcs();
        GTE_STORE_DATA(GTE_RGB2, *kept);
    } else {
        /* Past the glow: its colour, kept */
    }
}

void func_80040B44(DrawPass *pass) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 *kept = (u32 *)&scratch->level;
    u32 *primitive = pass->next;
    u32 *record;
    u32 depth_sum, fade, *slot, first, second, third;
    s32 facing, depth;

    if ((pass->command & TRIANGLE_GLOWS) == 0) {
        func_8003EBC4(pass);
        return;
    } else {
        depth_sum = shade_depths();
    }
    if (depth_sum == 0) {
        return;
    } else {
        fade = scratch->fade;
    }
    if (!shade_facing(pass, fade, &facing)) {
        return;
    } else {
        kept[0] = pass->corner[0];
    }
    if ((pass->command & TRIANGLE_ONE_SIDED) && facing <= 0) {
        return;
    } else {
        kept[1] = pass->corner[1];
        kept[2] = pass->corner[2];
    }
    GTE_READ_DATA(GTE_SZ1, depth);
    depth_glow_corner(&kept[0], pass->corner[0], depth, 1);
    GTE_READ_DATA(GTE_SZ2, depth);
    depth_glow_corner(&kept[1], pass->corner[1], depth, 0);
    GTE_READ_DATA(GTE_SZ3, depth);
    depth_glow_corner(&kept[2], pass->corner[2], depth, 0);
    if ((kept[0] | kept[1] | kept[2]) == 0) {
        return;
    } else {
        slot = depth_slot_of(pass, depth_sum);
        primitive[1] = POLY_GT3_BLENDED_CODE | (kept[0] & COLOUR_MASK);
    }
    primitive[4] = kept[1];
    record = (u32 *)scratch->texture_record;
    primitive[7] = kept[2];
    first = record[-3];
    second = record[-2];
    third = record[-1];
    GTE_STORE_DATA(GTE_SXY0, primitive[2]);
    GTE_STORE_DATA(GTE_SXY1, primitive[5]);
    GTE_STORE_DATA(GTE_SXY2, primitive[8]);
    primitive[3] = first;
    primitive[6] = second;
    primitive[9] = third;
    shade_link(pass, slot, POLY_GT3_TAG_LENGTH, POLY_GT3_WORDS);
}

/* Lit flat textured triangles (func_8004075C, func_80040948): an FT3 of the light's colour (the far colour, DPCS of
 * black by the light's strength: the light matrix times the first corner's normal, its bytes x, y and z) plus a
 * highlight from D_8005C650 by the third row's, both halved and 10 slots further back when it faces away;
 * the page's blending made opaque, or additive (the first, facing; the second, by its facing, culled when one-sided and
 * facing away on an opaque page) */
#define NORMAL_SHIFT_UP 24
#define NORMAL_SHIFT_DOWN 19
#define LIGHT_BIAS 0x1000
#define LIGHT_SCALE 7
#define LIGHT_ROUND 0x2000
#define LIGHT_SHIFT 4
#define HIGHLIGHT_SHIFT 3
#define HIGHLIGHT_START 0x180
#define HIGHLIGHT_STEPS 0x80
#define HIGHLIGHT_BACK_SHIFT 3
#define BACK_FACE_DEPTH 10
#define COMPONENT_MAX 0xFF
#define POLY_FT3_CODE 0x24000000
#define POLY_FT3_BLENDED_CODE 0x26000000
#define POLY_FT3_WORDS 8
#define POLY_FT3_TAG_LENGTH 0x07000000
#define TEXTURE_BLEND_SUBTRACTIVE 0x400000

extern u8 D_8005C650[];

static s32 normal_component(u32 colour, s32 byte) {
    return (s32)((colour >> (byte * BYTE_BITS)) << NORMAL_SHIFT_UP) >> NORMAL_SHIFT_DOWN;
}

static u32 lit_component(u32 colour, s32 byte, u32 highlight, u32 place) {
    u32 component = ((colour >> (byte * BYTE_BITS)) & COMPONENT_MAX) + highlight;

    if ((s32)(component - COMPONENT_MAX) > 0) {
        return COMPONENT_MAX << place;
    } else {
        return component << place;
    }
}

#define LIT_ADDITIVE_FACING 0
#define LIT_BY_FACING 1

static void lit_triangle(DrawPass *pass, s32 variant) {
    DrawScratch *scratch = DRAW_SCRATCH;
    u32 *primitive = pass->next;
    u32 *record;
    u32 depth_sum, fade, *slot, far, strength, highlight, colour, light, page, code, first, third;
    s32 facing, normal, reach;

    depth_sum = shade_depths();
    if (depth_sum == 0) {
        return;
    } else {
        fade = scratch->fade;
    }
    GTE_READ_DATA(GTE_MAC0, normal);
    facing = normal ^ fade ^ (pass->command << TRIANGLE_FACING_SHIFT);
    /* The facing is left where the texture's offset was (the retail code's $a2) */
    pass->texture = facing;
    if (normal == 0) {
        return;
    } else {
        far = scratch->far_depth;
    }
    if (facing < 0) {
        far -= BACK_FACE_DEPTH;
    } else {
        /* Facing the camera */
    }
    slot = depth_slot_of_depth(pass, far - (depth_sum >> DEPTH_SUM_SHIFT));
    GTE_STORE_DATA(GTE_SXY0, primitive[2]);
    GTE_STORE_DATA(GTE_SXY1, primitive[4]);
    GTE_STORE_DATA(GTE_SXY2, primitive[6]);
    GTE_WRITE_DATA(GTE_IR1, normal_component(pass->corner[0], 0));
    GTE_WRITE_DATA(GTE_IR2, normal_component(pass->corner[0], 1));
    GTE_WRITE_DATA(GTE_IR3, normal_component(pass->corner[0], 2));
    Gte_MultiplyMatrix(GTE_MATRIX_LIGHT, GTE_VECTOR_IR);
    GTE_READ_DATA(GTE_IR1, strength);
    GTE_READ_DATA(GTE_IR3, reach);
    strength = ((strength + LIGHT_BIAS) * LIGHT_SCALE + LIGHT_ROUND) >> LIGHT_SHIFT;
    if (facing < 0) {
        strength >>= 1;
    } else {
        /* Facing the camera */
    }
    strength -= fade & GTE_HALF_MASK;
    if ((s32)strength < 0) {
        strength = 0;
    } else {
        /* Lit */
    }
    reach >>= HIGHLIGHT_SHIFT;
    GTE_WRITE_DATA(GTE_IR0, strength);
    Gte_Dpcs();
    if (reach < 0) {
        reach = -reach;
    } else {
        /* Toward the light */
    }
    reach -= HIGHLIGHT_START;
    if (reach < 0) {
        highlight = 0;
    } else if (reach - HIGHLIGHT_STEPS >= 0) {
        highlight = D_8005C650[HIGHLIGHT_STEPS - 1];
    } else {
        highlight = D_8005C650[reach];
    }
    GTE_READ_DATA(GTE_RGB2, light);
    if (facing < 0) {
        highlight >>= HIGHLIGHT_BACK_SHIFT;
    } else {
        /* Facing the camera */
    }
    colour = lit_component(light, 2, highlight, 2 * BYTE_BITS) | lit_component(light, 1, highlight, BYTE_BITS)
             | lit_component(light, 0, highlight, 0);
    record = (u32 *)scratch->texture_record;
    page = record[-2];
    if (variant == LIT_ADDITIVE_FACING) {
        if ((page & TEXTURE_BLEND_BITS) != 0 || facing < 0) {
            code = POLY_FT3_CODE;
            page |= TEXTURE_BLEND_BITS;
        } else {
            code = POLY_FT3_BLENDED_CODE;
            page |= TEXTURE_BLEND_ADDITIVE;
        }
    } else if ((page & TEXTURE_BLEND_BITS) != 0) {
        if ((s32)((pass->command << 3) & facing) < 0) {
            return;
        } else {
            code = POLY_FT3_CODE;
            page |= TEXTURE_BLEND_BITS;
        }
    } else if (facing >= 0) {
        code = POLY_FT3_BLENDED_CODE;
        page |= TEXTURE_BLEND_ADDITIVE;
    } else {
        code = POLY_FT3_BLENDED_CODE;
        page |= TEXTURE_BLEND_SUBTRACTIVE;
    }
    primitive[1] = code | colour;
    first = record[-3];
    third = record[-1];
    primitive[3] = first;
    primitive[5] = page;
    primitive[7] = third;
    shade_link(pass, slot, POLY_FT3_TAG_LENGTH, POLY_FT3_WORDS);
}

void func_8004075C(DrawPass *pass) {
    lit_triangle(pass, LIT_ADDITIVE_FACING);
}

void func_80040948(DrawPass *pass) {
    lit_triangle(pass, LIT_BY_FACING);
}

/* Draws the frame's packet list, each packet through its three steps, into the frame's ordering table and primitives */
#define CAMERA_WORDS (MATRIX_WORDS + 3)

void func_8003DB94(DrawPacket *list, FrameBuffer *frame, WorldScene *scene) {
    DrawScratch *scratch = DRAW_SCRATCH;
    DrawPass pass;
    DrawStep place, draw;
    u32 *camera;
    s32 word;

    scratch->packet = list;
    scratch->scene = scene;
    scratch->next = frame->next;
    scratch->fog_far = scene->depth_fog_shift;
    scratch->fog_shift = scene->depth_fog_shift;
    scratch->fog_start = scene->depth_fog_start;
    camera = scene->camera_matrix;
    scratch->camera = scene->camera;
    for (word = 0; word < CAMERA_WORDS; word++) {
        scratch->camera_matrix[word] = camera[word];
    }
    pass.frame = frame;
    while ((pass.packet = scratch->packet) != 0) {
        place = pass.packet->place;
        reload_rotation(&pass);
        scratch->clock = pass.packet->clock;
        draw = pass.packet->draw;
        scratch->packet = pass.packet->next;
        pass.object = pass.packet->object;
        scratch->far_depth = OBJECT_FIELD(pass.object, OBJECT_DEPTH_BIAS) + FAR_DEPTH_BIAS;
        place(&pass);
        pass.scene = scratch->scene;
        pass.packet->prepare(&pass);
        draw(&pass);
    }
    frame->next = scratch->next;
}
