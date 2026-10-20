/* The handwritten code's equivalent C (docs/WORKFLOW.md, "The handwritten code"). Only the equivalent build compiles
 * it: configure.py leaves each function defined here out of that build's copy of asm/game/80039D2C.s, and links this
 * file after it. The default build assembles the handwritten code as it is. */
#include "common.h"
#include "game.h"
#define GTE_MVMVA
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
 * offsets from the zone's origin, as halves of 16ths */
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
