#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "common.h"

/* An entity: a zone item holding a list of properties by id (CrashEdit's Entity), an object to spawn or a camera
   path, which Ent_GetProperty searches */

/* The entity Ent_GetProperty searches next when a property is missing, 0 to stop */
#define ENT_NEXT_OFFSET 8
#define ENT_PROPERTY_COUNT_OFFSET 0xC
/* The property headers, sorted by id */
#define ENT_PROPERTIES_OFFSET 0x10
/* Where a property's offset counts from */
#define ENT_DATA_OFFSET 0xC

/* A property's type byte, as CrashEdit reads it: each row with its own value count (a halfword per row first, else one
   halfword for all), a meta value per row (a halfword; a camera path's node << 8 for its lists), the element type */
#define ENT_PROPERTY_SPARSE 0x40
#define ENT_PROPERTY_META 0x20
#define ENT_PROPERTY_TYPE_MASK 0x1F

/* The element types Ent_GetPropertyValue reads by size and sign; any other is read as a word */
#define ENT_TYPE_U8 1
#define ENT_TYPE_U16 2
#define ENT_TYPE_S8 17
#define ENT_TYPE_S16 18

/* Ent_GetProperty's row selection: the last row whose meta value is at or before the argument >> 8, the row whose meta
   value is equal to it, the row of an index, or the values interpolated between the row before and the next */
#define ENT_ROW_AT_OR_BEFORE 0
#define ENT_ROW_EXACT 1
#define ENT_ROW_INDEX 2
#define ENT_ROW_INTERPOLATED 3

/* Properties by CrashEdit's names */
#define ENT_PROP_POSITIONS 0x4B
#define ENT_PROP_ID 0x9F
#define ENT_PROP_SETTINGS 0xA4
#define ENT_PROP_TYPE 0xA9
#define ENT_PROP_SUBTYPE 0xAA
#define ENT_PROP_SLST 0x103
#define ENT_PROP_NEIGHBORS 0x109
#define ENT_PROP_DRAW_LIST_A 0x13B
#define ENT_PROP_DRAW_LIST_B 0x13C
#define ENT_PROP_CAMERA_INDEX 0x173
#define ENT_PROP_LOAD_LIST_A 0x208
#define ENT_PROP_LOAD_LIST_B 0x209

/* A property's 8-byte header, CrashEdit's layout */
typedef struct EntProperty {
    /* 0x0 */ s16 id;
    /* 0x2 */ u16 offset; /* of its data, from ENT_DATA_OFFSET */
    /* 0x4 */ u8 type; /* ENT_PROPERTY_SPARSE, ENT_PROPERTY_META and an element type */
    /* 0x5 */ u8 element_size;
    /* 0x6 */ u16 row_count; /* CrashEdit's "unknown", which its loaders read as the number of rows */
} EntProperty;

#endif /* GAME_ENTITY_H */
