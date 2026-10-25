#ifndef GAME_ZONE_H
#define GAME_ZONE_H

#include "common.h"

/* A zone's header, its entry's first item (CrashEdit's ZoneEntry): the worlds it draws, then the counts of the items
   after it, which come in that order (its info, its camera paths of three items each, its entities), and the zones
   it links to */
#define ZONE_WORLDS 8
#define ZONE_WORLD_BYTES 0x30
#define ZONE_CAMERA_ITEMS 3
/* The links' room: their flags follow them 8 words on, read by the same index */
#define ZONE_LINKS 8

typedef struct ZoneHeader {
    /* 0x000 */ s32 world_count;
    /* 0x004 */ u8 worlds[ZONE_WORLDS][ZONE_WORLD_BYTES];
    /* 0x184 */ s32 info_count;
    /* 0x188 */ s32 camera_count;
    /* 0x18C */ s32 entity_count;
    /* 0x190 */ s32 zone_count;
    /* The linked zones' EIDs, which Nsf_OpenEntry takes, and each link's flags */
    /* 0x194 */ u32 zones[ZONE_LINKS];
    /* 0x1B4 */ u32 zone_flags[ZONE_LINKS];
} ZoneHeader;

#endif /* GAME_ZONE_H */
