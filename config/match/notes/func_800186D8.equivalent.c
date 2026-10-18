#include "common.h"
#include "sdk.h"
#include "game.h"
#include "gte.h"

typedef struct {
    u8 pad0[0x8];
    s32 unk8;
    u8 padC[0x202C - 0xC];
    u16 unk202C;
    u16 unk202E;
    u8 pad2030[0x2088 - 0x2030];
    void *unk2088;
    char *unk208C;
    char *unk2090;
} Unk80063458;

typedef struct {
    s32 a;
    s32 pad;
    s16 b;
} Unk80060884;

extern Unk80063458 *g_BuildBuffer[];
extern Unk80060884 D_80060884;

/* A primitive's colour word: red, green, blue, and the GPU command code in the top byte */
typedef union PrimColour {
    u32 word;
    u8 c[4];
} PrimColour;

#define PRIM_COLOUR_CODE 3

/* A glyph's first words, as its quad takes them: colour, then texture coordinates with the CLUT and texture page */
typedef struct GlyphTexture {
    /* 0x00 */ PrimColour colour;
    /* 0x04 */ u32 uv0_clut;
    /* 0x08 */ u32 uv1_tpage;
    /* 0x0C */ u32 uv2_uv3;
} GlyphTexture;

/* libgpu's POLY_GT4: a textured quad shaded from a colour per vertex */
typedef struct PolyGT4 {
    /* 0x00 */ u32 tag;
    /* 0x04 */ PrimColour colour0;
    /* 0x08 */ u32 xy0;
    /* 0x0C */ u32 uv0_clut;
    /* 0x10 */ PrimColour colour1;
    /* 0x14 */ u32 xy1;
    /* 0x18 */ u32 uv1_tpage;
    /* 0x1C */ PrimColour colour2;
    /* 0x20 */ u32 xy2;
    /* 0x24 */ u16 uv2;
    /* 0x26 */ u16 pad2;
    /* 0x28 */ PrimColour colour3;
    /* 0x2C */ u32 xy3;
    /* 0x30 */ u16 uv3;
    /* 0x32 */ u16 pad3;
} PolyGT4;

/* A box's corners, as func_8001B83C keeps a string's bounds: x, y, z of one, then of the other */
#define BOX_X0 0
#define BOX_Y0 1
#define BOX_X1 3
#define BOX_Y1 4
/* A vertex in the plane z = 0 for the GTE: x in the low half, y in the high */
#define VERTEX_XY(x, y) ((u16)(x) | ((y) << GTE_HALF_BITS))
/* The ordering table's last entry, and the depth a primitive at Z 0 sorts at before the projection's bias */
#define OT_LAST 0x7FF
#define OT_DEPTH_BIAS 0x800
/* The three vertices' Z summed, / 32 */
#define SZ_SUM_SHIFT 5
#define PRIM_ADDRESS_MASK 0xFFFFFF
/* addPrim: a primitive's words after its tag, in the tag's top byte */
#define GT4_TAG_LENGTH 0x0C000000
/* POLY_GT4's command code, ORed into the glyph's */
#define GT4_CODE 0x3C
/* The object's halfwords at 0x28: a red, green and blue scale per vertex, 1.0 at 0x80 */
#define SHADE_FIRST 4
#define SHADE_SHIFT 7

/* A component of a glyph's colour scaled by the object's shade for a vertex */
#define SHADED(glyph, obj, vertex, component) \
    (((glyph)->colour.c[component] * (obj)->unk_20[SHADE_FIRST + (vertex) * 3 + (component)]) >> SHADE_SHIFT)

/* A box's quad in front of the camera, textured from a glyph, shaded per vertex by the object when shade is set, sorted
   into the ordering table by its depth; nothing when the GTE flags an error projecting the first three corners */
