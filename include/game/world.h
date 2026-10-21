#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include "common.h"

/* The world renderer: Gfx_DrawWorlds walks the zone's sort list and draws each polygon through the scene's mode, five
   entries called with a WorldPass (docs/TARGET.md, "The world renderer") */

/* A world as Sys_MainLoop copies it into the scratchpad. A vertex's word (x and y, a VXY pair, and colour bits) sits
   below vertices, word index going down, its halfword (z and colour bits) above, halfword index going up; a triangle's
   word and halfword sit the same way about triangles; a quad's two words go up from quads */
typedef struct WorldBlock {
    u32 offset_xy;
    s32 offset_z;
    u8 *triangles;
    u32 *quads;
    u8 *vertices;
    u8 *colours;
    u32 *animations;
    u32 *textures;
} WorldBlock;

/* A sort list entry's polygon: its world, kind and record */
typedef struct WorldRecord {
    WorldBlock *world;
    u32 kind;
    u32 first;
    u32 second;
} WorldRecord;

/* A polygon on its way through the scene's mode, and the frame it goes into: its world, material and vertices' words,
   the quad's fourth corner for its projector, the z each vertex was loaded with, the corners' colours, its depth and
   slot in the ordering table, the frame's next primitive, the lowest slot linked so far and what a primitive linked
   there points to, the table, and the textures' clock; the next entry and where its polygon is read to, which the
   projector does after its first corner's colour, as the retail code does while the GTE projects (in random trials a
   record may share memory with the frame's primitives, in the game never) */
typedef struct WorldPass {
    WorldBlock *world;
    u32 material;
    u32 word[4];
    u32 half[4];
    u32 quad_last_xy;
    s32 quad_last_z;
    s32 vertex_z[4];
    u32 colour[4];
    u32 depth;
    u32 *slot;
    u32 *next;
    u32 *lowest;
    u32 head;
    u32 *ot;
    s32 clock;
    WorldRecord *upcoming;
    u32 upcoming_entry;
} WorldPass;

typedef void (*WorldStep)(WorldPass *);

/* D_80062A18 as the world renderer reads it: the camera's position (24.8) and matrix (its rotation's five words and
   position, which the object renderers copy), the mode's five entries (its setup, triangle and quad projectors,
   triangle and quad drawers), and what the modes take: the sway's clock shift and size (D_80062A48, D_80062A4C, the
   camera's property 0x1B8), the lamps' far colour (D_80062A50), the fog's depth or the lamps' colour (D_80062A54), two
   lamps' positions (a VXY pair and z, D_80062A58 to D_80062A64), the player's position from the camera and the light's
   height and shade maps (D_80062AA4 to D_80062AB0), the depth fog's colour, shift and starting depth (D_80062AB4 to
   D_80062ABC, the camera's property 0x1DE), a tint's colour and depth (D_80062AC0, D_80062AC4) and the light's grid
   (D_80062AC8) */
typedef struct WorldScene {
    s32 *camera;
    u8 unk_04[8];
    u32 *camera_matrix;
    void (*setup)(struct WorldScene *);
    WorldStep project_triangle;
    WorldStep project_quad;
    WorldStep draw_triangle;
    WorldStep draw_quad;
    u8 unk_24[0xC];
    s32 sway_shift;
    s32 sway_scale;
    u32 lamp_far;
    u32 shade;
    u32 lamps[2][2];
    u8 unk_50[0x3C];
    s16 crash_x;
    s16 crash_y;
    s32 crash_z;
    u8 *light_heights;
    u8 *light_shades;
    u32 depth_fog_colour;
    s32 depth_fog_shift;
    s32 depth_fog_start;
    u32 tint_colour;
    s32 tint;
    u8 *light_grid;
} WorldScene;

#endif /* GAME_WORLD_H */
