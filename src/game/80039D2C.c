/* The handwritten code's equivalent C (docs/WORKFLOW.md, "The handwritten code"). Only the equivalent build compiles
 * it: configure.py leaves each function defined here out of that build's copy of asm/game/80039D2C.s, and links this
 * file after it. The default build assembles the handwritten code as it is. */
#include "common.h"
#include "game.h"

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

s32 Math_SqrMagnitude2(s32 x, s32 y) {
    u64 sum = (s64)x * x + (s64)y * y;

    return sum >> SQR_MAGNITUDE2_SHIFT;
}

s32 Math_SqrMagnitude3(s32 x, s32 y, s32 z) {
    u64 sum = (s64)x * x + (s64)y * y + (s64)z * z;

    return sum >> SQR_MAGNITUDE3_SHIFT;
}

s32 Math_Sin(s32 angle) {
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

s32 Math_Cos(s32 angle) {
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

/* func_8003CBF0 with x and y in $v1 and $a3; that routine stays assembly while func_8003C88C calls it */
EQUIV_RUNS(func_8003CBE4, func_8003CBF0)

void func_8003CBE4(s32 x, s32 y) {
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