void func_800186D8(void *arg0, void *arg1, s32 depth, void *arg3, s32 *box, s32 arg5, s32 shade) {
    GoolObject *obj = arg0;
    GlyphTexture *glyph = (GlyphTexture *)arg1;
    u32 *ot = (u32 *)arg5;
    PolyGT4 *quad;
    s32 bottom;
    s32 left;
    u32 corner;
    u32 flag;
    u32 colour;
    u32 sz1;
    u32 sz2;
    u32 sz3;
    s32 index;
    u32 *slot;
    u32 uv;

    bottom = box[BOX_Y1];
    left = box[BOX_X0];
    GTE_WRITE_DATA(GTE_VXY0, VERTEX_XY(left, bottom));
    GTE_WRITE_DATA(GTE_VZ0, 0);
    GTE_WRITE_DATA(GTE_VXY1, VERTEX_XY(box[BOX_X1], bottom));
    GTE_WRITE_DATA(GTE_VZ1, 0);
    GTE_WRITE_DATA(GTE_VXY2, VERTEX_XY(left, box[BOX_Y0]));
    GTE_WRITE_DATA(GTE_VZ2, 0);
    Gte_Rtpt();
    GTE_READ_CONTROL(GTE_FLAG, flag);
    if (flag & GTE_FLAG_ERROR) {
        return;
    } else {
        /* Projected */
    }
    quad = (PolyGT4 *)g_BuildBuffer[0]->unk8;
    g_BuildBuffer[0]->unk8 = (s32)(quad + 1);
    if (shade != 0) {
        quad->colour0.c[0] = SHADED(glyph, obj, 0, 0);
        quad->colour0.c[1] = SHADED(glyph, obj, 0, 1);
        quad->colour0.c[2] = SHADED(glyph, obj, 0, 2);
        quad->colour1.c[0] = SHADED(glyph, obj, 1, 0);
        quad->colour1.c[1] = SHADED(glyph, obj, 1, 1);
        quad->colour1.c[2] = SHADED(glyph, obj, 1, 2);
        quad->colour2.c[0] = SHADED(glyph, obj, 2, 0);
        quad->colour2.c[1] = SHADED(glyph, obj, 2, 1);
        quad->colour2.c[2] = SHADED(glyph, obj, 2, 2);
        quad->colour3.c[0] = SHADED(glyph, obj, 3, 0);
        quad->colour3.c[1] = SHADED(glyph, obj, 3, 1);
        quad->colour3.c[2] = SHADED(glyph, obj, 3, 2);
    } else {
        colour = glyph->colour.word;
        quad->colour0.word = colour;
        quad->colour1.word = colour;
        quad->colour2.word = colour;
        quad->colour3.word = colour;
    }
    GTE_READ_DATA(GTE_SZ1, sz1);
    quad->colour0.c[PRIM_COLOUR_CODE] = glyph->colour.c[PRIM_COLOUR_CODE] | GT4_CODE;
    GTE_READ_DATA(GTE_SZ2, sz2);
    GTE_STORE_DATA(GTE_SXY2, quad->xy2);
    GTE_READ_DATA(GTE_SZ3, sz3);
    GTE_STORE_DATA(GTE_SXY1, quad->xy1);
    index = depth + OT_DEPTH_BIAS - D_80060884.a / 2 - ((sz1 + sz2 + sz3) >> SZ_SUM_SHIFT);
    if (index >= OT_LAST) {
        slot = &ot[OT_LAST];
    } else if (index < 0) {
        slot = ot;
    } else {
        slot = &ot[index];
    }
    quad->tag = *slot | GT4_TAG_LENGTH;
    *slot = (u32)quad & PRIM_ADDRESS_MASK;
    corner = VERTEX_XY(box[BOX_X1], box[BOX_Y0]);
    GTE_STORE_DATA(GTE_SXY0, quad->xy0);
    GTE_WRITE_DATA(GTE_VXY0, corner);
    GTE_WRITE_DATA(GTE_VZ0, 0);
    Gte_Rtps();
    quad->uv0_clut = glyph->uv0_clut;
    quad->uv1_tpage = glyph->uv1_tpage;
    uv = glyph->uv2_uv3;
    quad->uv2 = uv;
    quad->uv3 = uv >> GTE_HALF_BITS;
    GTE_STORE_DATA(GTE_SXY2, quad->xy3);
}
